"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.ScreenController = exports.ControllerState = void 0;
var ControllerState;
(function (ControllerState) {
    ControllerState[ControllerState["CREATED"] = 0] = "CREATED";
    ControllerState[ControllerState["STARTED"] = 1] = "STARTED";
    ControllerState[ControllerState["STOPPED"] = 2] = "STOPPED";
})(ControllerState = exports.ControllerState || (exports.ControllerState = {}));
class ScreenController {
    constructor(appManagement, container) {
        this.appManagement = appManagement;
        this.container = container;
        this.hideDIV();
        this.state = ControllerState.CREATED;
    }
    get ElementId() { return this.container.id; }
    get State() { return this.state; }
    set State(value) { this.state = value; }
    showDIV() {
        this.container.style.display = "flex";
    }
    hideDIV() {
        this.container.style.display = "none";
    }
}
exports.ScreenController = ScreenController;
//# sourceMappingURL=ScreenController.js.map