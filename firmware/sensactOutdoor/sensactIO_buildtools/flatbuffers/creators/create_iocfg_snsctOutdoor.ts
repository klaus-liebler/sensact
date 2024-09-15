import * as fs from 'fs';
import * as path from 'path';
import * as fb from 'flatbuffers';
import {eBlindCommand} from '../../generated/flatbuffer_idl/sensact/comm/e-blind-command';
import {eBlindState} from '../../generated/flatbuffer_idl/sensact/comm/e-blind-state';
import {eOnOffCommand} from '../../generated/flatbuffer_idl/sensact/comm/e-on-off-command';
import {tBlindConfig} from '../../generated/flatbuffer_idl/sensact/comm/t-blind-config';
import {tConfigWrapper} from '../../generated/flatbuffer_idl/sensact/comm/t-config-wrapper';
import {tIoConfig} from '../../generated/flatbuffer_idl/sensact/comm/t-io-config';
import {tOnOffConfig} from '../../generated/flatbuffer_idl/sensact/comm/t-on-off-config';
import {tRgbwPwmConfig} from '../../generated/flatbuffer_idl/sensact/comm/t-rgbw-pwm-config';
import {tSinglePwmConfig} from '../../generated/flatbuffer_idl/sensact/comm/t-single-pwm-config';
import * as U from '../../generated/flatbuffer_idl/sensact/comm/u-config';
import * as E from '../../generated/flatbuffer_idl/sensact/comm';

let builder:any = new fb.Builder(1024);
let vect:number[]=[];
{
    let cfg=tBlindConfig.createtBlindConfig(builder, 1,2,E.eRelayInterlockMode.R1_DOWN__R2_UP, 120*1000, 120*1000);
    let wCfg = tConfigWrapper.createtConfigWrapper(builder, U.uConfig.tBlindConfig, cfg);
    vect.push(wCfg);
}
{
    let cfg=tBlindConfig.createtBlindConfig(builder, 3,4,E.eRelayInterlockMode.R1_DOWN__R2_UP, 120*1000, 120*1000);
    let wCfg = tConfigWrapper.createtConfigWrapper(builder, U.uConfig.tBlindConfig, cfg);
    vect.push(wCfg);
}
{
    let cfg=tSinglePwmConfig.createtSinglePwmConfig(builder, 12, 12, 4, 6*60*60*1000);
    let wCfg = tConfigWrapper.createtConfigWrapper(builder, U.uConfig.tSinglePwmConfig, cfg);
    vect.push(wCfg);
}
{
    let cfg=tOnOffConfig.createtOnOffConfig(builder, 5, 10000, E.eOnOffState.AUTO_OFF);
    let wCfg = tConfigWrapper.createtConfigWrapper(builder, U.uConfig.tOnOffConfig, cfg);
    vect.push(wCfg);
}
let cfg_vect = tIoConfig.createConfigsVector(builder, vect);
let cfg= tIoConfig.createtIoConfig(builder, 4714, cfg_vect);
builder.finish(cfg);
let bytes = builder.asUint8Array();
let full_path = process.argv[2];
fs.mkdirSync(path.dirname(full_path), { recursive: true });
fs.writeFileSync(full_path, bytes, "binary");

let bytesRead = fs.readFileSync(full_path);
let buf:any = new fb.ByteBuffer(bytesRead);

let cfgRead = tIoConfig.getRootAstIoConfig(buf);
console.log(`Timestamp is ${cfgRead.timestamp()} and configsLength is ${cfgRead.configsLength()}. Flatbuffers file written to ${full_path}`);