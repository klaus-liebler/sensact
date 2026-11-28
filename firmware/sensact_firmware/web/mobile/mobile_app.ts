// Einstieg der mobilen Bedienung unter /mobil: Lampen und Rollläden des ganzen Hauses, nach Stockwerken.
// Läuft im selben Bundle wie die Desktop-Oberfläche (die Station liefert für jeden Pfad dieselbe Seite
// aus, s. webmanager_base.hh "/*"), startet aber statt der Anwendungs-Shell nur <sensact-mobile>.
//
// Datenquelle ist dieselbe wie bei "Apps der Station": die vom Builder aus dem Hausmodell erzeugte
// App-Liste (sensactapps.ts, alle Apps des Hauses mit local-Flag). Gesendet wird über die erzeugten
// Send*Command-Funktionen -- Kommandos an Apps anderer Stationen leitet die Station über CAN weiter,
// deren Zustand kennt sie aus den Status-Meldungen auf dem Bus.
import { sensact } from "@generated/wsprotocol_ts/ws-protocol";
import { ApplicationId } from "@generated/sensact_applicationids/applicationIds";
import * as cmd from "@generated/sensact_sendCommandImplementation/sendCommandImplementation";
import SensactAppsBuild from "@generated/sensact_appsbuilder/sensactapps";
import * as CFG from "@generated/runtimeconfig_ts";
import * as Apps from "@klaus-liebler/web-components-sensact/sensactapps_base";
import type { ISensactContext } from "@klaus-liebler/sensact-base/interfaces";
import { RoomDisplayName } from "@klaus-liebler/sensact-base/application_id_utils";
import { WS_URL } from "@klaus-liebler/web-components/typescript/utils/constants";
import { buildFloors, kindFromTechnology, parseAppName, type DeviceInfo, type Floor, type Room } from "./house";
import { DeviceState, NO_DATA, blindOpenToTarget, dimLevelToTarget } from "./device_state";
import { StationConnection } from "./connection";
import { SensactMobileElement, type MobileActions } from "./mobile_view";

export const MOBILE_PATH = "/mobil";

export function IsMobilePath(pathname: string): boolean {
	return pathname === MOBILE_PATH || pathname === MOBILE_PATH + "/";
}

/** Während des Ziehens höchstens so oft ein Dimm-Kommando (ms); der Endwert geht immer raus. */
const DIM_SEND_INTERVAL_MS = 120;
/** RequestStatus nimmt höchstens 128 IDs (sensact.cs) */
const MAX_IDS_PER_REQUEST = 128;

function uiClassOf(app: Apps.SensactApplication): "onoff" | "pwm" | "blind" | null {
	if (app instanceof Apps.SinglePwmApplication) return "pwm";
	if (app instanceof Apps.BlindApplication) return "blind";
	if (app instanceof Apps.OnOffApplication) return "onoff";
	return null;
}

function roomName(room: string, level: string): string {
	// "Wohnen · EG" -> "Wohnen" (das Stockwerk steht schon im Kopf)
	return RoomDisplayName(room, level).split(" · ")[0];
}

export class MobileController implements ISensactContext, MobileActions {
	private readonly states = new Map<number, DeviceState>();
	private readonly devices: DeviceInfo[];
	private readonly floors: Floor[];
	private readonly connection: StationConnection;
	private view!: SensactMobileElement;
	private dimTimers = new Map<number, { last: number; pending: number; handle: number }>();

	constructor() {
		this.devices = [];
		for (const { local, app } of SensactAppsBuild(this)) {
			const name = ApplicationId[app.applicationId];
			const parts = name ? parseAppName(name) : null;
			if (!parts) continue;
			const kind = kindFromTechnology(parts.technology, uiClassOf(app));
			if (!kind) continue;
			this.devices.push({ id: app.applicationId, name, label: app.ApplicationDescription, kind, level: parts.level, room: parts.room, local });
			this.states.set(app.applicationId, new DeviceState(kind));
		}
		this.floors = buildFloors(this.devices, roomName);
		this.connection = new StationConnection(
			WS_URL,
			(ns, type, view) => this.onFrame(ns, type, view),
			(s) => {
				this.view?.SetConnection(s);
				if (s === "online") this.requestStatus(this.devices);
			},
		);
	}

	Start() {
		prepareDocument();
		this.view = new SensactMobileElement();
		document.body.replaceChildren(this.view);
		this.view.Init(CFG.SENSACT_MODEL_NAME, this.floors, this.states, this);
		this.connection.Start();
		// Telefon wieder an: Verbindung prüfen und Zustände frisch holen (Push-Meldungen könnten fehlen)
		document.addEventListener("visibilitychange", () => {
			if (document.visibilityState !== "visible") return;
			this.connection.ReconnectNowIfOffline();
			if (this.connection.State === "online") this.requestStatus(this.devices);
		});
	}

	// ---- ISensactContext (von den Send*Command-Funktionen benutzt) ----

	SendCommandMessage(id: ApplicationId, c: sensact.Command, payload: DataView): void {
		const bytes = Array.from(new Uint8Array(payload.buffer, payload.byteOffset, payload.byteLength));
		const data = new Array<number>(8).fill(0);
		for (let i = 0; i < bytes.length && i < 8; i++) data[i] = bytes[i];
		this.connection.Send(sensact.RequestCommand.encode({ id, cmd: c, payload: { data, len: bytes.length } }, 0));
	}

