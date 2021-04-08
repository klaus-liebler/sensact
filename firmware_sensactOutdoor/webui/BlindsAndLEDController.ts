import { ScreenController } from "./ScreenController";
import { AppManagement } from "./AppManagement";
import { SerializeContext } from "./SerializeContext";
export let DE_de = new Intl.NumberFormat('de-DE');
export const UPDATE_EACH_INTERVAL = 2;

declare var iro: any;
const KELVIN_MAX = 6500;
const KELVIN_MIN = 2700;
export class BlindsAndLEDController extends ScreenController {

  private butRgbOnOff: HTMLButtonElement;
  private butWwcwOnOff: HTMLButtonElement;
  private butSpotsOnOff: HTMLButtonElement;
  private butAwningUp: HTMLButtonElement;
  private butAwningStop: HTMLButtonElement;
  private butAwningDown: HTMLButtonElement;
  private butBlindsUp: HTMLButtonElement;
  private butBlindsStop: HTMLButtonElement;
  private butBlindsDown: HTMLButtonElement;
  private rgbPicker: any;
  private wwcwKelvinPicker: any;
  private wwcwIntensityPicker: any;
  private spotsPicker: any;

  public onCreate(): void { }
  public onFirstStart(): void { }
  public onRestart(): void { }
  public onStop(): void { }

  private http_put_webui(eApplicationId: number, eventId: number, payload?: number[]) {
    let buffer = new ArrayBuffer(256);
    let ctx = new SerializeContext(buffer);
    ctx.writeU32(eApplicationId);
    ctx.writeU32(eventId);
    payload?.forEach((v) => ctx.writeU32(v));

    let xhr = new XMLHttpRequest;
    xhr.onerror = (e) => { console.log("Fehler beim XMLHttpRequest!"); };
    xhr.open("PUT", "/webui", true);
    xhr.responseType = "arraybuffer";
    xhr.onload = (e) => this.parseHTTPResult(xhr.response);// Note: not oReq.responseText
  }

  private http_get_webui() {
    let xhr = new XMLHttpRequest;
    xhr.onerror = (e) => { console.log("Fehler beim XMLHttpRequest!"); };
    xhr.open("GET", "/webui", true);
    xhr.responseType = "arraybuffer";
    xhr.onload = (e) => this.parseHTTPResult(xhr.response); // Note: not oReq.responseText
  }

  private updateButton(but: HTMLButtonElement, state: number) {
    but.innerHTML = state == 0 ? "Aus" : "An";
  }

  private parseHTTPResult(arrayBuffer: ArrayBuffer) {
    if (!arrayBuffer || arrayBuffer.byteLength/4 < 6+5+4) {
      console.error("! arrayBuffer || arrayBuffer.byteLength");
      return;
    }
    let ctx = new SerializeContext(arrayBuffer);
    if(ctx.readU32()!=1) console.error("ctx.readU32()!=1");
    if(ctx.readU32()!=4) console.error("ctx.readU32()!=4");
    let rgbR = ctx.readU32();
    let rgbG = ctx.readU32();
    let rgbB = ctx.readU32();
    let rgbOnOff = ctx.readU32();
    
    if(ctx.readU32()!=2) console.error("ctx.readU32()!=2");
    if(ctx.readU32()!=3) console.error("ctx.readU32()!=3");
    let wwcwKelvin = ctx.readU32();
    let wwcwIntensity = ctx.readU32();
    let wwcwOnOff = ctx.readU32()

    if(ctx.readU32()!=3) console.error("ctx.readU32()!=3");
    if(ctx.readU32()!=2) console.error("ctx.readU32()!=2");
    let spots = ctx.readU32();
    let spotsOnOff = ctx.readU32();
    
    this.rgbPicker.color.rgb = { r: rgbR, g: rgbG, b: rgbB };
    this.updateButton(this.butRgbOnOff, rgbOnOff);
    this.wwcwKelvinPicker.color.kelvin = wwcwKelvin;
    this.wwcwIntensityPicker.color.value = wwcwIntensity;
    this.updateButton(this.butWwcwOnOff, wwcwOnOff);
    this.spotsPicker.color.value = spots;
    this.updateButton(this.butSpotsOnOff, spotsOnOff);
  }

  constructor(appManagement: AppManagement, container: HTMLElement) {
    super(appManagement, container);
    this.butRgbOnOff = <HTMLButtonElement>document.getElementById("rgbOnOff")!;
    this.butWwcwOnOff = <HTMLButtonElement>document.getElementById("wwcwOnOff")!;
    this.butSpotsOnOff = <HTMLButtonElement>document.getElementById("spotsOnOff")!;
    this.butAwningUp = <HTMLButtonElement>document.getElementById("awningUp")!;
    this.butAwningStop = <HTMLButtonElement>document.getElementById("awningStop")!;
    this.butAwningDown = <HTMLButtonElement>document.getElementById("awningDown")!;
    this.butBlindsUp = <HTMLButtonElement>document.getElementById("blindsUp")!;
    this.butBlindsStop = <HTMLButtonElement>document.getElementById("blindsStop")!;
    this.butBlindsDown = <HTMLButtonElement>document.getElementById("blindsDown")!;

    this.butRgbOnOff.onclick = (e) => this.http_put_webui(1, 1);
    this.butWwcwOnOff.onclick = (e) => this.http_put_webui(2, 2);
    this.butSpotsOnOff.onclick = (e) => this.http_put_webui(3, 1);
    this.butAwningUp.onclick = (e) => this.http_put_webui(4, 0);
    this.butAwningStop.onclick = (e) => this.http_put_webui(4, 1);
    this.butAwningDown.onclick = (e) => this.http_put_webui(4, 2);
    this.butBlindsUp.onclick = (e) => this.http_put_webui(5, 0);
    this.butBlindsStop.onclick = (e) => this.http_put_webui(5, 1);
    this.butBlindsDown.onclick = (e) => this.http_put_webui(5, 2);


    let width = (<HTMLDivElement>document.querySelector("#rgbPicker")).offsetWidth;
    width = Math.min(400, 0.9 * width);
    this.rgbPicker = new iro.ColorPicker('#rgbPicker', { width: width });
    this.wwcwKelvinPicker = new iro.ColorPicker("#wwcwKelvinPicker", {
      width: width,
      layoutDirection: 'vertical',
      layout: [
        {
          component: iro.ui.Slider,
          options: {
            sliderType: 'kelvin',
            minTemperature: KELVIN_MIN,
            maxTemperature: KELVIN_MAX

          }
        },
      ]
    });
    this.wwcwIntensityPicker = new iro.ColorPicker("#wwcwIntensityPicker", {
      width: width,
      layoutDirection: 'vertical',
      layout: [
        {
          component: iro.ui.Slider,
          options: {
            sliderType: 'value',
          }
        },
      ]
    });
    this.spotsPicker = new iro.ColorPicker("#spotsPicker", {
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

    this.rgbPicker.on('input:change', (color: any, changes: any) => this.http_put_webui(1, 0, [color.red, color.green, color.blue]));
    this.wwcwKelvinPicker.on('input:change', (color: any, changes: any) => {
      let val = color.kelvin;
      console.info(val);
      this.http_put_webui(2, 0, [val]);
    });
    this.wwcwIntensityPicker.on('input:change', (color: any, changes: any) =>{
      let val=color.value;
      console.info(val);
      this.http_put_webui(2, 1, [val]);
    }); 
    this.spotsPicker.on('input:change', (color: any, changes: any) =>{
      let val=color.value;
      console.info(val);
      this.http_put_webui(3, 0, [val]);
    });
  }
}