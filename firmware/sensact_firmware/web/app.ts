import { html } from "lit-html";
import { AppController } from "@klaus-liebler/web-components"
import type { IChatbot } from "@klaus-liebler/web-components"
import { OverviewScreenController } from "@klaus-liebler/web-components/typescript/screen_controller/overview_controller"
import { SystemController } from "@klaus-liebler/web-components/typescript/screen_controller/systeminfo_controller"
import { WifimanagerController } from "@klaus-liebler/web-components/typescript/screen_controller/wifimanager_controller"
import { JournalController } from "@klaus-liebler/web-components/typescript/screen_controller/journal_controller"
import { FingerprintScreenController } from "@klaus-liebler/web-components/typescript/screen_controller/fingerprint_controller"
import { CanMonitorScreenController } from "@klaus-liebler/web-components-sensact/canmonitor_controller";
import { SensactController } from "@klaus-liebler/web-components-sensact/sensact_controller";
import SensactAppsBuild from "@generated/sensact_appsbuilder/sensactapps"
import * as CFG from "@generated/runtimeconfig_ts"
import * as CONST from "@klaus-liebler/web-components/typescript/utils/constants";
import { NodemasterController } from "./nodemaster_controller";

let app: AppController;
document.addEventListener("DOMContentLoaded", (_) => {
  // Optional: Chatbot mit Google API Key aktivieren
  // Wenn Sie einen Google API Key haben und Google Generative AI nutzen möchten, aktivieren Sie das:
  let chatbot: IChatbot | null = null;
  // const googleApiKey = null; // Hier den API Key eintragen
  // if (googleApiKey) {
  //   import("@klaus-liebler/web-components/typescript/google_chatbot").then(({ Chatbot }) => {
  //     chatbot = new Chatbot(googleApiKey);
  //   });
  // }

  // Optional: Racing Game EasterEgg aktivieren
  let easterEggFn: ((canvas: HTMLCanvasElement) => void) | null = null;
  // const enableEasterEgg = true; // Set to enable
  // if (enableEasterEgg) {
  //   import("@klaus-liebler/web-components/typescript/screen_controller/racinggame_controller").then(({ runCarRace }) => {
  //     easterEggFn = runCarRace;
  //   });
  // }

  app = new AppController("SensAct", CONST.WS_URL, easterEggFn, `Modell ${CFG.SENSACT_MODEL_NAME} vom ${CFG.SENSACT_MODEL_CREATION_DT_STRING}`, chatbot,
    { hostLabel: CFG.SENSACT_NODE_NAME, look: "hmi" });

  // Reihenfolge = Reihenfolge im Arbeitsbereich-Umschalter; der erste ist die Startseite
  app.AddScreenController("dashboard", new RegExp("^/$"), html`<span>Übersicht</span>`, new OverviewScreenController(app, () => app.Workspaces),
    { icon: "dashboard", group: "Übersicht", description: "Alle Arbeitsbereiche" })
  const s = new SensactController(app);
  s.AddApps(SensactAppsBuild(s));
  app.AddScreenController("sensact", new RegExp("^/sensact$"), html`<span>Apps der Station</span>`, s,
    { title: "Apps der Station", icon: "house", group: "Haus", description: "Licht, Rollläden, Szenen dieser Station bedienen" })
  // @ts-ignore
  if(CFG.NODE_ID=="SNSCT_NODE_SIDEDOOR")
    app.AddScreenController("finger", new RegExp("^/finger$"), html`<span>Fingerabdruck</span>`, new FingerprintScreenController(app),
      { title: "Fingerabdruck", icon: "key", group: "Haus", description: "Fingerabdrücke anlernen und verwalten" })
  app.AddScreenController("nodemaster", new RegExp("^/nodemaster$"), html`<span>Ein-/Ausgänge</span>`, new NodemasterController(app),
    { title: "Ein-/Ausgänge", icon: "registers", group: "Diagnose", description: "Ein- und Ausgänge des Knotens direkt lesen und schalten" })
  app.AddScreenController("canmon", new RegExp("^/canmon$"), html`<span>CAN-Monitor</span>`, new CanMonitorScreenController(app),
    { title: "CAN-Monitor", icon: "can", group: "Diagnose", description: "Nachrichten auf dem CAN-Bus mitlesen" })
  app.AddScreenController("journal", new RegExp("^/journal$"), html`<span>Journal</span>`, new JournalController(app),
    { icon: "book", group: "Diagnose", description: "Ereignisse der Station" })
  app.AddTerminal()
  app.AddScreenController("system", new RegExp("^/system$"), html`<span>System</span>`, new SystemController(app),
    { icon: "chip", group: "Gerät", description: "Firmware, Stationspaket, Dateien, Neustart" })
  if(CFG.WIFI_ENABLED)
    app.AddScreenController("wifiman", new RegExp("^/wifiman$"), html`<span>WLAN</span>`, new WifimanagerController(app),
      { icon: "wifi", group: "Gerät", description: "WLAN-Netze suchen und verbinden" })

  app.Startup();
});


