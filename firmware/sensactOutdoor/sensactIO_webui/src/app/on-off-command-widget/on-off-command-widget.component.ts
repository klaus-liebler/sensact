import { AfterViewInit, Component, ElementRef, Input, OnInit, ViewChild } from '@angular/core';
import { CommunicationService } from '../communication.service';
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
  selector: 'app-on-off-command-widget',
  templateUrl: './on-off-command-widget.component.html',
  styleUrls: ['./on-off-command-widget.component.scss']
})
export class OnOffCommandWidgetComponent implements OnInit, AfterViewInit {
  public selectorWidth=0;
  public selectorLeft=0;
  public selectorText="";
  public selectorBackgroundColor="black";
  public selectorForegroundColor="white";
  public selectorVisibility="hidden";

  @ViewChild('selector')
  public selector: ElementRef;

  @ViewChild('butOn')
  public butOn: ElementRef;
  @ViewChild('butAuto')
  public butAuto: ElementRef;
  @ViewChild('butOff')
  public butOff: ElementRef;

  private onOffstate: E.eOnOffState = E.eOnOffState.AUTO_OFF;

  constructor(private comm:CommunicationService) { }
  ngAfterViewInit(): void {
    //this.updateUI(M.eOnOffState.AUTO_OFF);
  }

  @Input() public applicationId:number;
  @Input() set state(r:tState){
    let i=this.applicationId-1;
    if (r.states(i)?.stateType() != uState.tOnOffState) {
      return;
    }
    let onOffstate = r.states(i)!.state(new tOnOffState())!.state()
    if(this.selector && onOffstate!=this.onOffstate ){
      this.updateUI(onOffstate);
    }
  }

  private onCommand(c:E.eOnOffCommand){
    let builder = new fb.Builder(1024);
    let cmd=tOnOffCommand.createtOnOffCommand(builder, c, 0);
    let wcmd = tCommand.createtCommand(builder, this.applicationId, uCommand.tOnOffCommand, cmd);
    builder.finish(wcmd);
    let arr:Uint8Array = builder.asUint8Array();
    this.comm.putCommand(arr).subscribe((state:tState)=>{this.state=state});
  }

  onBtnOnClicked(){
    console.log(`Send ON with applicationId ${this.applicationId}`);
    this.onCommand(E.eOnOffCommand.ON);
  }

  onBtnAutoClicked(){
    console.log(`Send AUTO with applicationId ${this.applicationId}`);
    this.onCommand(E.eOnOffCommand.AUTO);
    
  }

  onBtnOffClicked(){
    console.log(`Send OFF with applicationId ${this.applicationId}`);
    this.onCommand(E.eOnOffCommand.OFF);
  }

  private updateUI(newState: E.eOnOffState) {
    this.onOffstate = newState;
    switch (this.onOffstate) {
      case E.eOnOffState.AUTO_OFF:
        this.selectorLeft = this.butOn.nativeElement.clientWidth;
        this.selectorWidth = this.butAuto.nativeElement.clientWidth;
        this.selectorText = "AUTO";
        this.selectorBackgroundColor = "black";
        this.selectorForegroundColor = "white";
        this.selectorVisibility = "visible";
        break;
      case E.eOnOffState.AUTO_ON:
        this.selectorLeft = this.butOn.nativeElement.clientWidth;
        this.selectorWidth = this.butAuto.nativeElement.clientWidth;
        this.selectorText = "AUTO";
        this.selectorBackgroundColor = "green";
        this.selectorForegroundColor = "white";
        this.selectorVisibility = "visible";
        break;
      case E.eOnOffState.MANUAL_ON:
        this.selectorLeft = 0;
        this.selectorWidth = this.butOn.nativeElement.clientWidth;
        this.selectorText = "ON";
        this.selectorBackgroundColor = "green";
        this.selectorForegroundColor = "black";
        this.selectorVisibility = "visible";
        break;
      case E.eOnOffState.MANUAL_OFF:
        this.selectorLeft = this.butOn.nativeElement.clientWidth + this.butAuto.nativeElement.clientWidth + 1;
        this.selectorWidth = this.butOff.nativeElement.clientWidth;
        this.selectorText = "OFF";
        this.selectorBackgroundColor = "black";
        this.selectorForegroundColor = "white";
        this.selectorVisibility = "visible";
        break;
    }
  }

  ngOnInit(): void {
  }

}
