import { ConfigWidgetManager } from "./Interfaces";
import { flatbuffers } from 'flatbuffers'; // located in node_modules
import * as C from '../generated/webui_core_comm_generated';
import { $ } from "./Utils"
import M=C.sensact.comm;


export abstract class ConfigWidget {
    private index: number = 0;
    public abstract fillFlatbuffer(builder:flatbuffers.Builder, vect:number[]): void;
    protected abstract renderWidgetCoreForm(container: HTMLDivElement): void;
    public renderWidget(container: HTMLElement, isFirst:boolean, isLast:boolean){
        let wCont= <HTMLDivElement>$.Html(container, "div");
        this.renderWidgetCoreForm(wCont);
        if(!isFirst)
            $.Html(wCont, "button", ["styled"], "▲", ["type", "button"]).onclick = (e) => this.UpPressed();
        if(!isLast)
            $.Html(wCont, "button", ["styled"], "▼", ["type", "button"]).onclick = (e) => this.DownPressed();;
        let btnDelete=$.Html(wCont, "button", ["styled"], "", ["type", "button"]);
        btnDelete.onclick = (e) => this.DeletePressed();
        $.SvgIcon(btnDelete, "bin2", ["smallicon"]);
    }

    public SetIndex(i: number) {
        this.index = i
    }
    public UpPressed() {
        this.manager.moveUp(this.index);
    }
    public abstract SetData(config:M.tConfigWrapper):void;
    public abstract CopyDataFromHtmlToModel():void;

    public DownPressed() {
        this.manager.moveDown(this.index);
    }

    public DeletePressed() {
        this.manager.delete(this.index);
    }

    constructor(protected manager: ConfigWidgetManager) { }
}

export class OnOffConfigWidget extends ConfigWidget{

    private inpRelay!: HTMLInputElement;
    private relay=0;
    private inpInitialState!: HTMLSelectElement;
    private initialState=M.eOnOffState.AUTO_OFF;
    private inpAutoOffMs!: HTMLInputElement;
    private autoOffMs=60000;

    
    protected renderWidgetCoreForm(container: HTMLDivElement): void {
        $.Html(container, "h3", [], "OnOff");
        this.inpRelay=$.LabelSpanInput(container, "Relay", this.relay, 0, 65535);
        this.inpInitialState=$.LabelSpanSelectFillEnum(container, "Initial State", M.eOnOffState, this.initialState)
        this.inpAutoOffMs=$.LabelSpanInput(container, "AutoOff [ms]", this.autoOffMs, 0, 100000);
    }
    public SetData(config: M.tConfigWrapper): void {
        if(config.configType()!=M.uConfig.tOnOffConfig) return;
        let x = config.config(new M.tOnOffConfig());
        if(!(x !== null && x !== undefined)) return;
        this.relay=x.relay();
        this.initialState=x.initialState();
        this.autoOffMs=x.autoOffMsecs();
    }
    public CopyDataFromHtmlToModel(): void {
        this.relay=this.inpRelay.valueAsNumber;
        this.initialState=Number.parseInt(this.inpInitialState.value);
        this.autoOffMs=this.inpAutoOffMs.valueAsNumber;
    }
    
    public fillFlatbuffer(builder:flatbuffers.Builder, vect:number[]): void {
        let x = M.tOnOffConfig.createtOnOffConfig(builder, this.relay, this.initialState, this.autoOffMs);
        vect.push(x);
        return;
    }
}


export class BlindsConfigWidget extends ConfigWidget {
   
    private inpRelay1!: HTMLInputElement;
    private relay1=0;
    private inpRelay2!: HTMLInputElement;
    private relay2=0;
    private inpMode!: HTMLSelectElement;
    private mode=M.eRelayInterlockMode.RELAY1_DOWN__RELAY2_UP;
    private inpUp!: HTMLInputElement;
    private up_msec=60000;
    private inpDown!: HTMLInputElement;
    private down_msec=60000;

