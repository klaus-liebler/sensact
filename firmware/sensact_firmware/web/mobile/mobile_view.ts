// <sensact-mobile>: die mobile Bedienoberfläche (Stockwerke, Räume, Lampen, Rollläden).
// Shadow DOM, damit die globalen Stile der Desktop-Shell (base.css, app.css -- die stecken im selben
// Bundle) hier nichts verändern. Kennt kein Protokoll: Bedienungen gehen als Aufrufe an MobileActions,
// Zustände kommen über Refresh() aus der gemeinsamen Map id -> DeviceState.
import { html, render, nothing, svg } from "lit-html";
import { ref } from "lit-html/directives/ref.js";
import { styleMap } from "lit-html/directives/style-map.js";
import { repeat } from "lit-html/directives/repeat.js";
import type { DeviceInfo, Floor, Room } from "./house";
import { DeviceState } from "./device_state";
import { attachHoldDrag, RANGE_PX } from "./hold_drag";
import css from "./mobile.css?inline";

export type ConnectionIndicator = "connecting" | "online" | "offline";

export interface MobileActions {
	toggle(d: DeviceInfo): void;
	/** final = Finger losgelassen */
	dim(d: DeviceInfo, level: number, final: boolean): void;
	blind(d: DeviceInfo, cmd: "up" | "stop" | "down"): void;
	blindTo(d: DeviceInfo, open: number): void;
	allLightsOff(room: Room): void;
	floorShown(floor: Floor): void;
}

const STORAGE_KEY = "sensact.mobile.floor";

function loadFloor(): string | null {
	try {
		return localStorage.getItem(STORAGE_KEY);
	} catch {
		return null;
	}
}
function saveFloor(key: string) {
	try {
		localStorage.setItem(STORAGE_KEY, key);
	} catch {
		/* privater Modus o. ä. */
	}
}

const bulbIcon = svg`<path d="M12 2.5a6.5 6.5 0 0 0-3.9 11.7c.6.5.9 1.1.9 1.8V17h6v-1c0-.7.3-1.3.9-1.8A6.5 6.5 0 0 0 12 2.5Z" fill="currentColor" opacity=".9"/><path d="M9.5 19h5M10.3 21.5h3.4" stroke="currentColor" stroke-width="1.8" stroke-linecap="round"/>`;
const ceilingIcon = svg`<path d="M12 2v5" stroke="currentColor" stroke-width="1.8" stroke-linecap="round"/><path d="M4.5 15a7.5 7.5 0 0 1 15 0Z" fill="currentColor" opacity=".9"/><path d="M9.5 18.5a2.5 2.5 0 0 0 5 0" stroke="currentColor" stroke-width="1.8" stroke-linecap="round" fill="none"/>`;
const upIcon = svg`<path d="M6 14.5 12 8.5l6 6" stroke="currentColor" stroke-width="2.4" fill="none" stroke-linecap="round" stroke-linejoin="round"/>`;
const downIcon = svg`<path d="M6 9.5l6 6 6-6" stroke="currentColor" stroke-width="2.4" fill="none" stroke-linecap="round" stroke-linejoin="round"/>`;
const stopIcon = svg`<rect x="7.5" y="7.5" width="9" height="9" rx="1.5" fill="currentColor"/>`;
const desktopIcon = svg`<rect x="3" y="4" width="18" height="12" rx="2" stroke="currentColor" stroke-width="1.8" fill="none"/><path d="M9 20h6M12 16v4" stroke="currentColor" stroke-width="1.8" stroke-linecap="round"/>`;
const icon = (content: unknown, cls = "icon") => html`<svg class=${cls} viewBox="0 0 24 24" aria-hidden="true">${content}</svg>`;

interface DragOverlay {
	device: DeviceInfo;
	kind: "dim" | "blind";
	value: number;
	/** obere Kante der Skala, links der Skala (px, viewport) */
	top: number;
	left: number;
	/** Wertanzeige links oder rechts der Skala */
	labelLeft: boolean;
}

export class SensactMobileElement extends HTMLElement {
	private floors: Floor[] = [];
	private states = new Map<number, DeviceState>();
	private actions!: MobileActions;
	private houseName = "";
	private current = 0;
	private connection: ConnectionIndicator = "connecting";
	private overlay: DragOverlay | null = null;
	private root: ShadowRoot;
	private pager: HTMLElement | null = null;
	private renderQueued = false;
	private attached = new WeakSet<Element>();

	constructor() {
		super();
		this.root = this.attachShadow({ mode: "open" });
	}

