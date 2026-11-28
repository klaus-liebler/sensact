// Geste der runden Bedienknöpfe: kurz antippen = Klick; gedrückt halten (HOLD_MS) und dann hoch/runter
// ziehen = Wert einstellen (Dimmer: Helligkeit, Rollladen: Öffnung).
//
// Warum erst halten und nicht sofort ziehen: Die Knöpfe füllen die ganze Seite. Würde jede senkrechte
// Bewegung auf einem Knopf dimmen, könnte man die Seite nicht mehr scrollen und nicht mehr zum nächsten
// Stockwerk wischen. Bewegt sich der Finger vor Ablauf der Haltezeit, gehört die Geste dem Browser
// (Scrollen/Wischen); ist die Haltezeit um, gehört sie dem Knopf, und touchmove wird blockiert, damit der
// Browser nicht nachträglich doch noch scrollt.
//
// Der Wert ändert sich relativ zur Startposition des Fingers (kein Sprung beim Ansetzen); RANGE_PX
// Fingerweg entsprechen dem vollen Bereich 0..1.

export const HOLD_MS = 280;
const SLOP_PX = 10;
export const RANGE_PX = 220;

export interface HoldDragHandlers {
	/** kurzer Tipp (auch Enter/Leertaste) */
	tap(): void;
	/** Haltezeit um: liefert den aktuellen Wert 0..1, ab dem gezogen wird; x/y = Finger (clientX/Y) */
	holdStart(x: number, y: number): number;
	/** Wert während des Ziehens, 0..1 */
	drag(value: number): void;
	/** Finger losgelassen (oder Geste abgebrochen), letzter Wert */
	release(value: number): void;
	/** gedrückt, Haltezeit läuft (für die Rückmeldung am Knopf) */
	arming?(on: boolean): void;
}

type Phase = "idle" | "pending" | "dragging";

export function attachHoldDrag(el: HTMLElement, h: HoldDragHandlers): () => void {
	let phase: Phase = "idle";
	let pointerId = -1;
	let startX = 0;
	let startY = 0;
	let startValue = 0;
	let value = 0;
	let timer = -1;
	// nach Halten oder Bewegen keinen Klick mehr auslösen (der Browser schickt ihn trotzdem)
	let suppressClick = false;

	const clearTimer = () => {
		if (timer >= 0) clearTimeout(timer);
		timer = -1;
	};

	const toIdle = () => {
		clearTimer();
		if (phase === "pending") h.arming?.(false);
		phase = "idle";
		pointerId = -1;
	};

	const onDown = (e: PointerEvent) => {
		if (e.button !== 0 || phase !== "idle") return;
		phase = "pending";
		pointerId = e.pointerId;
		startX = e.clientX;
		startY = e.clientY;
		suppressClick = false;
		h.arming?.(true);
		timer = window.setTimeout(() => {
			timer = -1;
			if (phase !== "pending") return;
			h.arming?.(false);
			phase = "dragging";
			suppressClick = true;
			startValue = value = h.holdStart(startX, startY);
			try {
				el.setPointerCapture(pointerId);
			} catch {
				/* Pointer schon weg */
			}
			navigator.vibrate?.(12);
		}, HOLD_MS);
	};

	const onMove = (e: PointerEvent) => {
		if (e.pointerId !== pointerId) return;
		if (phase === "pending") {
			if (Math.hypot(e.clientX - startX, e.clientY - startY) > SLOP_PX) {
				// Scrollen oder Wischen: die Geste gehört dem Browser
				suppressClick = true;
				toIdle();
			}
		} else if (phase === "dragging") {
			value = Math.max(0, Math.min(1, startValue + (startY - e.clientY) / RANGE_PX));
			h.drag(value);
		}
	};

	const onUp = (e: PointerEvent) => {
		if (e.pointerId !== pointerId) return;
		if (phase === "dragging") h.release(value);
		toIdle();
	};

	const onCancel = (e: PointerEvent) => {
		if (e.pointerId !== pointerId) return;
		suppressClick = true;
		if (phase === "dragging") h.release(value);
		toIdle();
	};

	// Muss nicht-passiv sein: nur so lässt sich das Scrollen des Browsers während des Ziehens verhindern.
	const onTouchMove = (e: TouchEvent) => {
		if (phase === "dragging" && e.cancelable) e.preventDefault();
	};

	const onClick = (e: MouseEvent) => {
		if (suppressClick) {
			suppressClick = false;
			e.preventDefault();
			e.stopPropagation();
			return;
		}
		h.tap();
	};

	const onContextMenu = (e: Event) => e.preventDefault();

	el.addEventListener("pointerdown", onDown);
	el.addEventListener("pointermove", onMove);
	el.addEventListener("pointerup", onUp);
	el.addEventListener("pointercancel", onCancel);
	el.addEventListener("lostpointercapture", onCancel);
	el.addEventListener("touchmove", onTouchMove, { passive: false });
	el.addEventListener("click", onClick);
	el.addEventListener("contextmenu", onContextMenu);
	return () => {
		toIdle();
		el.removeEventListener("pointerdown", onDown);
		el.removeEventListener("pointermove", onMove);
		el.removeEventListener("pointerup", onUp);
		el.removeEventListener("pointercancel", onCancel);
		el.removeEventListener("lostpointercapture", onCancel);
		el.removeEventListener("touchmove", onTouchMove);
		el.removeEventListener("click", onClick);
		el.removeEventListener("contextmenu", onContextMenu);
	};
}
