import { ScreenController } from "./ScreenController";
import { AppManagement } from "./AppManagement";
import { SerializeContext } from "./SerializeContext";
import {$} from "./Utils"
export let DE_de = new Intl.NumberFormat('de-DE');
export const UPDATE_EACH_INTERVAL = 2;

declare var iro: any;
const KELVIN_MAX = 6500;
const KELVIN_MIN = 2700;
export class Widget{
  protected  http_put_webui(command:string, obj:any=null) {
    if(!obj || obj===undefined){
      obj=new Object();
    }
    obj["id"]=this.id;
    obj["command"]=command;

    let xhr = new XMLHttpRequest;
    xhr.onerror = (e) => { console.log("Fehler beim XMLHttpRequest!"); };
    xhr.open("PUT", "/webui", true);
    xhr.setRequestHeader("Content-Type", "application/json;charset=UTF-8");
    xhr.send(JSON.stringify(obj));
  }

  protected updateButton(but: HTMLButtonElement, state: boolean) {
    but.innerHTML = state? "An" : "Aus";
  }
  protected http_get_webui() {//nein, das wird auf Screen-Ebene für alle Widgets auf der Seite auf einmal gemacht!
    let xhr = new XMLHttpRequest;
    xhr.onerror = (e) => { console.log("Fehler beim XMLHttpRequest!"); };
    xhr.open("GET", "/webui", true);
    xhr.setRequestHeader("Content-Type", "application/json;charset=UTF-8");
    xhr.onload = (e) => this.parseHTTPResult(xhr.responseText);
    xhr.send(/*body is ignored with get*/);
  }

  protected parseHTTPResult(value:string){}

  constructor(protected appManagement: AppManagement, protected container: HTMLDivElement, protected id:number){}
}
export class BlindsWidget extends Widget {
  private butBlindsUp!: HTMLButtonElement;
  private butBlindsStop!: HTMLButtonElement;
  private butBlindsDown!: HTMLButtonElement;

  protected parseHTTPResult(value:string) {

  }

  public renderWidgetAndHookHHandlers(){
    this.butBlindsUp=<HTMLButtonElement>$.Html(this.container, "button", [], ["updown"], "&#129153;");//<button id="blindsUp" class="updown" style="flex: 1 1 0;">&#129153;</button>
    this.butBlindsUp.style.flex="1 1 0";
    this.butBlindsStop=<HTMLButtonElement>$.Html(this.container, "button", [], [], "&#129153;");//<button id="blindsStop" style="flex: 1 1 0;"><svg class="icon"><use style="fill:black;" xlink:href="#icon-blinds"></use></svg></button>
    this.butBlindsStop.style.flex="1 1 0";
    $.SvgIcon( this.butBlindsStop, "blinds").style.fill="black";
    this.butBlindsDown=<HTMLButtonElement>$.Html(this.container, "button", [], ["updown"], "&#129155;");//<button id="blindsDown" class="updown" style="flex: 1 1 0;">&#129155;</button>
    this.butBlindsDown.style.flex="1 1 0";

    this.butBlindsUp.onclick = (e) => this.http_put_webui("upPressed");
    this.butBlindsStop.onclick = (e) => this.http_put_webui("stopPressed");
    this.butBlindsDown.onclick = (e) => this.http_put_webui("downPressed");
  }

  constructor(appManagement: AppManagement, container: HTMLDivElement, id:number) {
    super(appManagement, container, id);
    this.renderWidgetAndHookHHandlers();
  }
}
export class SinglePWMWidget extends Widget {


  private butSpotsOnOff!: HTMLButtonElement;
  private spotsPicker!: any;

  protected parseHTTPResult(value:string){
    let obj = JSON.parse(value);
    this.updateButton(this.butSpotsOnOff, obj["on"]);
    this.spotsPicker.color.value = obj["intensity"];
    
  }
  /*
<div id="spotsPicker" style="flex: 1 1 0; align-self: center;"></div>
<button id="spotsOnOff" class="onoff" style="flex: 0 0 0;">
    <svg class="icon icon-onoff">
        <use style="fill:black;" xlink:href="#icon-spot"></use>
    </svg>
</button>

  */

  public renderWidgetAndHookHHandlers(){
    
    let pickerContainer = $.Html(this.container, "div", []);
    pickerContainer.style.flex="1 1 0";
    pickerContainer.style.alignSelf="center";
    this.butSpotsOnOff=<HTMLButtonElement>$.Html(this.container, "button", [], ["onoff"], "?");//<button id="blindsUp" class="updown" style="flex: 1 1 0;">&#129153;</button>
    this.butSpotsOnOff.style.flex="0 0 0";
    $.SvgIcon( this.butSpotsOnOff, "spot").style.fill="black";

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
    this.spotsPicker.on('input:change', (color: any, changes: any) =>{
      let val=color.value;
      console.info(val);
      this.http_put_webui("intensityChanged", {intensity0_100:val});
    });
    this.butSpotsOnOff.onclick = (e) => this.http_put_webui("togglePressed");

  }

 

  constructor(appManagement: AppManagement, container: HTMLElement) {
    super(appManagement, container);

    
  }
}