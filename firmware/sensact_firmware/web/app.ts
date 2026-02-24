import { html } from "lit-html";
import { AppController } from "@klaus-liebler/web-components"
import type { IChatbot } from "@klaus-liebler/web-components"
import { DefaultScreenController } from "@klaus-liebler/web-components/typescript/screen_controller/screen_controller"
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

  app = new AppController("SensAct WebUI", CONST.WS_URL, easterEggFn, `:: Node ${CFG.SENSACT_NODE_NAME} :: SensactModel ${CFG.SENSACT_MODEL_NAME} created at  ${CFG.SENSACT_MODEL_CREATION_DT_STRING} `, chatbot);

  app.AddScreenController("dashboard", new RegExp("^/$"), html`<span>&#127760;</span><span>Home</span>`, new DefaultScreenController(app)) 
  app.AddScreenController("system", new RegExp("^/system$"), html`<span>🧰</span><span>System Settings</span>`, new SystemController(app))
  app.AddScreenController("wifiman", new RegExp("^/wifiman$"), html`<span>📶</span><span>Wifi Manager</span>`, new WifimanagerController(app))
  app.AddScreenController("canmon", new RegExp("^/canmon$"), html`<span>🔬</span><span>Can Bus Monitor</span>`, new CanMonitorScreenController(app))
  app.AddScreenController("nodemaster", new RegExp("^/nodemaster$"), html`<span>🧩</span><span>Node Master</span>`, new NodemasterController(app))
  const s = new SensactController(app);
  s.AddApps(SensactAppsBuild(s));
  app.AddScreenController("sensact", new RegExp("^/sensact$"), html`<span>✽</span><span>SensAct</span>`, s)
  app.AddScreenController("journal", new RegExp("^/journal$"), html`<span>📓</span><span>Journal</span>`, new JournalController(app))
  // @ts-ignore
  if(CFG.NODE_ID=="SNSCT_NODE_SIDEDOOR")
    app.AddScreenController("finger", new RegExp("^/finger$"), html`<span>🔑</span><span>Fingerprint</span>`, new FingerprintScreenController(app))
 
  app.Startup();
});


