import { ScreenController } from "./ScreenController";
import { AppManagement, WidgetContainer } from "./Interfaces";
import { BlindsWidget, SinglePWMWidget, StandbyControllerWidget, Widget } from "./Widgets";
import { $ } from "./Utils";
import { flatbuffers } from 'flatbuffers'; // located in node_modules
import * as C from '../generated/webui_core_comm_generated';

export let DE_de = new Intl.NumberFormat('de-DE');
export const UPDATE_EACH_INTERVAL = 2;


export class IOScreenController extends ScreenController implements WidgetContainer{
  private widgets:Map<number, Widget>;
  private timeOfLastprocessIoCtrl=0;
  private timer: number | undefined;

  public createWidgets(responseText:string){
    let obj=JSON.parse(responseText);
    let id=1;
    for (const item of obj.items) {
      let widgetContainer= $.Html(this.container, "div", []);
      let type:string=item.type;
      switch(type){
        //TODO: Do this in a more generic way
        case "BLIND":
          this.widgets.set(id, new BlindsWidget(this, widgetContainer,id));
          console.info("BLIND");
          break;
        case "SINGLEPWM":
          this.widgets.set(id, new SinglePWMWidget(this, widgetContainer,id));
          console.info("SINGLEPWM");
          break;
        case "STANDBYCONTROLLER":
          this.widgets.set(id, new StandbyControllerWidget(this, widgetContainer,id));
          console.info("STANDBYCONTROLLER");
          break;
      }
      id++;
      
    }
    for(const w of this.widgets.values()){
      w.renderWidgetAndHookHandlers();
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
    let dummy =JSON.stringify({
      timestamp:new Date(),
      version:1,
      items:[
        {
          type:"BLIND",
          relay1:0,
          relay2:1,
          mode:"RELAY1_UP__RELAY2_DOWN"
        },
        {
          type:"SINGLEPWM",
          pwm:2,
          idOfStandbyController:3,
        },
        {
          type:"STANDBYCONTROLLER",
          relay:2,
          waitTimeMsecs:10000,
        },
      ],
    });

    let xhr = new XMLHttpRequest;
    xhr.onerror = (e) => {
      this.createWidgets(dummy);
    };
    xhr.onload = (e) => {
      if(xhr.status<400)
        this.createWidgets(xhr.responseText);
      else
        this.createWidgets(dummy);
    }
    xhr.open("GET", "/iocfg", true);
    
    xhr.send(/*body is ignored with get*/);
  }
  
  constructor(appManagement: AppManagement, container: HTMLElement) {
    super(appManagement, container);
    this.widgets = new Map<number, Widget>();
  }
}