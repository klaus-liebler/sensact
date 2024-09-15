import { Injectable, OnDestroy, OnInit } from '@angular/core';
import { Observable, timer, Subscription, Subject, of } from 'rxjs';
import { HttpClient } from '@angular/common/http';
import { switchMap, map, tap, share, retry, takeUntil, filter, catchError } from 'rxjs/operators';
import * as fb from 'flatbuffers';

import {tState}  from './sensact/comm/t-state';
import {uState}  from './sensact/comm/u-state';
import { tBlindState } from './sensact/comm/t-blind-state';
import { tSinglePwmState } from './sensact/comm/t-single-pwm-state';
import { tOnOffState } from './sensact/comm/t-on-off-state';
import { tStateWrapper } from './sensact/comm/t-state-wrapper';

import * as E from './sensact/comm';


import { environment } from 'src/environments/environment';
import { tIoConfig } from './sensact/comm/t-io-config';
import { tBlindConfig } from './sensact/comm/t-blind-config';
import { tConfigWrapper } from './sensact/comm/t-config-wrapper';
import { uConfig } from './sensact/comm/u-config';
import { tSinglePwmConfig } from './sensact/comm/t-single-pwm-config';
import { tOnOffConfig } from './sensact/comm/t-on-off-config';

const IOSTATE_URL = "/iostate";
const IOCONFIG_URL = "/iocfg";

@Injectable({
  providedIn: 'root'
})
export class CommunicationService implements OnInit, OnDestroy{
  
  ngOnInit(): void {
    
  }
  ngOnDestroy(): void {
    this.stopPolling.next(true);
  }


  private ioState: Observable<tState>;
  private stopPolling = new Subject();
  private lastTimePUT:number=0;

  private static randomBlindState():E.eBlindState{
    let v=Math.random();
    return v<0.33?E.eBlindState.STOP:v<0.66?E.eBlindState.UP:E.eBlindState.DOWN;
  }

  private static randomOnOffState():E.eOnOffState{
    let v=Math.random();
    return v<0.50?E.eOnOffState.AUTO_OFF:E.eOnOffState.AUTO_ON;
  }

  public static CreateIoState():ArrayBuffer{
    let builder = new fb.Builder(1024);
    let cfg1=tBlindState.createtBlindState(builder, 8347, this.randomBlindState());
    let wCfg1 = tStateWrapper.createtStateWrapper(builder, 1, uState.tBlindState, cfg1);
    let cfg2=tSinglePwmState.createtSinglePwmState(builder, 50+10*Math.random(), Math.random()>0.5);
    let wCfg2 = tStateWrapper.createtStateWrapper(builder, 2, uState.tSinglePwmState, cfg2);
    let cfg3=tOnOffState.createtOnOffState(builder, this.randomOnOffState());
    let wCfg3 = tStateWrapper.createtStateWrapper(builder, 3, uState.tOnOffState, cfg3);
    let cfg_vect = tState.createStatesVector(builder, [wCfg1, wCfg2, wCfg3]);
    let cfg= tState.createtState(builder, 0, cfg_vect);
    builder.finish(cfg);
    return builder.asUint8Array();
}

public static CreateIoConfig():tIoConfig{
  let builder = new fb.Builder(1024);
  let cfg1=tBlindConfig.createtBlindConfig(builder, 2,3,E.eRelayInterlockMode.R1_DOWN__R2_UP, 10000, 5042);
  let wCfg1 = tConfigWrapper.createtConfigWrapper(builder, uConfig.tBlindConfig, cfg1);
  let cfg2=tSinglePwmConfig.createtSinglePwmConfig(builder, 1,3,0, 10000);
  let wCfg2 = tConfigWrapper.createtConfigWrapper(builder, uConfig.tSinglePwmConfig, cfg2);
  let cfg3=tOnOffConfig.createtOnOffConfig(builder, 3, 9900, E.eOnOffState.AUTO_OFF);
  let wCfg3 = tConfigWrapper.createtConfigWrapper(builder, uConfig.tOnOffConfig, cfg3);
  let cfg_vect = tIoConfig.createConfigsVector(builder, [wCfg1, wCfg2, wCfg3]);
  let cfg= tIoConfig.createtIoConfig(builder, 0, cfg_vect);
  builder.finish(cfg);
  let arr= builder.asUint8Array();
  let buf = new fb.ByteBuffer(new Uint8Array(arr));
      return tIoConfig.getRootAstIoConfig(buf);
}

public getIoConfig():Observable<tIoConfig>{
  if(!environment.production){
    return of(CommunicationService.CreateIoConfig());
  }
  return this.http.get(IOCONFIG_URL, {responseType: 'arraybuffer', }).pipe(
    map((arraybuffer)=>{
      this.lastTimePUT=Date.now();
      let buf = new fb.ByteBuffer(new Uint8Array(arraybuffer));
      return tIoConfig.getRootAstIoConfig(buf);
    })
  );
}

private handleError<T>(operation = 'operation', result?: T) {
  return (error: any): Observable<T> => {

    // TODO: send the error to remote logging infrastructure
    console.error(error); // log to console instead

    // TODO: better job of transforming error for user consumption
    //this.log(`${operation} failed: ${error.message}`);

    // Let the app keep running by returning an empty result.
    return of(result as T);
  };
}

public putIoConfig(arr: Uint8Array):Observable<{}> {
  let buffer = arr.buffer.slice(arr.byteOffset, arr.byteOffset+arr.byteLength)
  
  let arr1=new Uint8Array(buffer);
  let s=""
  if(!environment.production){
    for(let i=0;i<arr1.length;i++){
      s+=arr[i].toString(16)+", ";
    }
  }
  console.log(`arr.byteOffset: ${arr.byteOffset} arr.byteLenght: ${arr.byteLength} content: ${s}`);
  return this.http.put(IOCONFIG_URL, buffer, {headers: {"Content-Type": "application/octet-stream"}}).pipe(
    tap( () => console.log('tapped putIoConfig')),
    catchError(this.handleError<object>('putIoConfig', []))
  );
}

public putCommand(arr: Uint8Array): Observable<tState> {
  this.lastTimePUT= Date.now()+100000;
  console.log(`arr.byteOffset: ${arr.byteOffset} arr.byteLenght: ${arr.byteLength} lenght ${arr.length}`);
  let buffer = arr.buffer.slice(arr.byteOffset, arr.byteOffset+arr.byteLength)
  return this.http.put(IOSTATE_URL, buffer, {responseType: 'arraybuffer', headers: {"Content-Type": "application/octet-stream"},}).pipe(
    map((arraybuffer)=>{
      this.lastTimePUT=Date.now();
      let buf = new fb.ByteBuffer(new Uint8Array(arraybuffer));
      return tState.getRootAstState(buf);
    })
  );
}

public getIoState(): Observable<tState> {
  return this.ioState.pipe(   
    tap(() => console.log('data sent to subscriber'))
  );
}
  

  constructor(private http: HttpClient) {
    this.ioState = timer(1, 5000).pipe(
      filter(()=>{return Date.now()>this.lastTimePUT+1000}),
      environment.production?switchMap(() => http.get(IOSTATE_URL, {responseType: 'arraybuffer'})):map(()=>{return CommunicationService.CreateIoState()}),
      tap((x)=>{console.log(`Received an ioState message with len ${x.byteLength}`)}),
      map((arraybuffer)=>{
        let buf = new fb.ByteBuffer(new Uint8Array(arraybuffer));
        return tState.getRootAstState(buf);
      }),
      retry(),
      share(),
      takeUntil(this.stopPolling)
   );
  }
}
