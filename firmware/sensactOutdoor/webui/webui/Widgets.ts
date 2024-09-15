import {WidgetContainer as WidgetManager } from "./Interfaces";
import { flatbuffers } from 'flatbuffers'; // located in node_modules
import * as C from '../generated/webui_core_comm_generated';
import { $ } from "./Utils"
export const UPDATE_EACH_INTERVAL = 2;
declare var iro: any;

const KELVIN_MAX = 6500;
const KELVIN_MIN = 2700;
export abstract class Widget {
  public abstract processIoState(r: C.sensact.comm.tState, i: number): void;
  public abstract renderWidgetAndHookHandlers(container: HTMLElement): void;
  constructor(protected manager: WidgetManager, protected id: number) { }
}



export class BlindsWidget extends Widget {
  private butBlindsUp!: HTMLButtonElement;
  private butBlindsStop!: HTMLButtonElement;
  private butBlindsDown!: HTMLButtonElement;

  public processIoState(r: C.sensact.comm.tState, i: number): void {
    if (r.states(i)?.stateType() != C.sensact.comm.uState.tBlindState) {
      return;
    }
    let state = r.states(i)!.state(new C.sensact.comm.tBlindState())!.state();
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

  private blindEventBuilder(e:C.sensact.comm.eBlindCommand):Uint8Array{
    let builder = new flatbuffers.Builder(1024);
    let evt = C.sensact.comm.tBlindCommand.createtBlindCommand(builder, e);
    let ioCmd = C.sensact.comm.tCommand.createtCommand(builder, C.sensact.comm.uCommand.tBlindCommand, this.id, evt);
    builder.finish(ioCmd);
    return builder.asUint8Array();
  }

  public renderWidgetAndHookHandlers(container: HTMLElement) {
    this.butBlindsUp = <HTMLButtonElement>$.Html(container, "button", ["updown"], "🢁");//<button id="blindsUp" class="updown" style="flex: 1 1 0;">&#129153;</button>
    this.butBlindsUp.style.flex = "1 1 0";
    this.butBlindsStop = <HTMLButtonElement>$.Html(container, "button");//<button id="blindsStop" style="flex: 1 1 0;"><svg class="icon"><use style="fill:black;" xlink:href="#icon-blinds"></use></svg></button>
    this.butBlindsStop.style.flex = "1 1 0";
    $.SvgIcon(this.butBlindsStop, "blinds").style.fill = "black";
    this.butBlindsDown = <HTMLButtonElement>$.Html(container, "button", ["updown"], "🢃");//<button id="blindsDown" class="updown" style="flex: 1 1 0;">&#129155;</button>
    this.butBlindsDown.style.flex = "1 1 0";

    this.butBlindsUp.onclick = (e) => {
      this.manager.http_put_iocmd(this.blindEventBuilder(C.sensact.comm.eBlindCommand.UP));
    }
    this.butBlindsStop.onclick = (e) => {
      this.manager.http_put_iocmd(this.blindEventBuilder(C.sensact.comm.eBlindCommand.STOP));
    }
    this.butBlindsDown.onclick = (e) => {
      this.manager.http_put_iocmd(this.blindEventBuilder(C.sensact.comm.eBlindCommand.DOWN));
    }
  }

  constructor(m: WidgetManager, id: number) {
    super(m, id);
  }
}


export class SinglePwmWidget extends Widget {
  private butSpotsOnOff!: HTMLButtonElement;
  private spotsPicker!: any;
  private firstCallOfProcessIoCtrl=true;

  public processIoState(r: C.sensact.comm.tState, i: number) {
    if (r.states(i)?.stateType() != C.sensact.comm.uState.tSinglePwmState) {
      return;
    }
    let on = r.states(i)!.state(new C.sensact.comm.tSinglePwmState())!.on();
    let intensity_0_100 = r.states(i)!.state(new C.sensact.comm.tSinglePwmState())!.intensity0100();
    this.butSpotsOnOff.style.backgroundColor=on?"green":"grey";
    if(this.firstCallOfProcessIoCtrl){
      this.spotsPicker.color.set(intensity_0_100);
      this.firstCallOfProcessIoCtrl=false;
    }
  }

 

