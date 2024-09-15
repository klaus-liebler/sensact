import { ScreenController } from "./ScreenController";
import { AppManagement, ConfigWidgetManager } from "./Interfaces";
import { $ } from "./Utils";
import { flatbuffers } from 'flatbuffers'; // located in node_modules
import * as C from '../generated/webui_core_comm_generated';
import { BlindsConfigWidget, ConfigWidget, OnOffConfigWidget, SinglePwmConfigWidget } from "./ConfigWidgets";
import { DummyGenerator } from "./DummyData";
import M = C.sensact.comm;

export let DE_de = new Intl.NumberFormat('de-DE');
export const UPDATE_EACH_INTERVAL = 2;

enum ConfigWidgetTypes {
  Blind,
  OnOff,
  SinglePwm,

}

export class ConfigScreenController extends ScreenController implements ConfigWidgetManager {
  private widgets: Array<ConfigWidget> = new Array<ConfigWidget>();
  private divItemlist!: HTMLDivElement;
  private typeSelector!: HTMLSelectElement;

  public addWidgets(config: C.sensact.comm.tIoConfig) {
    for (let i = 0; i < config.configsLength(); i++) {
      let w: ConfigWidget | null = null;

      switch (config.configs(i)?.configType()) {
        case C.sensact.comm.uConfig.tBlindConfig:
          w = new BlindsConfigWidget(this);
          break;
        case C.sensact.comm.uConfig.tOnOffConfig:
          w = new OnOffConfigWidget(this);
          break;
        case C.sensact.comm.uConfig.tSinglePwmConfig:
          w = new SinglePwmConfigWidget(this);
          break;
      }
      if (!w) throw new Error("if(!w)");

      this.widgets.push(w);
      w.SetIndex(i);
    }
    this.redraw();
  }



  public onCreate(): void { }

  public onRestart(): void {

  }

  public onStop(): void {

  }



  public onFirstStart(): void {
    let top = <HTMLDivElement>$.Html(this.container, "div", ["top"]);
    this.typeSelector = $.LabelSpanSelectFillEnum(top, "Select a Widget", ConfigWidgetTypes, 0);
    $.Html(top, "button", ["styled"], "Add", ["type", "button"]).onclick = (e) => this.AddPressed();
    $.Html(top, "button", ["styled"], "Save", ["type", "button"]).onclick = (e) => this.SavePressed();;
    this.divItemlist = <HTMLDivElement>$.Html(this.container, "div", ["itemlist"]);
    let xhr = new XMLHttpRequest;
    xhr.responseType = "arraybuffer";
    xhr.onerror = (e) => {
      this.createWidgets(DummyGenerator.CreateIoConfig());
    };
    xhr.onload = (e) => {
      if (xhr.status < 400) {
        let arrayBuffer: ArrayBuffer = xhr.response;
        let buf = new flatbuffers.ByteBuffer(new Uint8Array(arrayBuffer));
        let cfg = C.sensact.comm.tIoConfig.getRootAstIoConfig(buf);
        this.createWidgets(cfg);
      }
      else
        this.createWidgets(DummyGenerator.CreateIoConfig());
    }
    xhr.open("GET", "/iocfg", true);

    xhr.send(/*body is ignored with get*/);
  }
  private createWidgets(config: C.sensact.comm.tIoConfig) {
    for (let i = 0; i < config.configsLength(); i++) {
      let w: ConfigWidget | null = null;
      switch (config.configs(i)?.configType()) {
        case C.sensact.comm.uConfig.tBlindConfig:
          w = new BlindsConfigWidget(this);
          break;
        case C.sensact.comm.uConfig.tOnOffConfig:
          w = new OnOffConfigWidget(this);
          break;
        case C.sensact.comm.uConfig.tSinglePwmConfig:
          w = new SinglePwmConfigWidget(this);
          break;
      }
      if (!w) throw new Error("!w")
      w.SetData(config.configs(i)!);
      w.SetIndex(i);
      w.renderWidget(this.divItemlist, i == 0, i + 1 == config.configsLength());
      this.widgets.push(w);

    }
    this.redraw();
  }
  private SavePressed(): void {
    let builder = new flatbuffers.Builder(1024);
    let configs_vect: number[] = [];
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
  private AddPressed(): void {
    let w: ConfigWidget | null = null;
    switch (Number.parseInt(this.typeSelector.value)) {
      case ConfigWidgetTypes.Blind:
        w = new BlindsConfigWidget(this);
        break;
      case ConfigWidgetTypes.OnOff:
        w = new OnOffConfigWidget(this);
        break;
      case ConfigWidgetTypes.SinglePwm:
        w = new SinglePwmConfigWidget(this);
        break;
    }
    if (!w) throw new Error("if(!w)")
    w.renderWidget(this.divItemlist, false, false);//muss hier gerendert werden, damit die Elemente exisitieren, die beim redraw->CopyDataFromHtmlToModel() ausgewertet werden
    this.widgets.push(w);
    this.redraw();
  }

  private redraw(): void {
    for (let i = 0; i < this.widgets.length; i++) {
      this.widgets[i].CopyDataFromHtmlToModel();
    }
    this.divItemlist.innerText = "";
    for (let i = 0; i < this.widgets.length; i++) {
      this.widgets[i].SetIndex(i);
      this.widgets[i].renderWidget(this.divItemlist, i == 0, i + 1 == this.widgets.length);
    }
  }

  constructor(appManagement: AppManagement, container: HTMLElement) {
    super(appManagement, container);
  }
  moveUp(i: number): void {
    let w = this.widgets[i - 1];
    this.widgets[i - 1] = this.widgets[i];
    this.widgets[i] = w;
    this.redraw();
  }
  moveDown(i: number): void {
    let w = this.widgets[i];
    this.widgets[i] = this.widgets[i + 1];
    this.widgets[i + 1] = w;
    this.redraw();
  }
  delete(i: number): void {
    this.widgets.splice(i, 1);
    this.redraw();
  }
}