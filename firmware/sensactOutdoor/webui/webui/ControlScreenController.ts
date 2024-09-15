import { ScreenController } from "./ScreenController";
import { AppManagement, WidgetContainer } from "./Interfaces";
import { BlindsWidget, SinglePwmWidget, OnOffWidget, Widget } from "./Widgets";
import { $ } from "./Utils";
import { flatbuffers } from 'flatbuffers'; // located in node_modules
import * as C from '../generated/webui_core_comm_generated';
import { DummyGenerator } from "./DummyData";

export let DE_de = new Intl.NumberFormat('de-DE');
export const UPDATE_EACH_INTERVAL = 2;


export class ControlScreenController extends ScreenController implements WidgetContainer{
  private widgets:Map<number, Widget>;
  private timeOfLastprocessIoCtrl=0;
  private timer: number | undefined;

  public createWidgets(config:C.sensact.comm.tIoConfig){
    let id=1;
    for(let i=0;i<config.configsLength();i++){
      let w:Widget|undefined;
      let widgetContainer= $.Html(this.container, "div", []);
      switch(config.configs(i)?.configType()){
        case C.sensact.comm.uConfig.tBlindConfig:
          w=new BlindsWidget(this, id);
          break;
        case C.sensact.comm.uConfig.tOnOffConfig:
          w=new OnOffWidget(this, id);
          break;
        case C.sensact.comm.uConfig.tSinglePwmConfig:
          w=new SinglePwmWidget(this, id);
          break;
      }
      if(w !== null && w !== undefined){
        this.widgets.set(id, w);
        w.renderWidgetAndHookHandlers(widgetContainer);
      }
      id++;
    }
  }

  public  http_put_iocmd(buf:Uint8Array) {
    this.timeOfLastprocessIoCtrl=Date.now()+60000;//um http_get_ioctrl zunächst mal zu vermeiden
    let xhr = new XMLHttpRequest;
    xhr.onerror = (e) => { console.log("Fehler beim XMLHttpRequest!"); };
    xhr.open("PUT", "/iocmd", true);
    xhr.responseType = "arraybuffer";
    xhr.onload = (e) => this.processIoState(xhr);
    xhr.send(buf);
  }

  public  http_get_iocmd() {
    if(Date.now()-this.timeOfLastprocessIoCtrl<800){
      return;
    }
    let xhr = new XMLHttpRequest;
    xhr.onerror = (e) => { console.log("Fehler beim XMLHttpRequest!"); };
    xhr.open("GET", "/iocmd", true);
    xhr.responseType = "arraybuffer";
    xhr.onload = (e) => this.processIoState(xhr);
    xhr.send();
  }

  public processIoState(xhr:XMLHttpRequest){
    this.timeOfLastprocessIoCtrl=Date.now()
    if(xhr.responseType!="arraybuffer"){
      console.error("No arraybuffer returned!");
      return;
    }
    let buf = new flatbuffers.ByteBuffer(new Uint8Array(xhr.response));
    let r= C.sensact.comm.tState.getRootAstState(buf);
    for(let i=0;i<r.statesLength();i++){
      let appId=r.states(i)!.applicationId();
      let widget=this.widgets.get(appId);
      widget?.processIoState(r, i);
    }
  }

  public onCreate(): void { }
  
  public onRestart(): void {
    this.timer = window.setInterval(() => { this.http_get_iocmd(); }, 1000);
  }
  
  public onStop(): void {
    window.clearInterval(this.timer);
  }
  
  public onFirstStart(): void {
    this.timer = window.setInterval(() => { this.http_get_iocmd(); }, 1000);

    
    let xhr = new XMLHttpRequest;
    xhr.responseType = "arraybuffer";
    xhr.onerror = (e) => {
      this.createWidgets(DummyGenerator.CreateIoConfig());
    };
    xhr.onload = (e) => {
      if(xhr.status<400){
        let arrayBuffer:ArrayBuffer=xhr.response;
        let buf = new flatbuffers.ByteBuffer(new Uint8Array(arrayBuffer));
        let cfg= C.sensact.comm.tIoConfig.getRootAstIoConfig(buf);
        this.createWidgets(cfg);
      }
      else
        this.createWidgets(DummyGenerator.CreateIoConfig());
    }
    xhr.open("GET", "/iocfg", true);
    
    xhr.send(/*body is ignored with get*/);
  }
  
  constructor(appManagement: AppManagement, container: HTMLElement) {
    super(appManagement, container);
    this.widgets = new Map<number, Widget>();
  }
}