// Schlanke WebSocket-Verbindung zur Station für die mobile Bedienung -- dieselbe Verbindung
// (/webmanager_ws, Session-Cookie) und dasselbe Rahmenformat wie die Desktop-Oberfläche
// (@klaus-liebler/web-components/app_controller.ts), aber ohne Anwendungs-Shell, Dialoge und Spinner.
// Verhalten beim Verbindungsverlust wie dort: alle 1,5 s neu verbinden; nach einigen Fehlversuchen die
// Seite neu laden, denn ist die Session nach einem Neustart der Station weg, zeigt nur der Seitenaufruf
// das Login-Formular (der WebSocket-Handshake verrät JS den Grund nicht).
import { FLAG_ERROR, HEADER_SIZE, readHeader } from "@generated/wsprotocol_ts/ws-protocol";

export type ConnectionState = "connecting" | "online" | "offline";

const RECONNECT_INTERVAL_MS = 1500;
const RELOAD_AFTER_ATTEMPTS = 4;

export class StationConnection {
	private socket: WebSocket | null = null;
	private buffer: Uint8Array[] = [];
	private attempts = 0;
	private reconnectHandle = -1;
	private state: ConnectionState = "connecting";

	constructor(
		private readonly url: string,
		private readonly onFrame: (namespaceId: number, messageTypeId: number, view: DataView) => void,
		private readonly onState: (state: ConnectionState) => void,
	) {}

	get State() {
		return this.state;
	}

	Start() {
		this.connect();
	}

	/** Fertig gerahmter Frame (encode(..., 0)); wird gepuffert, solange keine Verbindung besteht. */
	Send(frame: Uint8Array) {
		if (this.socket?.readyState === WebSocket.OPEN) this.socket.send(frame);
		else this.buffer.push(frame);
	}

	private setState(s: ConnectionState) {
		if (s === this.state) return;
		this.state = s;
		this.onState(s);
	}

	private connect() {
		const socket = new WebSocket(this.url);
		socket.binaryType = "arraybuffer";
		this.socket = socket;
		socket.onopen = () => {
			this.attempts = 0;
			this.setState("online");
			const pending = this.buffer;
			this.buffer = [];
			for (const f of pending) socket.send(f);
		};
		socket.onmessage = (ev: MessageEvent) => {
			if (!(ev.data instanceof ArrayBuffer) || ev.data.byteLength < HEADER_SIZE) return;
			const view = new DataView(ev.data);
			const header = readHeader(view);
			if (header.flags & FLAG_ERROR) {
				console.warn(`Fehlerantwort der Station für ${header.namespaceId}/${header.messageTypeId}`);
				return;
			}
			this.onFrame(header.namespaceId, header.messageTypeId, view);
		};
		socket.onclose = (ev) => {
			if (this.socket !== socket) return;
			this.setState("offline");
			if (ev.code === 1000) return;
			this.scheduleReconnect();
		};
		// onerror: nur informativ, onclose folgt immer
		socket.onerror = () => console.warn("WebSocket-Fehler");
	}

	private scheduleReconnect() {
		if (this.reconnectHandle >= 0) return;
		this.attempts++;
		if (this.attempts > RELOAD_AFTER_ATTEMPTS) {
			window.location.reload();
			return;
		}
		this.reconnectHandle = window.setTimeout(() => {
			this.reconnectHandle = -1;
			this.setState("connecting");
			this.connect();
		}, RECONNECT_INTERVAL_MS);
	}

	/** z. B. nach dem Aufwecken des Telefons: nicht erst auf den nächsten Versuch warten. */
	ReconnectNowIfOffline() {
		if (this.socket && this.socket.readyState !== WebSocket.CLOSED) return;
		if (this.reconnectHandle >= 0) {
			clearTimeout(this.reconnectHandle);
			this.reconnectHandle = -1;
		}
		this.setState("connecting");
		this.connect();
	}
}