	// ---- MobileActions ----

	toggle(d: DeviceInfo): void {
		const s = this.states.get(d.id)!;
		// sofort zeigen; die Meldung der Station bestätigt oder korrigiert
		s.on = !s.on;
		if (s.on && d.kind === "dimmer" && s.level === 0) s.level = 1;
		s.touchLocal();
		this.view.Refresh();
		navigator.vibrate?.(8);
		if (d.kind === "switch") {
			if (s.on) cmd.SendONCommand(d.id, 0, this);
			else cmd.SendOFFCommand(d.id, 0, this);
		} else {
			cmd.SendTOGGLECommand(d.id, this);
		}
	}

	dim(d: DeviceInfo, level: number, final: boolean): void {
		const s = this.states.get(d.id)!;
		s.level = level;
		s.on = level > 0;
		s.touchLocal();
		const target = dimLevelToTarget(level);
		let t = this.dimTimers.get(d.id);
		if (!t) {
			t = { last: 0, pending: -1, handle: -1 };
			this.dimTimers.set(d.id, t);
		}
		const send = (v: number) => {
			t!.last = Date.now();
			t!.pending = -1;
			cmd.SendSET_VERTICAL_TARGETCommand(d.id, v, this);
		};
		if (final) {
			if (t.handle >= 0) clearTimeout(t.handle);
			t.handle = -1;
			send(target);
			this.view.Refresh();
			return;
		}
		const wait = DIM_SEND_INTERVAL_MS - (Date.now() - t.last);
		if (wait <= 0) {
			send(target);
		} else {
			t.pending = target;
			if (t.handle < 0) {
				t.handle = window.setTimeout(() => {
					t!.handle = -1;
					if (t!.pending >= 0) send(t!.pending);
				}, wait);
			}
		}
	}

	blind(d: DeviceInfo, c: "up" | "stop" | "down"): void {
		navigator.vibrate?.(8);
		if (c === "up") cmd.SendUPCommand(d.id, 0, this);
		else if (c === "down") cmd.SendDOWNCommand(d.id, 0, this);
		else cmd.SendSTOPCommand(d.id, this);
	}

	blindTo(d: DeviceInfo, open: number): void {
		cmd.SendSET_VERTICAL_TARGETCommand(d.id, blindOpenToTarget(open), this);
	}

	allLightsOff(room: Room): void {
		navigator.vibrate?.(15);
		for (const d of room.lights) {
			const s = this.states.get(d.id)!;
			if (!s.on) continue;
			s.on = false;
			s.touchLocal();
			cmd.SendOFFCommand(d.id, 0, this);
		}
		this.view.Refresh();
	}

	floorShown(floor: Floor): void {
		// beim Wechsel das Stockwerk aktualisieren -- die Meldungen anderer Stationen kommen per Push,
		// aber was vor dem Verbinden passiert ist, kennt nur die Station
		if (this.connection.State === "online") this.requestStatus(floor.rooms.flatMap((r) => [...r.lights, ...r.blinds]));
	}

	// ---- Station -> Oberfläche ----

	private requestStatus(devices: DeviceInfo[]) {
		for (let i = 0; i < devices.length; i += MAX_IDS_PER_REQUEST) {
			const ids = devices.slice(i, i + MAX_IDS_PER_REQUEST).map((d) => ({ value: d.id }));
			this.connection.Send(sensact.RequestStatus.encode({ ids }, 0));
		}
	}

	private onFrame(namespaceId: number, messageTypeId: number, view: DataView) {
		if (namespaceId !== sensact.NAMESPACE_ID) return;
		if (messageTypeId === sensact.NotifyStatus.TYPE_ID) {
			const m = sensact.NotifyStatus.decode(view, 0);
			// Push mit "kein Zustand" ignorieren (wie SensactController), sonst flackert die Anzeige
			if (m.status.data[0] === NO_DATA) return;
			this.states.get(m.id)?.update(m.status.data);
		} else if (messageTypeId === sensact.ResponseStatus.TYPE_ID) {
			for (const st of sensact.ResponseStatus.decode(view, 0).states) this.states.get(st.id)?.update(st.status.data);
		} else {
			return;
		}
		this.view.Refresh();
	}
}

/** Viewport und Farben fürs Telefon; die index.html ist die der Desktop-Oberfläche. */
function prepareDocument() {
	const meta = (name: string, content: string) => {
		let m = document.head.querySelector<HTMLMetaElement>(`meta[name="${name}"]`);
		if (!m) {
			m = document.createElement("meta");
			m.name = name;
			document.head.appendChild(m);
		}
		m.content = content;
	};
	meta("viewport", "width=device-width, initial-scale=1, viewport-fit=cover");
	meta("theme-color", "#070f1c");
	meta("apple-mobile-web-app-capable", "yes");
	meta("mobile-web-app-capable", "yes");
	meta("apple-mobile-web-app-status-bar-style", "black-translucent");
	meta("apple-mobile-web-app-title", "Licht");
	document.title = "SensAct mobil";
	document.documentElement.removeAttribute("data-theme");
	document.documentElement.style.background = "#070f1c";
	document.body.style.margin = "0";
	document.body.style.background = "#070f1c";
	document.body.style.overscrollBehavior = "none";
}

export function StartMobileApp() {
	new MobileController().Start();
}
