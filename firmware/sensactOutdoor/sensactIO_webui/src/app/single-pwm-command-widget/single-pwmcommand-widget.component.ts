import { Component, NgZone, OnDestroy, OnInit, Input, ViewChild, ElementRef, AfterViewInit } from '@angular/core';
import iro from '@jaames/iro';
import { IroColorPicker } from '@jaames/iro/dist/ColorPicker';
import { CommunicationService } from '../communication.service';

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
  selector: 'app-single-pwmcommand-widget',
  templateUrl: './single-pwm-command-widget.component.html',
  styleUrls: ['./single-pwm-command-widget.component.scss']
})
export class SinglePwmCommandWidgetComponent implements OnInit, OnDestroy, AfterViewInit {

  public on:boolean=false;
  public intensity_0_1:number=0;
  private spotsPicker: IroColorPicker;
  private firstCallOfProcessIoCtrl=true;

  @ViewChild('iro')
  public iroDiv: ElementRef;

  private _applicationId:number;

  @Input() set applicationId(value:number){
    this._applicationId=value;
  }

  @Input() set state(r:tState){
    let i=this._applicationId-1;
    if (r.states(i)?.stateType() != uState.tSinglePwmState) {
      return;
    }
    this.on = r.states(i)!.state(new tSinglePwmState())!.on();
    this.intensity_0_1 = r.states(i)!.state(new tSinglePwmState())!.intensity01();
    //this.butSpotsOnOff.style.backgroundColor=on?"green":"grey";
    console.log(JSON.stringify({on:this.on, intensity_0_100:this.intensity_0_1, firstCallOfProcessIoCtrl:this.firstCallOfProcessIoCtrl}));
    if(this.spotsPicker && this.firstCallOfProcessIoCtrl){
      this.spotsPicker.color.setChannel("hsv", "v", this.intensity_0_1*100);
      this.firstCallOfProcessIoCtrl=false;
    }
  }

  public ngOnInit():void {}

  public ngOnDestroy(): void {
    this.spotsPicker.off('color:change', (color, changes)=>this.ngZone.run(() => this.onColorChange(color, changes)));
    this.spotsPicker=null;
  }
  

  public onColorChange(color:iro.Color, changes) {
    this.intensity_0_1 = color.hsv.v?color.hsv.v/100:0;
    let builder = new fb.Builder(1024);
    let cmd=tSinglePwmCommand.createtSinglePwmCommand(builder, E.eSinglePwmCommand.CHANGE_INTENSITY, this.intensity_0_1);
    let wcmd = tCommand.createtCommand(builder, this._applicationId, uCommand.tSinglePwmCommand, cmd);
    builder.finish(wcmd);
    let arr:Uint8Array = builder.asUint8Array();
    this.comm.putCommand(arr).subscribe((state:tState)=>{this.state=state});
  }

  public onToggle(){
    console.log(`Send TOGGLE with applicationId ${this._applicationId}`);
    let builder = new fb.Builder(1024);
    let cmd=tSinglePwmCommand.createtSinglePwmCommand(builder, E.eSinglePwmCommand.TOGGLE, this.spotsPicker.color.hsv.v);
    let wcmd = tCommand.createtCommand(builder, this._applicationId, uCommand.tSinglePwmCommand, cmd);
    builder.finish(wcmd);
    let arr:Uint8Array = builder.asUint8Array();
    this.comm.putCommand(arr).subscribe((state:tState)=>{this.state=state});
  }

  constructor(public ngZone: NgZone, private comm:CommunicationService) {
  }

  ngAfterViewInit(): void {
    let width = Math.min(400, 0.9 * this.iroDiv.nativeElement.offsetWidth);
    this.spotsPicker = iro.ColorPicker(this.iroDiv.nativeElement, {
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
    this.spotsPicker.on('color:change', (color, changes)=>this.ngZone.run(() => this.onColorChange(color, changes)));
  }


}
