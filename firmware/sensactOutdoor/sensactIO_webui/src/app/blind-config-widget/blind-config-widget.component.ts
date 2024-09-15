import { Component, Input, OnInit, Output, EventEmitter} from '@angular/core';
import { ConfigWrapperCreator, Widget } from '../config-ui/config-ui.component';
import * as fb from 'flatbuffers';
import {tBlindCommand}  from '../sensact/comm/t-blind-command';
import {tBlindState}  from '../sensact/comm/t-blind-state';
import {tBlindConfig}  from '../sensact/comm/t-blind-config';
import { tConfigWrapper } from '../sensact/comm/t-config-wrapper';
import {tCommand}  from '../sensact/comm/t-command';
import {uCommand}  from '../sensact/comm/u-command';
import {tState}  from '../sensact/comm/t-state';
import {uState}  from '../sensact/comm/u-state';
import * as E from '../sensact/comm';
import {uConfig}  from  '../sensact/comm/u-config';




@Component({
  selector: 'app-blind-config-widget',
  templateUrl: './blind-config-widget.component.html',
  styleUrls: ['./blind-config-widget.component.scss'],
})
export class BlindConfigWidgetComponent implements OnInit, ConfigWrapperCreator {
  RelayInterlockModes=E.eRelayInterlockMode;
  _widgetData:Widget;

  public relay1:number;
  public relay2:number;
  public timeUpMsecs:number;
  public timeDownMsecs:number;
  public relayInterlockMode:E.eRelayInterlockMode;

  @Input() set widgetData(w:Widget){
    this._widgetData=w;
    w.configWrapperCreator=this;
    if(w.rawData==null)return
    if(w.rawData.configType()!=uConfig.tBlindConfig) return;
    this.relay1=w.rawData.config(new tBlindConfig()).relay1();
    this.relay2=w.rawData.config(new tBlindConfig()).relay2();
    this.timeUpMsecs=w.rawData.config(new tBlindConfig()).timeUpMsecs();
    this.timeDownMsecs=w.rawData.config(new tBlindConfig()).timeDownMsecs();
    this.relayInterlockMode=w.rawData.config(new tBlindConfig()).mode();
  }

  @Output() deleteItemEvent = new EventEmitter<number>();
  @Output() moveItemUpEvent = new EventEmitter<number>();
  @Output() moveItemDownEvent = new EventEmitter<number>();

  onBtnDeleteClicked=()=>this.deleteItemEvent.emit(this._widgetData.index);
  onBtnUpClicked=()=>this.moveItemUpEvent.emit(this._widgetData.index);
  onBtnDownClicked=()=>this.moveItemDownEvent.emit(this._widgetData.index);

  constructor() { }
  BuildAndReturnConfigWrapper(builder: fb.Builder):fb.Offset {
    let cfg1=tBlindConfig.createtBlindConfig(builder, this.relay1,this.relay2,this.relayInterlockMode, this.timeUpMsecs, this.timeDownMsecs);
    return tConfigWrapper.createtConfigWrapper(builder, uConfig.tBlindConfig, cfg1);
  }

  ngOnInit(): void {
  }

}
