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
exports.SinglePwmConfigWidget = exports.BlindsConfigWidget = exports.OnOffConfigWidget = exports.ConfigWidget = void 0;
const C = __importStar(require("../generated/webui_core_comm_generated"));
const Utils_1 = require("./Utils");
var M = C.sensact.comm;
class ConfigWidget {
    constructor(manager) {
        this.manager = manager;
        this.index = 0;
    }
    renderWidget(container, isFirst, isLast) {
        let wCont = Utils_1.$.Html(container, "div");
        this.renderWidgetCoreForm(wCont);
        if (!isFirst)
            Utils_1.$.Html(wCont, "button", ["styled"], "▲", ["type", "button"]).onclick = (e) => this.UpPressed();
        if (!isLast)
            Utils_1.$.Html(wCont, "button", ["styled"], "▼", ["type", "button"]).onclick = (e) => this.DownPressed();
        ;
        let btnDelete = Utils_1.$.Html(wCont, "button", ["styled"], "", ["type", "button"]);
        btnDelete.onclick = (e) => this.DeletePressed();
        Utils_1.$.SvgIcon(btnDelete, "bin2", ["smallicon"]);
    }
    SetIndex(i) {
        this.index = i;
    }
    UpPressed() {
        this.manager.moveUp(this.index);
    }
    DownPressed() {
        this.manager.moveDown(this.index);
    }
    DeletePressed() {
        this.manager.delete(this.index);
    }
}
exports.ConfigWidget = ConfigWidget;
class OnOffConfigWidget extends ConfigWidget {
    constructor() {
        super(...arguments);
        this.relay = 0;
        this.initialState = M.eOnOffState.AUTO_OFF;
        this.autoOffMs = 60000;
    }
    renderWidgetCoreForm(container) {
        Utils_1.$.Html(container, "h3", [], "OnOff");
        this.inpRelay = Utils_1.$.LabelSpanInput(container, "Relay", this.relay, 0, 65535);
        this.inpInitialState = Utils_1.$.LabelSpanSelectFillEnum(container, "Initial State", M.eOnOffState, this.initialState);
        this.inpAutoOffMs = Utils_1.$.LabelSpanInput(container, "AutoOff [ms]", this.autoOffMs, 0, 100000);
    }
    SetData(config) {
        if (config.configType() != M.uConfig.tOnOffConfig)
            return;
        let x = config.config(new M.tOnOffConfig());
        if (!(x !== null && x !== undefined))
            return;
        this.relay = x.relay();
        this.initialState = x.initialState();
        this.autoOffMs = x.autoOffMsecs();
    }
    CopyDataFromHtmlToModel() {
        this.relay = this.inpRelay.valueAsNumber;
        this.initialState = Number.parseInt(this.inpInitialState.value);
        this.autoOffMs = this.inpAutoOffMs.valueAsNumber;
    }
    fillFlatbuffer(builder, vect) {
        let x = M.tOnOffConfig.createtOnOffConfig(builder, this.relay, this.initialState, this.autoOffMs);
        vect.push(x);
        return;
    }
}
exports.OnOffConfigWidget = OnOffConfigWidget;
class BlindsConfigWidget extends ConfigWidget {
    constructor() {
        super(...arguments);
        this.relay1 = 0;
        this.relay2 = 0;
        this.mode = M.eRelayInterlockMode.RELAY1_DOWN__RELAY2_UP;
        this.up_msec = 60000;
        this.down_msec = 60000;
    }
    renderWidgetCoreForm(wCont) {
        Utils_1.$.Html(wCont, "h3", [], "Blind");
        this.inpRelay1 = Utils_1.$.LabelSpanInput(wCont, "Relay1", this.relay1, 0, 65535);
        this.inpRelay2 = Utils_1.$.LabelSpanInput(wCont, "Relay2", this.relay2, 0, 65535);
        this.inpMode = Utils_1.$.LabelSpanSelectFillEnum(wCont, "Mode", M.eRelayInterlockMode, this.mode);
        this.inpUp = Utils_1.$.LabelSpanInput(wCont, "Up [ms]", this.up_msec, 1, 100000);
        this.inpDown = Utils_1.$.LabelSpanInput(wCont, "Down [ms]", this.down_msec, 1, 100000);
    }
    SetData(config) {
        if (config.configType() != M.uConfig.tBlindConfig)
            return;
        let x = config.config(new M.tBlindConfig());
        if (!(x !== null && x !== undefined))
            return;
        this.relay1 = x.relay1();
        this.relay2 = x.relay2();
        this.mode = x.mode();
        this.up_msec = x.timeUpMsecs();
        this.down_msec = x.timeDownMsecs();
    }
    CopyDataFromHtmlToModel() {
        this.relay1 = this.inpRelay1.valueAsNumber;
        this.relay2 = this.inpRelay2.valueAsNumber;
        this.mode = Number.parseInt(this.inpMode.value);
        this.up_msec = this.inpUp.valueAsNumber;
        this.down_msec = this.inpDown.valueAsNumber;
    }
    fillFlatbuffer(builder, vect) {
        let x = M.tBlindConfig.createtBlindConfig(builder, this.relay1, this.relay2, this.mode, this.up_msec, this.down_msec);
        vect.push(x);
        return;
    }
}
exports.BlindsConfigWidget = BlindsConfigWidget;
class SinglePwmConfigWidget extends ConfigWidget {
    constructor() {
        super(...arguments);
        this.pwmFirst = 0;
        this.pwmLast = 0;
        this.standbyControllerId = 0;
        this.autoOffMs = 60000;
    }
    renderWidgetCoreForm(wCont) {
        Utils_1.$.Html(wCont, "h3", [], "SinglePwm");
        this.inpPwmFirst = Utils_1.$.LabelSpanInput(wCont, "pwm_first", this.pwmFirst, 0, 65535);
        this.inpPwmLast = Utils_1.$.LabelSpanInput(wCont, "pwm_last", this.pwmLast, 0, 65535);
        this.inpStandbyControllerId = Utils_1.$.LabelSpanInput(wCont, "stndby", this.standbyControllerId, 0, 65535);
        this.inpAutoOffMs = Utils_1.$.LabelSpanInput(wCont, "AutoOff [ms]", this.autoOffMs, 0, 100000);
    }
    SetData(config) {
        if (config.configType() != M.uConfig.tSinglePwmConfig)
            return;
        let x = config.config(new M.tSinglePwmConfig());
        if (!(x !== null && x !== undefined))
            return;
        this.pwmFirst = x.pwmFirst();
        this.pwmLast = x.pwmLast();
        this.standbyControllerId = x.idOfStandbyController();
        this.autoOffMs = x.autoOffMsecs();
    }
    CopyDataFromHtmlToModel() {
        this.pwmFirst = this.inpPwmFirst.valueAsNumber;
        this.pwmLast = this.inpPwmLast.valueAsNumber;
        this.standbyControllerId = this.inpStandbyControllerId.valueAsNumber;
        this.autoOffMs = this.inpAutoOffMs.valueAsNumber;
    }
    fillFlatbuffer(builder, vect) {
        let x = M.tSinglePwmConfig.createtSinglePwmConfig(builder, this.pwmFirst, this.pwmLast, this.standbyControllerId, this.autoOffMs);
        vect.push(x);
        return;
    }
}
exports.SinglePwmConfigWidget = SinglePwmConfigWidget;
//# sourceMappingURL=ConfigWidgets.js.map