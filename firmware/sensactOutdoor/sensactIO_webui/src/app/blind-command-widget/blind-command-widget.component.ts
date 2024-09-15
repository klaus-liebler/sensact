import { Component, Input, OnInit } from '@angular/core';
import * as fb from 'flatbuffers'; // located in node_modules
import { CommunicationService } from '../communication.service';
import {tBlindCommand}  from '../sensact/comm/t-blind-command';
import {tBlindState}  from '../sensact/comm/t-blind-state';
import {tCommand}  from '../sensact/comm/t-command';
import {uCommand}  from '../sensact/comm/u-command';
import {tState}  from '../sensact/comm/t-state';
import {uState}  from '../sensact/comm/u-state';
import * as E from '../sensact/comm';


@Component({
  selector: 'app-blind-command-widget',
  templateUrl: './blind-command-widget.component.html',
  styleUrls: ['./blind-command-widget.component.scss']
})
export class BlindCommandWidgetComponent implements OnInit {
  eBlindState=E.eBlindState;
  constructor(private comm:CommunicationService) { }
  public blState:E.eBlindState;

  @Input() public applicationId:number;
  @Input() set state(r:tState){
    let i=this.applicationId-1;
    if (r.states(i)?.stateType() != uState.tBlindState) {
      return;
    }
    this.blState = r.states(i)!.state(new tBlindState()).state();
  }

  private onCommand(c:E.eBlindCommand){
    let builder = new fb.Builder(1024);
    let cmd=tBlindCommand.createtBlindCommand(builder, c);
    let wcmd = tCommand.createtCommand(builder, this.applicationId, uCommand.tBlindCommand, cmd);
    builder.finish(wcmd);
    let arr:Uint8Array = builder.asUint8Array();
    this.comm.putCommand(arr).subscribe((state:tState)=>{this.state=state});
  }

  onBtnUpClicked(){
    console.log(`Send UP with applicationId ${this.applicationId}`);
    this.onCommand(E.eBlindCommand.UP);
  }

  onBtnDownClicked(){
    console.log(`Send DOWN with applicationId ${this.applicationId}`);
    this.onCommand(E.eBlindCommand.DOWN);
    
  }

  onBtnStopClicked(){
    console.log(`Send STOP with applicationId ${this.applicationId}`);
    this.onCommand(E.eBlindCommand.STOP);
  }
  
  

  ngOnInit(): void {
  }

}
