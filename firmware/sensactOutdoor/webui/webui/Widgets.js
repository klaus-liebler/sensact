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
exports.OnOffWidget = exports.SinglePwmWidget = exports.BlindsWidget = exports.Widget = exports.UPDATE_EACH_INTERVAL = void 0;
const flatbuffers_1 = require("flatbuffers"); // located in node_modules
const C = __importStar(require("../generated/webui_core_comm_generated"));
const Utils_1 = require("./Utils");
exports.UPDATE_EACH_INTERVAL = 2;
const KELVIN_MAX = 6500;
const KELVIN_MIN = 2700;
class Widget {
    constructor(manager, id) {
        this.manager = manager;
        this.id = id;
    }
}
exports.Widget = Widget;
class BlindsWidget extends Widget {
    constructor(m, id) {
        super(m, id);
    }
    processIoState(r, i) {
        var _a;
        if (((_a = r.states(i)) === null || _a === void 0 ? void 0 : _a.stateType()) != C.sensact.comm.uState.tBlindState) {
            return;
        }
        let state = r.states(i).state(new C.sensact.comm.tBlindState()).state();
        //let position = r.states(i)!.state(new C.sensact.comm.tBlindState())!.position();
        switch (state) {
            case C.sensact.comm.eBlindState.DOWN:
                this.butBlindsDown.style.color = "green";
                this.butBlindsUp.style.color = "black";
                break;
            case C.sensact.comm.eBlindState.UP:
                this.butBlindsDown.style.color = "black";
                this.butBlindsUp.style.color = "green";
                break;
            default:
                this.butBlindsDown.style.color = "black";
                this.butBlindsUp.style.color = "black";
                break;
        }
    }
    blindEventBuilder(e) {
        let builder = new flatbuffers_1.flatbuffers.Builder(1024);
        let evt = C.sensact.comm.tBlindCommand.createtBlindCommand(builder, e);
        let ioCmd = C.sensact.comm.tCommand.createtCommand(builder, C.sensact.comm.uCommand.tBlindCommand, this.id, evt);
        builder.finish(ioCmd);
        return builder.asUint8Array();
    }
    renderWidgetAndHookHandlers(container) {
        this.butBlindsUp = Utils_1.$.Html(container, "button", ["updown"], "🢁"); //<button id="blindsUp" class="updown" style="flex: 1 1 0;">&#129153;</button>
        this.butBlindsUp.style.flex = "1 1 0";
        this.butBlindsStop = Utils_1.$.Html(container, "button"); //<button id="blindsStop" style="flex: 1 1 0;"><svg class="icon"><use style="fill:black;" xlink:href="#icon-blinds"></use></svg></button>
        this.butBlindsStop.style.flex = "1 1 0";
        Utils_1.$.SvgIcon(this.butBlindsStop, "blinds").style.fill = "black";
        this.butBlindsDown = Utils_1.$.Html(container, "button", ["updown"], "🢃"); //<button id="blindsDown" class="updown" style="flex: 1 1 0;">&#129155;</button>
        this.butBlindsDown.style.flex = "1 1 0";
        this.butBlindsUp.onclick = (e) => {
            this.manager.http_put_iocmd(this.blindEventBuilder(C.sensact.comm.eBlindCommand.UP));
        };
        this.butBlindsStop.onclick = (e) => {
            this.manager.http_put_iocmd(this.blindEventBuilder(C.sensact.comm.eBlindCommand.STOP));
        };
        this.butBlindsDown.onclick = (e) => {
            this.manager.http_put_iocmd(this.blindEventBuilder(C.sensact.comm.eBlindCommand.DOWN));
        };
    }
}
exports.BlindsWidget = BlindsWidget;
class SinglePwmWidget extends Widget {
    constructor(manager, id) {
        super(manager, id);
        this.firstCallOfProcessIoCtrl = true;
    }
    processIoState(r, i) {
        var _a;
        if (((_a = r.states(i)) === null || _a === void 0 ? void 0 : _a.stateType()) != C.sensact.comm.uState.tSinglePwmState) {
            return;
        }
        let on = r.states(i).state(new C.sensact.comm.tSinglePwmState()).on();
        let intensity_0_100 = r.states(i).state(new C.sensact.comm.tSinglePwmState()).intensity0100();
        this.butSpotsOnOff.style.backgroundColor = on ? "green" : "grey";
        if (this.firstCallOfProcessIoCtrl) {
            this.spotsPicker.color.set(intensity_0_100);
            this.firstCallOfProcessIoCtrl = false;
        }
    }
    renderWidgetAndHookHandlers(container) {
        let pickerContainer = Utils_1.$.Html(container, "div", []);
        pickerContainer.style.flex = "1 1 0";
        pickerContainer.style.alignSelf = "center";
        this.butSpotsOnOff = Utils_1.$.Html(container, "button", ["onoff"]); //<button id="blindsUp" class="updown" style="flex: 1 1 0;">&#129153;</button>
        this.butSpotsOnOff.style.flex = "0 0 0";
        Utils_1.$.SvgIcon(this.butSpotsOnOff, "spot").style.fill = "black";
        let width = Math.min(400, 0.9 * pickerContainer.offsetWidth);
        this.spotsPicker = new iro.ColorPicker(pickerContainer, {
            width: width,
            layoutDirection: 'vertical',
            color: "#000",
            layout: [
                {
                    component: iro.ui.Slider,
                    options: {
                        sliderType: 'value',
                    }
                },
            ]
        });
        this.spotsPicker.on('input:change', (color, changes) => {
            let intensity0_100 = color.value;
            console.info(intensity0_100);
            let builder = new flatbuffers_1.flatbuffers.Builder(1024);
            let evt = C.sensact.comm.tSinglePwmCommand.createtSinglePwmCommand(builder, C.sensact.comm.eSinglePwmCommand.CHANGE_INTENSITY, intensity0_100);
            let ioCmd = C.sensact.comm.tCommand.createtCommand(builder, C.sensact.comm.uCommand.tSinglePwmCommand, this.id, evt);
            builder.finish(ioCmd);
            this.manager.http_put_iocmd(builder.asUint8Array());
        });
        this.butSpotsOnOff.onclick = (e) => {
            let builder = new flatbuffers_1.flatbuffers.Builder(1024);
            C.sensact.comm.tSinglePwmCommand.starttSinglePwmCommand(builder);
            C.sensact.comm.tSinglePwmCommand.addCmd(builder, C.sensact.comm.eSinglePwmCommand.TOGGLE);
            let evt = C.sensact.comm.tSinglePwmCommand.endtSinglePwmCommand(builder);
            let ioCmd = C.sensact.comm.tCommand.createtCommand(builder, C.sensact.comm.uCommand.tSinglePwmCommand, this.id, evt);
            builder.finish(ioCmd);
            this.manager.http_put_iocmd(builder.asUint8Array());
        };
    }
}
exports.SinglePwmWidget = SinglePwmWidget;
class OnOffWidget extends Widget {
    constructor(manager, id) {
        super(manager, id);
        this.state = C.sensact.comm.eOnOffState.AUTO_OFF;
    }
    updateUI(newState) {
        this.state = newState;
        switch (this.state) {
            case C.sensact.comm.eOnOffState.AUTO_OFF:
                this.selector.style.left = this.butOn.clientWidth + "px";
                this.selector.style.width = this.butAuto.clientWidth + "px";
                this.selector.innerHTML = "AUTO";
                this.selector.style.backgroundColor = "black";
                this.selector.style.color = "white";
                this.selector.style.visibility = "visible";
                break;
            case C.sensact.comm.eOnOffState.AUTO_ON:
                this.selector.style.left = this.butOn.clientWidth + "px";
                this.selector.style.width = this.butAuto.clientWidth + "px";
                this.selector.innerHTML = "AUTO";
                this.selector.style.backgroundColor = "green";
                this.selector.style.color = "black";
                this.selector.style.visibility = "visible";
                break;
            case C.sensact.comm.eOnOffState.MANUAL_ON:
                this.selector.style.left = "0px";
                this.selector.style.width = this.butOn.clientWidth + "px";
                this.selector.innerHTML = "ON";
                this.selector.style.backgroundColor = "green";
                this.selector.style.color = "black";
                this.selector.style.visibility = "visible";
                break;
            case C.sensact.comm.eOnOffState.MANUAL_OFF:
                this.selector.style.left = this.butOn.clientWidth + this.butAuto.clientWidth + 1 + "px";
                this.selector.style.width = this.butOff.clientWidth + "px";
                this.selector.innerHTML = "OFF";
                this.selector.style.backgroundColor = "black";
                this.selector.style.color = "white";
                this.selector.style.visibility = "visible";
                break;
        }
    }
    processIoState(r, i) {
        var _a;
        if (((_a = r.states(i)) === null || _a === void 0 ? void 0 : _a.stateType()) != C.sensact.comm.uState.tOnOffState) {
            return;
        }
        let state = r.states(i).state(new C.sensact.comm.tOnOffState()).state();
        this.updateUI(state);
    }
    renderWidgetAndHookHandlers(container) {
        let subcontainer = Utils_1.$.Html(container, "div"); //one single element
        subcontainer.style.flex = "1 1 0";
        subcontainer.style.position = "relative";
        let itemscontainer = Utils_1.$.Html(subcontainer, "div", ["switch_3_ways_container"]); //one single element//noiw, a container for the items; with flex layout
        itemscontainer.style.display = "flex";
        //itemscontainer.style.flexFlow="row nowrap";//default!
        //itemscontainer.style.justifyContent="flex-start";//default
        //itemscontainer.style.alignItems="stretch";//default
        this.butOn = Utils_1.$.Html(itemscontainer, "div", ["switch_3_ways_item"]);
        Utils_1.$.Html(this.butOn, "span", [], "On");
        this.butAuto = Utils_1.$.Html(itemscontainer, "div", ["switch_3_ways_item"]);
        Utils_1.$.Html(this.butAuto, "span", [], "Auto");
        this.butOff = Utils_1.$.Html(itemscontainer, "div", ["switch_3_ways_item"], "Off");
        Utils_1.$.Html(this.butOff, "span", [], "Off");
        this.selector = Utils_1.$.Html(subcontainer, "div", ["switch_3_ways_selector"]);
        this.selector.style.visibility = "hidden";
        Utils_1.$.Html(this.selector, "span", [], "");
        this.butOn.onclick = (e) => {
            let builder = new flatbuffers_1.flatbuffers.Builder(1024);
            let evt = C.sensact.comm.tOnOffCommand.createtOnOffCommand(builder, C.sensact.comm.eOnOffCommand.ON, 0);
            let ioCmd = C.sensact.comm.tCommand.createtCommand(builder, C.sensact.comm.uCommand.tOnOffCommand, this.id, evt);
            builder.finish(ioCmd);
            let buf = builder.asUint8Array();
            this.manager.http_put_iocmd(buf);
        };
        this.butAuto.onclick = (e) => {
            let builder = new flatbuffers_1.flatbuffers.Builder(1024);
            let evt = C.sensact.comm.tOnOffCommand.createtOnOffCommand(builder, C.sensact.comm.eOnOffCommand.AUTO, 0);
            let ioCtrl = C.sensact.comm.tCommand.createtCommand(builder, C.sensact.comm.uCommand.tOnOffCommand, this.id, evt);
            builder.finish(ioCtrl);
            let buf = builder.asUint8Array();
            this.manager.http_put_iocmd(buf);
        };
        this.butOff.onclick = (e) => {
            let builder = new flatbuffers_1.flatbuffers.Builder(1024);
            let evt = C.sensact.comm.tOnOffCommand.createtOnOffCommand(builder, C.sensact.comm.eOnOffCommand.OFF, 0);
            let ioCmd = C.sensact.comm.tCommand.createtCommand(builder, C.sensact.comm.uCommand.tOnOffCommand, this.id, evt);
            builder.finish(ioCmd);
            let buf = builder.asUint8Array();
            this.manager.http_put_iocmd(buf);
        };
    }
}
exports.OnOffWidget = OnOffWidget;
//# sourceMappingURL=Widgets.js.map