	Init(title: string, floors: Floor[], states: Map<number, DeviceState>, actions: MobileActions) {
		this.houseName = title;
		this.floors = floors;
		this.states = states;
		this.actions = actions;
		const saved = loadFloor();
		const idx = floors.findIndex((f) => f.key === saved);
		this.current = idx >= 0 ? idx : Math.max(0, floors.findIndex((f) => f.key === "L1"));
		this.renderNow();
		// erst nach dem Layout an die richtige Seite springen (ohne Animation)
		requestAnimationFrame(() => this.scrollToFloor(this.current, false));
		window.addEventListener("resize", () => this.scrollToFloor(this.current, false));
		this.floorShown();
	}

	SetConnection(c: ConnectionIndicator) {
		this.connection = c;
		this.Refresh();
	}

	get CurrentFloor(): Floor | undefined {
		return this.floors[this.current];
	}

	/** nach Zustandsänderungen; gebündelt auf den nächsten Frame */
	Refresh() {
		if (this.renderQueued) return;
		this.renderQueued = true;
		requestAnimationFrame(() => {
			this.renderQueued = false;
			this.renderNow();
		});
	}

	private renderNow() {
		render(this.template(), this.root);
	}

	private state(d: DeviceInfo): DeviceState {
		let s = this.states.get(d.id);
		if (!s) {
			s = new DeviceState(d.kind);
			this.states.set(d.id, s);
		}
		return s;
	}

	// ---- Stockwerke ----------------------------------------------------------------------------------

	private scrollToFloor(i: number, smooth = true) {
		if (!this.pager) return;
		this.pager.scrollTo({ left: i * this.pager.clientWidth, behavior: smooth ? "smooth" : "auto" });
		if (!smooth) this.setCurrent(i);
	}

	private setCurrent(i: number) {
		if (i === this.current || i < 0 || i >= this.floors.length) return;
		this.current = i;
		saveFloor(this.floors[i].key);
		navigator.vibrate?.(6);
		this.floorShown();
		this.Refresh();
	}

	private floorShown() {
		const f = this.floors[this.current];
		if (f) this.actions.floorShown(f);
	}

	private onPagerScroll() {
		const p = this.pager!;
		const i = Math.round(p.scrollLeft / Math.max(1, p.clientWidth));
		// Kopfzeile schon während des Wischens umschalten, sobald die Seite mehr als halb drin ist
		if (i !== this.current) this.setCurrent(i);
	}

	private onKey(e: KeyboardEvent) {
		if (e.key === "ArrowRight" && !e.altKey) this.scrollToFloor(Math.min(this.current + 1, this.floors.length - 1));
		else if (e.key === "ArrowLeft" && !e.altKey) this.scrollToFloor(Math.max(this.current - 1, 0));
	}

	// ---- Gesten der Knöpfe ---------------------------------------------------------------------------

	private bindLamp(el: Element | undefined, d: DeviceInfo) {
		if (!el || this.attached.has(el)) return;
		this.attached.add(el);
		const btn = el as HTMLElement;
		if (d.kind !== "dimmer") {
			btn.addEventListener("click", () => this.actions.toggle(d));
			return;
		}
		attachHoldDrag(btn, {
			tap: () => this.actions.toggle(d),
			arming: (on) => btn.classList.toggle("arming", on),
			holdStart: (_x, y) => {
				const s = this.state(d);
				const start = s.on ? s.level : 0;
				this.openOverlay(btn, d, "dim", start, y);
				return start;
			},
			drag: (v) => {
				this.updateOverlay(v);
				this.actions.dim(d, v, false);
			},
			release: (v) => {
				this.overlay = null;
				this.actions.dim(d, v, true);
				this.Refresh();
			},
		});
		btn.addEventListener("keydown", (e: KeyboardEvent) => {
			if (e.key !== "ArrowUp" && e.key !== "ArrowDown") return;
			e.preventDefault();
			const s = this.state(d);
			const v = Math.max(0, Math.min(1, (s.on ? s.level : 0) + (e.key === "ArrowUp" ? 0.1 : -0.1)));
			this.actions.dim(d, v, true);
		});
	}

	private bindShutter(el: Element | undefined, d: DeviceInfo) {
		if (!el || this.attached.has(el)) return;
		this.attached.add(el);
		const btn = el as HTMLElement;
		attachHoldDrag(btn, {
			tap: () => {
				const s = this.state(d);
				if (s.movingUp || s.movingDown) this.actions.blind(d, "stop");
				else this.actions.blind(d, s.open > 0.5 ? "down" : "up");
			},
			arming: (on) => btn.classList.toggle("arming", on),
			holdStart: (_x, y) => {
				const start = this.state(d).open;
				this.openOverlay(btn, d, "blind", start, y);
				return start;
			},
			drag: (v) => this.updateOverlay(v),
			release: (v) => {
				this.overlay = null;
				this.actions.blindTo(d, v);
				this.Refresh();
			},
		});
	}

