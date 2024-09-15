"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const ControlScreenController_1 = require("./ControlScreenController");
const ConfigScreenController_1 = require("./ConfigScreenController");
const ScreenController_1 = require("./ScreenController");
const Utils_1 = require("./Utils");
class AppController {
    constructor() {
        this.screenControllers = [];
        this.activeControllerIndex = 0;
    }
    setActiveScreen(newIndex) {
        if (newIndex == this.activeControllerIndex)
            return;
        this.screenControllers.forEach((controller, i) => {
            if (i == newIndex) {
                controller.showDIV();
                if (controller.State == ScreenController_1.ControllerState.CREATED) {
                    controller.onFirstStart();
                    controller.State = ScreenController_1.ControllerState.STARTED;
                }
                else {
                    controller.onRestart();
                    controller.State = ScreenController_1.ControllerState.STARTED;
                }
            }
            else {
                controller.hideDIV();
                if (controller.State == ScreenController_1.ControllerState.STARTED) {
                    controller.onStop();
                    controller.State = ScreenController_1.ControllerState.STOPPED;
                }
            }
        });
        this.activeControllerIndex = newIndex;
    }
    startup() {
        let header = document.querySelector("header");
        Utils_1.$.Html(header, "button", ["small"], "CMD", ["type", "button"]).onclick = (e) => this.setActiveScreen(0);
        Utils_1.$.Html(header, "button", ["small"], "CFG", ["type", "button"]).onclick = (e) => this.setActiveScreen(1);
        this.screenControllers.push(new ControlScreenController_1.ControlScreenController(this, Utils_1.$.HtmlAfter(header, "main", ["ControlScreen"])));
        this.screenControllers.push(new ConfigScreenController_1.ConfigScreenController(this, Utils_1.$.HtmlAfter(header, "main", ["ConfigScreen"])));
        this.setActiveScreen(0);
    }
}
let app;
document.addEventListener("DOMContentLoaded", (e) => {
    app = new AppController();
    app.startup();
});
//# sourceMappingURL=Main.js.map