  public renderWidgetAndHookHandlers(container: HTMLElement) {

    let pickerContainer = $.Html(container, "div", []);
    pickerContainer.style.flex = "1 1 0";
    pickerContainer.style.alignSelf = "center";
    this.butSpotsOnOff = <HTMLButtonElement>$.Html(container, "button", ["onoff"]);//<button id="blindsUp" class="updown" style="flex: 1 1 0;">&#129153;</button>
    this.butSpotsOnOff.style.flex = "0 0 0";
    $.SvgIcon(this.butSpotsOnOff, "spot").style.fill = "black";

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
    this.spotsPicker.on('input:change', (color: any, changes: any) => {
      let intensity0_100 = color.value;
      console.info(intensity0_100);
      let builder = new flatbuffers.Builder(1024);
      let evt = C.sensact.comm.tSinglePwmCommand.createtSinglePwmCommand(builder, C.sensact.comm.eSinglePwmCommand.CHANGE_INTENSITY, intensity0_100);
      let ioCmd = C.sensact.comm.tCommand.createtCommand(builder, C.sensact.comm.uCommand.tSinglePwmCommand, this.id, evt);
      builder.finish(ioCmd);
      this.manager.http_put_iocmd(builder.asUint8Array());
    });
    this.butSpotsOnOff.onclick = (e) => {
      let builder = new flatbuffers.Builder(1024);
      C.sensact.comm.tSinglePwmCommand.starttSinglePwmCommand(builder);
      C.sensact.comm.tSinglePwmCommand.addCmd(builder, C.sensact.comm.eSinglePwmCommand.TOGGLE);
      let evt = C.sensact.comm.tSinglePwmCommand.endtSinglePwmCommand(builder);
      let ioCmd = C.sensact.comm.tCommand.createtCommand(builder, C.sensact.comm.uCommand.tSinglePwmCommand, this.id, evt);
      builder.finish(ioCmd);
      this.manager.http_put_iocmd(builder.asUint8Array());
    }
  }

  constructor(manager: WidgetManager, id: number) {
    super(manager, id);
  }
}

export class OnOffWidget extends Widget {
  private butOn!: HTMLDivElement;
  private butAuto!: HTMLDivElement;
  private butOff!: HTMLDivElement;
  private selector!: HTMLDivElement;
  private state: C.sensact.comm.eOnOffState = C.sensact.comm.eOnOffState.AUTO_OFF;

  private updateUI(newState: C.sensact.comm.eOnOffState) {
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

  public processIoState(r: C.sensact.comm.tState, i: number): void {
    if (r.states(i)?.stateType() != C.sensact.comm.uState.tOnOffState) {
      return;
    }
    let state = r.states(i)!.state(new C.sensact.comm.tOnOffState())!.state();
    this.updateUI(state)
  }

  public renderWidgetAndHookHandlers(container: HTMLElement) {

    let subcontainer = <HTMLDivElement>$.Html(container, "div");//one single element
    subcontainer.style.flex = "1 1 0";
    subcontainer.style.position = "relative";
    let itemscontainer = <HTMLDivElement>$.Html(subcontainer, "div", ["switch_3_ways_container"]);//one single element//noiw, a container for the items; with flex layout
    itemscontainer.style.display = "flex";

    //itemscontainer.style.flexFlow="row nowrap";//default!
    //itemscontainer.style.justifyContent="flex-start";//default
    //itemscontainer.style.alignItems="stretch";//default
    this.butOn = <HTMLDivElement>$.Html(itemscontainer, "div", ["switch_3_ways_item"]);
    $.Html(this.butOn, "span", [], "On");
    this.butAuto = <HTMLDivElement>$.Html(itemscontainer, "div", ["switch_3_ways_item"]);
    $.Html(this.butAuto, "span", [], "Auto");

    this.butOff = <HTMLDivElement>$.Html(itemscontainer, "div", ["switch_3_ways_item"], "Off");
    $.Html(this.butOff, "span", [], "Off");

    this.selector = <HTMLDivElement>$.Html(subcontainer, "div", ["switch_3_ways_selector"]);
    this.selector.style.visibility = "hidden";
    $.Html(this.selector, "span", [], "");

    this.butOn.onclick = (e) => {
      let builder = new flatbuffers.Builder(1024);
      let evt = C.sensact.comm.tOnOffCommand.createtOnOffCommand(builder, C.sensact.comm.eOnOffCommand.ON, 0);
      let ioCmd = C.sensact.comm.tCommand.createtCommand(builder, C.sensact.comm.uCommand.tOnOffCommand, this.id, evt);
      builder.finish(ioCmd);
      let buf = builder.asUint8Array();
      this.manager.http_put_iocmd(buf);
    }
    this.butAuto.onclick = (e) => {
      let builder = new flatbuffers.Builder(1024);
      let evt = C.sensact.comm.tOnOffCommand.createtOnOffCommand(builder, C.sensact.comm.eOnOffCommand.AUTO, 0);
      let ioCtrl = C.sensact.comm.tCommand.createtCommand(builder, C.sensact.comm.uCommand.tOnOffCommand, this.id, evt);
      builder.finish(ioCtrl);
      let buf = builder.asUint8Array();
      this.manager.http_put_iocmd(buf);
    }
    this.butOff.onclick = (e) => {
      let builder = new flatbuffers.Builder(1024);
      let evt = C.sensact.comm.tOnOffCommand.createtOnOffCommand(builder, C.sensact.comm.eOnOffCommand.OFF, 0);
      let ioCmd = C.sensact.comm.tCommand.createtCommand(builder, C.sensact.comm.uCommand.tOnOffCommand, this.id, evt);
      builder.finish(ioCmd);
      let buf = builder.asUint8Array();
      this.manager.http_put_iocmd(buf);
    }
  }

  constructor(manager: WidgetManager, id: number) {
    super(manager, id);
  }
}