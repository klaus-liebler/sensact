import { Component, EventEmitter, Input, OnInit, Output } from '@angular/core';
import { Widget } from '../config-ui/config-ui.component';
import * as fb from 'flatbuffers';
import { tOnOffCommand } from '../sensact/comm/t-on-off-command';
import { tOnOffConfig } from '../sensact/comm/t-on-off-config';
import { tOnOffState } from '../sensact/comm/t-on-off-state';
import { tConfigWrapper } from '../sensact/comm/t-config-wrapper';
import {tCommand}  from '../sensact/comm/t-command';
import {uCommand}  from '../sensact/comm/u-command';
import {tState}  from '../sensact/comm/t-state';
import {uState}  from '../sensact/comm/u-state';
import * as E from '../sensact/comm';
import {uConfig}  from  '../sensact/comm/u-config';

@Component({
  selector: 'app-on-off-config-widget',
  templateUrl: './on-off-config-widget.component.html',
  styleUrls: ['./on-off-config-widget.component.scss']
})
export class OnOffConfigWidgetComponent implements OnInit {
  OnOffStates=E.eOnOffState;

  RelayInterlockModes=E.eRelayInterlockMode;
  _widgetData:Widget;

  public relay:number;
  public initialState:E.eOnOffState;
  public autoOffMsecs:number;
  
  
  @Input() set widgetData(w:Widget){
    this._widgetData=w;
    w.configWrapperCreator=this;
    if(w.rawData==null)return
    if(w.rawData.configType()!=uConfig.tOnOffConfig) return;
    this.relay=w.rawData.config(new tOnOffConfig()).relay();
    this.initialState=w.rawData.config(new tOnOffConfig()).initialState();
    this.autoOffMsecs=w.rawData.config(new tOnOffConfig()).autoOffMsecs();  
  }

  @Output() deleteItemEvent = new EventEmitter<number>();
  @Output() moveItemUpEvent = new EventEmitter<number>();
  @Output() moveItemDownEvent = new EventEmitter<number>();

  onBtnDeleteClicked=()=>this.deleteItemEvent.emit(this._widgetData.index);
  onBtnUpClicked=()=>this.moveItemUpEvent.emit(this._widgetData.index);
  onBtnDownClicked=()=>this.moveItemDownEvent.emit(this._widgetData.index);

  BuildAndReturnConfigWrapper(builder: fb.Builder):fb.Offset {
    let cfg3=tOnOffConfig.createtOnOffConfig(builder, this.relay, this.autoOffMsecs, this.initialState);
    return tConfigWrapper.createtConfigWrapper(builder, uConfig.tOnOffConfig, cfg3);
  }
  constructor() { }

  ngOnInit(): void {
  }

}
