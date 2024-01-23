import { UNIT_SEPARATOR, RECORD_SEPARATOR, GROUP_SEPARATOR, gel } from "./utils";
import { CommandMessage } from "../generated/websensact";
import { ApplicationId } from "../generated/application-id";
import { Command } from "../generated/command";
import * as flatbuffers from 'flatbuffers';
import * as x from "../generated/sendCommandImplementation";




export function onoff(myself: HTMLInputElement, id: ApplicationId, internalApp: boolean = true) {
  if (myself.checked) {
    x.SendONCommand(id, 0);
  } else {
    x.SendOFFCommand(id, 0);
  }
  console.log(`onoff ${id} ${myself.checked}`);
}

export function blind_stop(myself: HTMLInputElement, id: ApplicationId, internalApp: boolean = true) {
  x.SendSTOPCommand(id);
  console.log(`blind_stop ${id}`);
}

export function blind_up(myself: HTMLInputElement, id: ApplicationId, internalApp: boolean = true) {
  x.SendUPCommand(id, 1);
  console.log(`blind_up ${id}`);
}

export function blind_down(myself: HTMLInputElement, id: ApplicationId, internalApp: boolean = true) {
  x.SendDOWNCommand(id, 1);
  console.log(`blind_down ${id}`);
}

export function singlepwm_toggle(myself: HTMLInputElement, id: number, internalApp: boolean = true) {
  if (myself.checked) {
    x.SendONCommand(id, 0);
  } else {
    x.SendOFFCommand(id, 0);
  }
  console.log(`singlepwm_toggle ${id} ${myself.checked} `);
}

export function singlepwm_slider(myself: HTMLInputElement, id: ApplicationId, internalApp: boolean = true) {
  x.SendSET_VERTICAL_TARGETCommand(id, parseInt(myself.value));
  console.log(`singlepwm_slider ${id} ${myself.value}`);
}
const URL_PREFIX = "http://127.0.0.1:3000";
const WS_PREFIX="ws://127.0.0.1:3000";
const RESTART_URL = URL_PREFIX + "/restart";
const WS_URL=WS_PREFIX+"/log";

const GET_TEXT: RequestInit = { method: "GET", mode: "no-cors" };
const MAX_MESSAGE_COUNT=20;

class AppController {

  private btnUpdateState = <HTMLInputElement>gel("btnUpdateState");
  private scroller = <HTMLDivElement>gel('scroller');
  private anchor = <HTMLDivElement>gel('anchor');
  private socket:WebSocket;
  private messageCount=0;
  private ansi_escape =new RegExp("(\\x9B|\\x1B\\[)[0-?]*[ -\\/]*[@-~]");

  constructor() { }

  private log(message:string) {
    let msg = document.createElement('p');
    if(message.startsWith("I")){
      msg.className = 'info';
    }else if(message.startsWith("W")){
      msg.className = 'warn';
    }else{
      msg.className = 'error';
    }
    msg.innerHTML =message.replace(this.ansi_escape, "");
    this.scroller.insertBefore(msg, this.anchor);
    this.messageCount++;
    if(this.messageCount>MAX_MESSAGE_COUNT){
      this.scroller.removeChild(this.scroller.firstChild!);
    }
};


private connect (server:string) {
    try {
        this.log(`IConnecting to ${server}` );
        try{
          this.socket.close();
        }catch(e){
          //do nothing
        }
        this.socket = new WebSocket(server);
        this.socket.onerror = (event:Event)=>{ this.log('ESocketError'); };
        this.socket.onopen = (event)=>{this.log('ISocket.open');};
        this.socket.onmessage = (event)=> {this.log(event.data); };
    } catch (e) {
        this.log('E '+e);
    }
};






  public startup() {
    this.connect(WS_URL);


    this.btnUpdateState.onclick = (e: MouseEvent) => {
      fetch(RESTART_URL, GET_TEXT)
      .then((e) => { 
        e.text(); 
      })
      .catch(e=>{console.error(e)})
      .then(txt =>{
        console.log(txt);
      })
      .catch(e=>{console.error(e)});
      
    };
  }
}

let app: AppController;
document.addEventListener("DOMContentLoaded", (e) => {
  app = new AppController();
  app.startup();
});