	private openOverlay(btn: HTMLElement, d: DeviceInfo, kind: "dim" | "blind", value: number, fingerY: number) {
		const r = btn.getBoundingClientRect();
		const vh = window.innerHeight;
		const W = 64;
		// Skala so legen, dass der Griff unter dem Finger startet und ihm 1:1 folgt; nur am Bildrand verschoben
		let top = fingerY - (1 - value) * RANGE_PX;
		top = Math.max(12, Math.min(vh - RANGE_PX - 12, top));
		const cx = r.left + r.width / 2;
		const left = Math.max(8, Math.min(window.innerWidth - W - 8, cx - W / 2));
		this.overlay = { device: d, kind, value, top, left, labelLeft: cx > window.innerWidth / 2 };
		this.renderNow();
	}

	private updateOverlay(v: number) {
		if (!this.overlay) return;
		this.overlay.value = v;
		this.renderNow();
	}

	// ---- Templates -----------------------------------------------------------------------------------

	private template() {
		const f = this.floors[this.current];
		const prev = this.floors[this.current - 1];
		const next = this.floors[this.current + 1];
		const lightsOn = f ? f.rooms.flatMap((r) => r.lights).filter((d) => this.state(d).on).length : 0;
		const connText = { online: "verbunden", connecting: "verbinde …", offline: "keine Verbindung" }[this.connection];
		return html`
			<style>
				${css}
			</style>
			<div class="app ${this.overlay ? "dragging" : ""}" @keydown=${(e: KeyboardEvent) => this.onKey(e)}>
				<header class="top">
					<button class="nav prev" ?disabled=${!prev} @click=${() => this.scrollToFloor(this.current - 1)} aria-label=${prev ? `zu ${prev.name}` : "kein Stockwerk darunter"}>
						${prev ? html`<span class="chev">‹</span>${prev.short}` : nothing}
					</button>
					<div class="title">
						<span class="floor-name">${f?.name ?? "–"}</span>
						<span class="sub">${lightsOn === 0 ? "alle Lichter aus" : lightsOn === 1 ? "1 Licht an" : `${lightsOn} Lichter an`}</span>
					</div>
					<button class="nav next" ?disabled=${!next} @click=${() => this.scrollToFloor(this.current + 1)} aria-label=${next ? `zu ${next.name}` : "kein Stockwerk darüber"}>
						${next ? html`${next.short}<span class="chev">›</span>` : nothing}
					</button>
				</header>
				<div class="status-row">
					<span class="conn ${this.connection}" title="WebSocket-Verbindung zur Station"><i></i>${connText}</span>
					<span class="house">${this.houseName}</span>
					<a class="desktop" href="/" title="Vollständige Weboberfläche">${icon(desktopIcon, "icon small")}</a>
				</div>
				${this.connection === "offline" ? html`<div class="banner">Keine Verbindung zur Station – verbinde neu …</div>` : nothing}
				<main
					class="pager"
					tabindex="-1"
					${ref((el) => {
						if (el && el !== this.pager) {
							this.pager = el as HTMLElement;
							this.pager.addEventListener("scroll", () => this.onPagerScroll(), { passive: true });
						}
					})}
				>
					${repeat(
						this.floors,
						(fl) => fl.key,
						(fl, i) => html`
							<section class="page" aria-label=${fl.name} aria-hidden=${i !== this.current ? "true" : "false"}>
								${fl.rooms.map((r) => this.roomTemplate(r))}
								<div class="page-end"></div>
							</section>
						`,
					)}
				</main>
				<nav class="floors" role="tablist" aria-label="Stockwerk">
					${this.floors.map(
						(fl, i) => html`<button role="tab" aria-selected=${i === this.current ? "true" : "false"} class=${i === this.current ? "active" : ""} @click=${() => this.scrollToFloor(i)}>
							${fl.short}
						</button>`,
					)}
				</nav>
				${this.overlayTemplate()}
			</div>
		`;
	}

	private roomTemplate(r: Room) {
		const anyOn = r.lights.some((d) => this.state(d).on);
		return html`
			<article class="room">
				<header>
					<h2>${r.name}</h2>
					${r.lights.length > 1 ? html`<button class="pill ${anyOn ? "" : "hidden"}" @click=${() => this.actions.allLightsOff(r)} ?disabled=${!anyOn}>Alle aus</button>` : nothing}
				</header>
				${r.lights.length ? html`<div class="lamps">${r.lights.map((d) => this.lampTemplate(d))}</div>` : nothing}
				${r.blinds.length ? html`<div class="blinds">${r.blinds.map((d) => this.blindTemplate(d))}</div>` : nothing}
			</article>
		`;
	}

