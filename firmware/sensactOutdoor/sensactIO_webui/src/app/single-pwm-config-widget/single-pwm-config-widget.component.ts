import { Component, EventEmitter, Input, OnInit, Output } from '@angular/core';
import { Widget } from '../config-ui/config-ui.component';
import * as fb from 'flatbuffers';
import { tSinglePwmCommand } from '../sensact/comm/t-single-pwm-command';
import { tSinglePwmConfig } from '../sensact/comm/t-single-pwm-config';
import { tSinglePwmState } from '../sensact/comm/t-single-pwm-state';
import { tConfigWrapper } from '../sensact/comm/t-config-wrapper';
import {tCommand}  from '../sensact/comm/t-command';
import {uCommand}  from '../sensact/comm/u-command';
import {tState}  from '../sensact/comm/t-state';
import {uState}  from '../sensact/comm/u-state';
import * as E from '../sensact/comm';
import {uConfig}  from  '../sensact/comm/u-config';


@Component({
  selector: 'app-single-pwm-config-widget',
  templateUrl: './single-pwm-config-widget.component.html',
  styleUrls: ['./single-pwm-config-widget.component.scss']
})
export class SinglePwmConfigWidgetComponent implements OnInit {

  _widgetData:Widget;

  public pwmFirst:number;
  public pwmLast:number;
  public idOfStandbyController:number;
  public autoOffMsecs:number;
  
  
  @Input() set widgetData(w:Widget){
    this._widgetData=w;
    w.configWrapperCreator=this;
    if(w.rawData==null)return
    if(w.rawData.configType()!=uConfig.tSinglePwmConfig) return;
    this.pwmFirst=w.rawData.config(new tSinglePwmConfig()).pwmFirst();
    this.pwmLast=w.rawData.config(new tSinglePwmConfig()).pwmLast();
    this.idOfStandbyController=w.rawData.config(new tSinglePwmConfig()).idOfStandbyController();
    this.autoOffMsecs=w.rawData.config(new tSinglePwmConfig()).autoOffMsecs();  
  }

  @Output() deleteItemEvent = new EventEmitter<number>();
  @Output() moveItemUpEvent = new EventEmitter<number>();
  @Output() moveItemDownEvent = new EventEmitter<number>();

  onBtnDeleteClicked=()=>this.deleteItemEvent.emit(this._widgetData.index);
  onBtnUpClicked=()=>this.moveItemUpEvent.emit(this._widgetData.index);
  onBtnDownClicked=()=>this.moveItemDownEvent.emit(this._widgetData.index);

  BuildAndReturnConfigWrapper(builder: fb.Builder):fb.Offset {
    let cfg2=tSinglePwmConfig.createtSinglePwmConfig(builder, this.pwmFirst, this.pwmLast, this.idOfStandbyController, this.autoOffMsecs);
    return tConfigWrapper.createtConfigWrapper(builder, uConfig.tSinglePwmConfig, cfg2);
  }
  constructor() { }

  ngOnInit(): void {
  }


}
