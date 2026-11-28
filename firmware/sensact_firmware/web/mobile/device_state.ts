// Zustand je Lampe/Rollladen, wie ihn die Station meldet (ResponseStatus/NotifyStatus, 4 x uint16).
// Status-Belegung: espidf-components/sensact-applicationmodel/README.md.
import type { DeviceKind } from "./house";

/** 0xFFFF in Slot 0 = die Station kennt (noch) keinen Zustand dieser App. */
export const NO_DATA = 0xffff;
/** Solange nach einer eigenen Bedienung eingehende Meldungen nicht die Anzeige überschreiben (ms). */
const LOCAL_HOLD_MS = 1500;

export enum BlindMotion {
	ENERGY_SAVE = 0,
	STOP = 1,
	PREPARE_UP = 2,
	PREPARE_DOWN = 3,
	UP = 4,
	DOWN = 5,
}

export class DeviceState {
	/** schon ein Zustand von der Station gekommen? */
	known = false;
	/** Lampe an */
	on = false;
	/** Helligkeit 0..1 (Dimmer: Zielhelligkeit, wenn an, sonst die gemerkte) */
	level = 0;
	/** Rollladen: Öffnung 0..1 (0 = zu, 1 = offen) */
	open = 0;
	motion = BlindMotion.STOP;
	private localUntil = 0;

	constructor(public readonly kind: DeviceKind) {}

	/** Nach eigener Bedienung: Anzeige kurz gegen veraltete Meldungen schützen. */
	touchLocal(now = Date.now()) {
		this.localUntil = now + LOCAL_HOLD_MS;
	}

	update(s: ArrayLike<number>, now = Date.now()): void {
		if (s[0] === NO_DATA) {
			this.known = false;
			return;
		}
		this.known = true;
		const protectedByLocal = now < this.localUntil;
		switch (this.kind) {
			case "switch":
				// cOnOff: [1] = an/aus
				if (!protectedByLocal) this.on = s[1] !== 0;
				break;
			case "dimmer":
				// cSinglePWM: [0] an, [1] gemerkte Helligkeit, [3] Zielhelligkeit
				if (!protectedByLocal) {
					this.on = s[0] !== 0;
					this.level = (this.on && s[3] > 0 ? s[3] : s[1]) / 65535;
				}
				break;
			case "blind":
				// cBlind: [0] Bewegung, [1] Position in % (0 = zu, 100 = offen)
				this.motion = s[0] as BlindMotion;
				this.open = Math.max(0, Math.min(100, s[1])) / 100;
				break;
		}
	}

	get movingUp() {
		return this.motion === BlindMotion.UP || this.motion === BlindMotion.PREPARE_UP;
	}
	get movingDown() {
		return this.motion === BlindMotion.DOWN || this.motion === BlindMotion.PREPARE_DOWN;
	}
}

/** Dimmer: 0..1 -> SET_VERTICAL_TARGET (0..65535, 0 = aus) */
export function dimLevelToTarget(level: number): number {
	return Math.round(Math.max(0, Math.min(1, level)) * 65535);
}

/** Rollladen: Öffnung 0..1 -> SET_VERTICAL_TARGET. 16384..49151 = Teilöffnung 0..100 % (cBlind); ganz
 *  am Rand die Werte aus dem Überfahrbereich (wie der Rollladen-Timer), damit "ganz zu/auf" sicher bis zum
 *  Endschalter fährt, auch wenn die geschätzte Position etwas daneben liegt. */
export function blindOpenToTarget(open: number): number {
	if (open <= 0.02) return 1000;
	if (open >= 0.98) return 65000;
	return Math.round(16384 + open * (49151 - 16384));
}
