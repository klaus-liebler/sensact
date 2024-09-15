import { Component, OnDestroy, OnInit } from '@angular/core';
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
import { tIoConfig } from '../sensact/comm/t-io-config';

export interface ConfigWrapperCreator {
  BuildAndReturnConfigWrapper(builder: fb.Builder): fb.Offset;
}

export class Widget {
  constructor(
    public type: uConfig,
    public index: number,
    public isLast: boolean,
    public rawData: tConfigWrapper = null,
    public configWrapperCreator: ConfigWrapperCreator = null,
  ) { }
}

@Component({
  selector: 'app-config-ui',
  templateUrl: './config-ui.component.html',
  styleUrls: ['./config-ui.component.scss']
})
export class ConfigUiComponent implements OnInit {
  uConfig = uConfig;
  widgets = new Array<Widget>();

  public currentlySelectedWidgetType = uConfig.NONE;

  public onBtnAddClicked() {
    if (this.currentlySelectedWidgetType == uConfig.NONE) return;
    let l = this.widgets.length;
    if (l > 0) {
      this.widgets[l - 1].isLast = false;
    }
    let w = new Widget(this.currentlySelectedWidgetType, this.widgets.length, true);
    this.widgets.push(w);
  }

  public onBtnSaveClicked() {
    let builder = new fb.Builder(1024);
    let cfgs:fb.Offset[]=[];
    for(let w of this.widgets){
      cfgs.push(w.configWrapperCreator.BuildAndReturnConfigWrapper(builder));
    }
    let cfg_vect = tIoConfig.createConfigsVector(builder, cfgs);
    let cfg= tIoConfig.createtIoConfig(builder, 0, cfg_vect);
    builder.finish(cfg);
    let arr= builder.asUint8Array();
    this.comm.putIoConfig(arr).subscribe();
  }

  public moveItemUpHandler(index: number) {
    if (index < 1 || index > this.widgets.length) return;
    let temp = this.widgets[index - 1];
    this.widgets[index - 1] = this.widgets[index];
    this.widgets[index] = temp;
    this.widgets[index - 1].index = index - 1;
    this.widgets[index - 1].isLast = false;
    this.widgets[index].index = index;
    this.widgets[index].isLast=(index==(this.widgets.length-1));
  }

  public moveItemDownHandler(index: number) {
    if (index < 0 || index > this.widgets.length - 2) return;
    let temp = this.widgets[index + 1];
    this.widgets[index + 1] = this.widgets[index];
    this.widgets[index] = temp;
    this.widgets[index].index = index;
    this.widgets[index].isLast = false;
    this.widgets[index + 1].index = index + 1;
    this.widgets[index+1].isLast=((index+1)==(this.widgets.length-1));
  }

  public deleteItemHandler(index: number) {
    if (index < 0 || !(index < this.widgets.length)) return;
    this.widgets.splice(index, 1);
    this.widgets[this.widgets.length-1].isLast=true;//mache dies einfach immer, auch wenn es nicht nötig wäre. Ein Setzen auf false geht schneller, als eine Prüfung, ob das notwendig ist
  }
  constructor(public comm: CommunicationService) { }

  ngOnInit(): void {
    this.comm.getIoConfig().subscribe(cfg => {
      let len = cfg.configsLength();
      for (let i = 0; i < len; i++) {
        let w = new Widget(cfg.configs(i).configType(), i, i == len - 1, cfg.configs(i));
        this.widgets.push(w);
        console.log("Created " + w.type);
      }
    });
  }

}
