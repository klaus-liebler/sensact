// Arbeitsbereich "Mobil" in der Desktop-Oberfläche: führt zur mobilen Bedienung unter /mobil (eigene Seite
// ohne Shell). Ein normaler Link, damit man ihn auf dem Telefon als Lesezeichen/Startbildschirm ablegen kann.
import { html } from "lit-html";
import { ScreenController } from "@klaus-liebler/web-components/typescript/screen_controller/screen_controller";
import { MOBILE_PATH } from "./mobile_app";

export class MobileLinkController extends ScreenController {
	Template = () => html`
		<h1>Mobile Bedienung</h1>
		<p>
			Lampen und Rollläden des ganzen Hauses, nach Stockwerken, für das Telefon: zwischen den Stockwerken
			waagrecht wischen (oder unten antippen), Lampe antippen = ein/aus, Dimmer gedrückt halten und
			hoch/runter ziehen = dimmen, Rollladen-Symbol gedrückt halten und ziehen = Position.
		</p>
		<p><a class="button primary" href=${MOBILE_PATH}>Mobile Bedienung öffnen</a></p>
		<p>Tipp: auf dem Telefon über „Zum Startbildschirm hinzufügen“ wie eine App ablegen.</p>
	`;
	OnMessage(_namespaceId: number, _messageTypeId: number, _view: DataView): void {}
	OnCreate(): void {}
	OnFirstStart(): void {}
	OnRestart(): void {}
	OnPause(): void {}
}