	private lampTemplate(d: DeviceInfo) {
		const s = this.state(d);
		const dimmer = d.kind === "dimmer";
		const level = dimmer ? (s.on ? s.level : 0) : s.on ? 1 : 0;
		const dragging = this.overlay?.device === d;
		const shown = dragging ? this.overlay!.value : level;
		const pct = Math.round(shown * 100);
		const stateText = !s.known ? "Zustand unbekannt" : s.on || (dragging && shown > 0) ? (dimmer ? `an, ${pct} %` : "an") : "aus";
		return html`
			<div class="lamp-cell">
				<button
					class="lamp ${dimmer ? "dimmer" : "switch"} ${s.on || (dragging && shown > 0) ? "on" : ""} ${s.known ? "" : "unknown"} ${dragging ? "active" : ""} ${dimmer && (s.on || dragging) && s.known ? "has-pct" : ""}"
					style=${styleMap({ "--level": String(shown) })}
					aria-pressed=${s.on ? "true" : "false"}
					aria-label="${d.label}: ${stateText}${dimmer ? ". Halten und ziehen zum Dimmen" : ""}"
					${ref((el) => this.bindLamp(el, d))}
				>
					${icon(dimmer ? bulbIcon : ceilingIcon)}
					${dimmer && (s.on || dragging) && s.known ? html`<span class="pct">${pct}</span>` : nothing}
				</button>
				<span class="lamp-label">${d.label || d.name}</span>
			</div>
		`;
	}

	private blindTemplate(d: DeviceInfo) {
		const s = this.state(d);
		const dragging = this.overlay?.device === d;
		const open = dragging ? this.overlay!.value : s.open;
		const pctOpen = Math.round(open * 100);
		const status = dragging
			? `auf ${pctOpen} % fahren`
			: !s.known
				? "Position unbekannt"
				: s.movingUp
					? "fährt auf …"
					: s.movingDown
						? "fährt zu …"
						: pctOpen >= 99
							? "offen"
							: pctOpen <= 1
								? "geschlossen"
								: `${pctOpen} % offen`;
		return html`
			<div class="blind ${s.movingUp ? "up" : ""} ${s.movingDown ? "down" : ""} ${s.known ? "" : "unknown"}">
				<button
					class="shutter ${dragging ? "active" : ""}"
					style=${styleMap({ "--closed": String(1 - open) })}
					aria-label="${d.label}: ${status}. Antippen fährt, halten und ziehen stellt die Position ein"
					${ref((el) => this.bindShutter(el, d))}
				>
					<span class="slats"></span>
				</button>
				<div class="blind-text">
					<span class="name">${d.label || d.name}</span>
					<span class="pos">${status}</span>
				</div>
				<div class="blind-btns">
					<button class="round-sm ${s.movingUp ? "lit" : ""}" aria-label="${d.label} auf" @click=${() => this.actions.blind(d, "up")}>${icon(upIcon)}</button>
					<button class="round-sm" aria-label="${d.label} stopp" @click=${() => this.actions.blind(d, "stop")}>${icon(stopIcon)}</button>
					<button class="round-sm ${s.movingDown ? "lit" : ""}" aria-label="${d.label} ab" @click=${() => this.actions.blind(d, "down")}>${icon(downIcon)}</button>
				</div>
			</div>
		`;
	}

	private overlayTemplate() {
		const o = this.overlay;
		if (!o) return nothing;
		const pct = Math.round(o.value * 100);
		const handleY = (1 - o.value) * RANGE_PX;
		const label = o.kind === "dim" ? (pct === 0 ? "aus" : `${pct} %`) : `${pct} % offen`;
		return html`
			<div class="scrim"></div>
			<div class="slider ${o.kind}" style=${styleMap({ top: `${o.top}px`, left: `${o.left}px`, height: `${RANGE_PX}px` })} aria-hidden="true">
				<div class="fill" style=${styleMap({ height: `${o.value * RANGE_PX}px` })}></div>
				<div class="handle" style=${styleMap({ top: `${handleY}px` })}></div>
				<div class="value ${o.labelLeft ? "left" : "right"}" style=${styleMap({ top: `${handleY}px` })}>
					<strong>${label}</strong><span>${o.device.label}</span>
				</div>
			</div>
		`;
	}
}

if (!customElements.get("sensact-mobile")) customElements.define("sensact-mobile", SensactMobileElement);
