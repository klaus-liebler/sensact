import { Component, NgZone, OnDestroy, OnInit } from '@angular/core';
import { Subscription } from 'rxjs';
import { CommunicationService } from '../communication.service';
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

interface Widget{
  applicationId:number;
  type:uState;
}

@Component({
  selector: 'app-command-ui',
  templateUrl: './command-ui.component.html',
  styleUrls: ['./command-ui.component.scss']
})
export class CommandUiComponent implements OnInit, OnDestroy {
  public uState=uState;
  private info:Subscription|null;
  public widgets:Array<Widget>=new Array<Widget>();
  public state:tState;
  
  constructor(private ngZone: NgZone, public ioState:CommunicationService) { }
  ngOnDestroy(): void {
    this.info?.unsubscribe();
  }

  ngOnInit(): void {
    this.info = this.ioState.getIoState().subscribe((r)=>{
      let len = r.statesLength();
      if(this.widgets.length!=len){
        this.widgets= new Array<Widget>(len);
      }
      for(let i=0;i<len;i++){
        if(this.widgets[i]==null 
            || this.widgets[i].applicationId!=r.states(i).applicationId()
            || this.widgets[i].type!=r.states(i).stateType()){
              this.widgets[i]={applicationId:r.states(i).applicationId(), type:r.states(i).stateType()};
              console.log("Created "+this.widgets[i].type);
            } 
      }
      this.state=r;
    });
  }

}
