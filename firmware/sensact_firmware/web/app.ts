import { html } from "lit-html";
import { AppController, DefaultScreenController, SystemController, WifimanagerController, JournalController, FingerprintScreenController } from "@klaus-liebler/web-components"
import { CanMonitorScreenController } from "@klaus-liebler/web-components-sensact/canmonitor_controller";
import { SensactController } from "@klaus-liebler/web-components-sensact/sensact_controller";
import SensactAppsBuild from "@generated/sensact_appsbuilder/sensactapps"
import * as CFG from "@generated/runtimeconfig_ts"
import * as CONST from "@klaus-liebler/web-components/typescript/utils/constants";

let app: AppController;
document.addEventListener("DOMContentLoaded", (e) => {
  app = new AppController("SensAct WebUI", CONST.WS_URL, false, false, `:: Node ${CFG.SENSACT_NODE_NAME} :: SensactModel ${CFG.SENSACT_MODEL_NAME} created at  ${CFG.SENSACT_MODEL_CREATION_DT_STRING} `);

  app.AddScreenController("dashboard", new RegExp("^/$"), html`<span>&#127760;</span><span>Home</span>`, new DefaultScreenController(app)) 
  app.AddScreenController("system", new RegExp("^/system$"), html`<span>🧰</span><span>System Settings</span>`, new SystemController(app))
  app.AddScreenController("wifiman", new RegExp("^/wifiman$"), html`<span>📶</span><span>Wifi Manager</span>`, new WifimanagerController(app))
  app.AddScreenController("canmon", new RegExp("^/canmon$"), html`<span>🔬</span><span>Can Bus Monitor</span>`, new CanMonitorScreenController(app))
  const s = new SensactController(app);
  s.AddApps(SensactAppsBuild(s));
  app.AddScreenController("sensact", new RegExp("^/sensact$"), html`<span>✽</span><span>SensAct</span>`, s)
  app.AddScreenController("journal", new RegExp("^/journal$"), html`<span>📓</span><span>Journal</span>`, new JournalController(app))
  if(CFG.NODE_ID=="SNSCT_NODE_SIDEDOOR")
    app.AddScreenController("finger", new RegExp("^/finger$"), html`<span>🔑</span><span>Fingerprint</span>`, new FingerprintScreenController(app))
 
  app.Startup();
});


