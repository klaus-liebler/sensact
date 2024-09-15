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
exports.ConfigScreenController = exports.UPDATE_EACH_INTERVAL = exports.DE_de = void 0;
const ScreenController_1 = require("./ScreenController");
const Utils_1 = require("./Utils");
const flatbuffers_1 = require("flatbuffers"); // located in node_modules
const C = __importStar(require("../generated/webui_core_comm_generated"));
const ConfigWidgets_1 = require("./ConfigWidgets");
const DummyData_1 = require("./DummyData");
var M = C.sensact.comm;
exports.DE_de = new Intl.NumberFormat('de-DE');
exports.UPDATE_EACH_INTERVAL = 2;
var ConfigWidgetTypes;
(function (ConfigWidgetTypes) {
    ConfigWidgetTypes[ConfigWidgetTypes["Blind"] = 0] = "Blind";
    ConfigWidgetTypes[ConfigWidgetTypes["OnOff"] = 1] = "OnOff";
    ConfigWidgetTypes[ConfigWidgetTypes["SinglePwm"] = 2] = "SinglePwm";
})(ConfigWidgetTypes || (ConfigWidgetTypes = {}));
class ConfigScreenController extends ScreenController_1.ScreenController {
    constructor(appManagement, container) {
        super(appManagement, container);
        this.widgets = new Array();
    }
    addWidgets(config) {
        var _a;
        for (let i = 0; i < config.configsLength(); i++) {
            let w = null;
            switch ((_a = config.configs(i)) === null || _a === void 0 ? void 0 : _a.configType()) {
                case C.sensact.comm.uConfig.tBlindConfig:
                    w = new ConfigWidgets_1.BlindsConfigWidget(this);
                    break;
                case C.sensact.comm.uConfig.tOnOffConfig:
                    w = new ConfigWidgets_1.OnOffConfigWidget(this);
                    break;
                case C.sensact.comm.uConfig.tSinglePwmConfig:
                    w = new ConfigWidgets_1.SinglePwmConfigWidget(this);
                    break;
            }
            if (!w)
                throw new Error("if(!w)");
            this.widgets.push(w);
            w.SetIndex(i);
        }
        this.redraw();
    }
    onCreate() { }
    onRestart() {
    }
    onStop() {
    }
    onFirstStart() {
        let top = Utils_1.$.Html(this.container, "div", ["top"]);
        this.typeSelector = Utils_1.$.LabelSpanSelectFillEnum(top, "Select a Widget", ConfigWidgetTypes, 0);
        Utils_1.$.Html(top, "button", ["styled"], "Add", ["type", "button"]).onclick = (e) => this.AddPressed();
        Utils_1.$.Html(top, "button", ["styled"], "Save", ["type", "button"]).onclick = (e) => this.SavePressed();
        ;
        this.divItemlist = Utils_1.$.Html(this.container, "div", ["itemlist"]);
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
    createWidgets(config) {
        var _a;
        for (let i = 0; i < config.configsLength(); i++) {
            let w = null;
            switch ((_a = config.configs(i)) === null || _a === void 0 ? void 0 : _a.configType()) {
                case C.sensact.comm.uConfig.tBlindConfig:
                    w = new ConfigWidgets_1.BlindsConfigWidget(this);
                    break;
                case C.sensact.comm.uConfig.tOnOffConfig:
                    w = new ConfigWidgets_1.OnOffConfigWidget(this);
                    break;
                case C.sensact.comm.uConfig.tSinglePwmConfig:
                    w = new ConfigWidgets_1.SinglePwmConfigWidget(this);
                    break;
            }
            if (!w)
                throw new Error("!w");
            w.SetData(config.configs(i));
            w.SetIndex(i);
            w.renderWidget(this.divItemlist, i == 0, i + 1 == config.configsLength());
            this.widgets.push(w);
        }
        this.redraw();
    }
    SavePressed() {
        let builder = new flatbuffers_1.flatbuffers.Builder(1024);
        let configs_vect = [];
        for (let i = 0; i < this.widgets.length; i++) {
            this.widgets[i].fillFlatbuffer(builder, configs_vect);
        }
        let configs = M.tIoConfig.createConfigsVector(builder, configs_vect);
        let ioConfig = M.tIoConfig.createtIoConfig(builder, 0, configs);
        builder.finish(ioConfig);
        let xhr = new XMLHttpRequest;
        xhr.open("PUT", "/iocmd", true);
        xhr.send(builder.asUint8Array());
    }
    AddPressed() {
        let w = null;
        switch (Number.parseInt(this.typeSelector.value)) {
            case ConfigWidgetTypes.Blind:
                w = new ConfigWidgets_1.BlindsConfigWidget(this);
                break;
            case ConfigWidgetTypes.OnOff:
                w = new ConfigWidgets_1.OnOffConfigWidget(this);
                break;
            case ConfigWidgetTypes.SinglePwm:
                w = new ConfigWidgets_1.SinglePwmConfigWidget(this);
                break;
        }
        if (!w)
            throw new Error("if(!w)");
        w.renderWidget(this.divItemlist, false, false); //muss hier gerendert werden, damit die Elemente exisitieren, die beim redraw->CopyDataFromHtmlToModel() ausgewertet werden
        this.widgets.push(w);
        this.redraw();
    }
    redraw() {
        for (let i = 0; i < this.widgets.length; i++) {
            this.widgets[i].CopyDataFromHtmlToModel();
        }
        this.divItemlist.innerText = "";
        for (let i = 0; i < this.widgets.length; i++) {
            this.widgets[i].SetIndex(i);
            this.widgets[i].renderWidget(this.divItemlist, i == 0, i + 1 == this.widgets.length);
        }
    }
    moveUp(i) {
        let w = this.widgets[i - 1];
        this.widgets[i - 1] = this.widgets[i];
        this.widgets[i] = w;
        this.redraw();
    }
    moveDown(i) {
        let w = this.widgets[i];
        this.widgets[i] = this.widgets[i + 1];
        this.widgets[i + 1] = w;
        this.redraw();
    }
    delete(i) {
        this.widgets.splice(i, 1);
        this.redraw();
    }
}
exports.ConfigScreenController = ConfigScreenController;
//# sourceMappingURL=ConfigScreenController.js.map