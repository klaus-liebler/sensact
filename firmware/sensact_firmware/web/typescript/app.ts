import { html } from "lit-html";
import "../style/app.css";
import {AppController, DefaultScreenController, SystemController, WifimanagerController} from "@klaus-liebler/web-components"
import { WS_URL, WS_URL_ESP32_AP, WS_URL_ESP32_STA } from "@klaus-liebler/web-components/typescript/utils/constants";
import { CanMonitorScreenController } from "@klaus-liebler/sensact/canmonitor_controller";
import { SensactController } from "@klaus-liebler/sensact/sensact_controller";
import * as SensactAppsBuild from "@generated/sensact_ts/sensactapps"



let app: AppController;
document.addEventListener("DOMContentLoaded", (e) => {
  app = new AppController("Sensact WebUI", WS_URL, false, false, "");

  app.AddScreenController("dashboard", new RegExp("^/$"), html`<span>&#127760;</span><span>Home</span>`, new DefaultScreenController(app)) 
  app.AddScreenController("system", new RegExp("^/system$"), html`<span>🧰</span><span>System Settings</span>`, new SystemController(app))
  app.AddScreenController("wifiman", new RegExp("^/wifiman$"), html`<span>📶</span><span>Wifi Manager</span>`, new WifimanagerController(app))
  app.AddScreenController("canmon", new RegExp("^/canmon$"), html`<span>🔬</span><span>Can Bus Monitor</span>`, new CanMonitorScreenController(app))
  const s = new SensactController(app);
  s.AddApps(SensactAppsBuild.default(s));
  app.AddScreenController("sensact", new RegExp("^/sensact$"), html`<span>🔬</span><span>Sensact</span>`, s)
 
  app.Startup();
});


