// Hausmodell der mobilen Bedienung: aus der (generierten) App-Liste der Station die Lampen und Rollläden
// heraussuchen und nach Stockwerk und Raum ordnen. Ebene und Raum stehen im App-Namen
// (TYP__EBENE_RAUM_GERAET, s. docs/konzepte/02-hausmodell.md) -- dieselbe Quelle wie die Gruppierung
// "Nach Räumen" der Desktop-Oberfläche (sensact-base/application_id_utils.ts).
// Bewusst ohne Abhängigkeit auf generierten Code, damit es ohne Station/Builder testbar bleibt.

export type DeviceKind = "dimmer" | "switch" | "blind";

export interface DeviceInfo {
	/** App-Nummer (ApplicationId) */
	id: number;
	/** App-Name, z. B. PWM___L1_LVNG_S */
	name: string;
	/** Beschreibung aus dem Hausmodell, z. B. "Spots" */
	label: string;
	kind: DeviceKind;
	level: string;
	room: string;
	/** App läuft auf dieser Station (sonst geht das Kommando über CAN) */
	local: boolean;
}

export interface Room {
	key: string;
	name: string;
	lights: DeviceInfo[];
	blinds: DeviceInfo[];
}

export interface Floor {
	key: string;
	/** "Erdgeschoss" */
	name: string;
	/** "EG" für die Stockwerksleiste */
	short: string;
	rooms: Room[];
}

/** Bestandteile eines App-Namens, z. B. PWM___L1_LVNG_S -> { technology: "PWM", level: "L1", room: "LVNG" } */
export function parseAppName(name: string): { technology: string; level: string; room: string } | null {
	const parts = name.split("_").filter((p) => p.length > 0);
	if (parts.length < 3) return null;
	return { technology: parts[0], level: parts[1], room: parts[2] };
}

/** Welche Technik-Präfixe die mobile Oberfläche zeigt (Netzteile, Pumpen, Taster ... nicht). */
export function kindFromTechnology(technology: string, uiClass: "onoff" | "pwm" | "blind" | null): DeviceKind | null {
	switch (uiClass) {
		case "pwm":
			return "dimmer";
		case "blind":
			return technology === "BLIND" ? "blind" : null;
		case "onoff":
			return technology === "POWIT" ? "switch" : null;
		default:
			return null;
	}
}

// Reihenfolge der Stockwerke in der Leiste = Reihenfolge beim Wischen (von unten nach oben, außen zuletzt)
const FLOORS: Array<{ key: string; name: string; short: string }> = [
	{ key: "L0", name: "Keller", short: "KG" },
	{ key: "L1", name: "Erdgeschoss", short: "EG" },
	{ key: "LS", name: "Treppenhaus", short: "TH" },
	{ key: "L2", name: "Obergeschoss", short: "OG" },
	{ key: "L3", name: "Dachgeschoss", short: "DG" },
	{ key: "LX", name: "Außen", short: "Außen" },
];

/** Stockwerk, das beim ersten Öffnen gezeigt wird. */
export const DEFAULT_FLOOR = "L1";

export function buildFloors(devices: DeviceInfo[], roomName: (room: string, level: string) => string): Floor[] {
	const byFloor = new Map<string, Map<string, Room>>();
	for (const d of devices) {
		let rooms = byFloor.get(d.level);
		if (!rooms) {
			rooms = new Map();
			byFloor.set(d.level, rooms);
		}
		let room = rooms.get(d.room);
		if (!room) {
			room = { key: `${d.level}_${d.room}`, name: roomName(d.room, d.level), lights: [], blinds: [] };
			rooms.set(d.room, room);
		}
		(d.kind === "blind" ? room.blinds : room.lights).push(d);
	}
	const known = FLOORS.filter((f) => byFloor.has(f.key));
	const unknown = [...byFloor.keys()]
		.filter((k) => !FLOORS.some((f) => f.key === k))
		.sort()
		.map((k) => ({ key: k, name: k, short: k }));
	return [...known, ...unknown].map((f) => ({
		...f,
		// Räume alphabetisch nach Anzeigenamen; die Geräte darin in der Reihenfolge des Hausmodells
		rooms: [...byFloor.get(f.key)!.values()].sort((a, b) => a.name.localeCompare(b.name, "de")),
	}));
}