    public renderWidgetCoreForm(wCont: HTMLDivElement): void{
        
        $.Html(wCont, "h3", [], "Blind");
        this.inpRelay1=$.LabelSpanInput(wCont, "Relay1", this.relay1, 0, 65535);
        this.inpRelay2=$.LabelSpanInput(wCont, "Relay2", this.relay2, 0, 65535);
        this.inpMode=$.LabelSpanSelectFillEnum(wCont, "Mode", M.eRelayInterlockMode, this.mode)
        this.inpUp=$.LabelSpanInput(wCont, "Up [ms]", this.up_msec, 1, 100000);
        this.inpDown=$.LabelSpanInput(wCont, "Down [ms]", this.down_msec, 1, 100000);
    }

    public SetData(config:M.tConfigWrapper):void{
        if(config.configType()!=M.uConfig.tBlindConfig) return;
        let x = config.config(new M.tBlindConfig());
        if(!(x !== null && x !== undefined)) return;
        this.relay1=x.relay1();
        this.relay2=x.relay2();
        this.mode=x.mode();
        this.up_msec=x.timeUpMsecs();
        this.down_msec=x.timeDownMsecs();          
    }

    public CopyDataFromHtmlToModel(){
        this.relay1=this.inpRelay1.valueAsNumber;
        this.relay2=this.inpRelay2.valueAsNumber;
        this.mode=Number.parseInt(this.inpMode.value);
        this.up_msec=this.inpUp.valueAsNumber;
        this.down_msec=this.inpDown.valueAsNumber;
    }


    public fillFlatbuffer(builder:flatbuffers.Builder, vect:number[]): void{
        let x = M.tBlindConfig.createtBlindConfig(builder, this.relay1, this.relay2, this.mode, this.up_msec, this.down_msec);
        vect.push(x);
        return;
    }
}

export class SinglePwmConfigWidget extends ConfigWidget {
    private inpPwmFirst!: HTMLInputElement;
    private pwmFirst=0;
    private inpPwmLast!: HTMLInputElement;
    private pwmLast=0;
    private inpStandbyControllerId!: HTMLInputElement;
    private standbyControllerId=0;
    private inpAutoOffMs!: HTMLInputElement;
    private autoOffMs=60000;

    public renderWidgetCoreForm(wCont: HTMLDivElement): void{
        $.Html(wCont, "h3", [], "SinglePwm");
        this.inpPwmFirst=$.LabelSpanInput(wCont, "pwm_first",this.pwmFirst, 0, 65535);
        this.inpPwmLast=$.LabelSpanInput(wCont, "pwm_last", this.pwmLast, 0, 65535);
        this.inpStandbyControllerId=$.LabelSpanInput(wCont, "stndby", this.standbyControllerId, 0, 65535);
        this.inpAutoOffMs=$.LabelSpanInput(wCont, "AutoOff [ms]", this.autoOffMs, 0, 100000);
    }

    public SetData(config:M.tConfigWrapper):void{
        if(config.configType()!=M.uConfig.tSinglePwmConfig) return;
        let x = config.config(new M.tSinglePwmConfig());
        if(!(x !== null && x !== undefined)) return;
        this.pwmFirst=x.pwmFirst();
        this.pwmLast=x.pwmLast();
        this.standbyControllerId=x.idOfStandbyController();
        this.autoOffMs=x.autoOffMsecs();     
    }

    public CopyDataFromHtmlToModel(){
        this.pwmFirst=this.inpPwmFirst.valueAsNumber;
        this.pwmLast=this.inpPwmLast.valueAsNumber;
        this.standbyControllerId=this.inpStandbyControllerId.valueAsNumber;
        this.autoOffMs=this.inpAutoOffMs.valueAsNumber;
    }

    public fillFlatbuffer(builder:flatbuffers.Builder, vect:number[]): void{
        let x = M.tSinglePwmConfig.createtSinglePwmConfig(builder, this.pwmFirst, this.pwmLast, this.standbyControllerId, this.autoOffMs);
        vect.push(x);
        return;
    }
}