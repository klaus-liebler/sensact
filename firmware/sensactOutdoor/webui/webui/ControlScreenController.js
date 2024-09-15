"use strict";
var __createBinding = (this && this.__createBinding) || (Object.create ? (function(o, m, k, k2) {
    if (k2 === undefined) k2 = k;
    Object.defineProperty(o, k2, { enumerable: true, get: function() { return m[k]; } });
}) : (function(o, m, k, k2) {
    if (k2 === undefined) k2 = k;
    o[k2] = m[k];
}));
var __setModuleDefault = (this && this.__setModuleDefault) || (Object.create ? (function(o, v) {
    Object.defineProperty(o, "default", { enumerable: true, value: v });
}) : function(o, v) {
    o["default"] = v;
});
var __importStar = (this && this.__importStar) || function (mod) {
    if (mod && mod.__esModule) return mod;
    var result = {};
    if (mod != null) for (var k in mod) if (k !== "default" && Object.prototype.hasOwnProperty.call(mod, k)) __createBinding(result, mod, k);
    __setModuleDefault(result, mod);
    return result;
};
Object.defineProperty(exports, "__esModule", { value: true });
exports.ControlScreenController = exports.UPDATE_EACH_INTERVAL = exports.DE_de = void 0;
const ScreenController_1 = require("./ScreenController");
const Widgets_1 = require("./Widgets");
const Utils_1 = require("./Utils");
const flatbuffers_1 = require("flatbuffers"); // located in node_modules
const C = __importStar(require("../generated/webui_core_comm_generated"));
const DummyData_1 = require("./DummyData");
exports.DE_de = new Intl.NumberFormat('de-DE');
exports.UPDATE_EACH_INTERVAL = 2;
class ControlScreenController extends ScreenController_1.ScreenController {
    constructor(appManagement, container) {
        super(appManagement, container);
        this.timeOfLastprocessIoCtrl = 0;
        this.widgets = new Map();
    }
    createWidgets(config) {
        var _a;
        let id = 1;
        for (let i = 0; i < config.configsLength(); i++) {
            let w;
            let widgetContainer = Utils_1.$.Html(this.container, "div", []);
            switch ((_a = config.configs(i)) === null || _a === void 0 ? void 0 : _a.configType()) {
                case C.sensact.comm.uConfig.tBlindConfig:
                    w = new Widgets_1.BlindsWidget(this, id);
                    break;
                case C.sensact.comm.uConfig.tOnOffConfig:
                    w = new Widgets_1.OnOffWidget(this, id);
                    break;
                case C.sensact.comm.uConfig.tSinglePwmConfig:
                    w = new Widgets_1.SinglePwmWidget(this, id);
                    break;
            }
            if (w !== null && w !== undefined) {
                this.widgets.set(id, w);
                w.renderWidgetAndHookHandlers(widgetContainer);
            }
            id++;
        }
    }
    http_put_iocmd(buf) {
        this.timeOfLastprocessIoCtrl = Date.now() + 60000; //um http_get_ioctrl zunächst mal zu vermeiden
        let xhr = new XMLHttpRequest;
        xhr.onerror = (e) => { console.log("Fehler beim XMLHttpRequest!"); };
        xhr.open("PUT", "/iocmd", true);
        xhr.responseType = "arraybuffer";
        xhr.onload = (e) => this.processIoState(xhr);
        xhr.send(buf);
    }
    http_get_iocmd() {
        if (Date.now() - this.timeOfLastprocessIoCtrl < 800) {
            return;
        }
        let xhr = new XMLHttpRequest;
        xhr.onerror = (e) => { console.log("Fehler beim XMLHttpRequest!"); };
        xhr.open("GET", "/iocmd", true);
        xhr.responseType = "arraybuffer";
        xhr.onload = (e) => this.processIoState(xhr);
        xhr.send();
    }
    processIoState(xhr) {
        this.timeOfLastprocessIoCtrl = Date.now();
        if (xhr.responseType != "arraybuffer") {
            console.error("No arraybuffer returned!");
            return;
        }
        let buf = new flatbuffers_1.flatbuffers.ByteBuffer(new Uint8Array(xhr.response));
        let r = C.sensact.comm.tState.getRootAstState(buf);
        for (let i = 0; i < r.statesLength(); i++) {
            let appId = r.states(i).applicationId();
            let widget = this.widgets.get(appId);
            widget === null || widget === void 0 ? void 0 : widget.processIoState(r, i);
        }
    }
    onCreate() { }
    onRestart() {
        this.timer = window.setInterval(() => { this.http_get_iocmd(); }, 1000);
    }
    onStop() {
        window.clearInterval(this.timer);
    }
    onFirstStart() {
        this.timer = window.setInterval(() => { this.http_get_iocmd(); }, 1000);
        let xhr = new XMLHttpRequest;
        xhr.responseType = "arraybuffer";
        xhr.onerror = (e) => {
            this.createWidgets(DummyData_1.DummyGenerator.CreateIoConfig());
        };
        xhr.onload = (e) => {
            if (xhr.status < 400) {
                let arrayBuffer = xhr.response;
                let buf = new flatbuffers_1.flatbuffers.ByteBuffer(new Uint8Array(arrayBuffer));
                let cfg = C.sensact.comm.tIoConfig.getRootAstIoConfig(buf);
                this.createWidgets(cfg);
            }
            else
                this.createWidgets(DummyData_1.DummyGenerator.CreateIoConfig());
        };
        xhr.open("GET", "/iocfg", true);
        xhr.send( /*body is ignored with get*/);
    }
}
exports.ControlScreenController = ControlScreenController;
//# sourceMappingURL=ControlScreenController.js.map