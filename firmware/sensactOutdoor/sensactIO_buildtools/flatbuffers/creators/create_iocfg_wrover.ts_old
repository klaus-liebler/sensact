import * as fs from 'fs';
import * as path from 'path';
import { flatbuffers } from 'flatbuffers'; // located in node_modules
import * as C from '../../generated/flatbuffer_idl/webui_core_comm_generated';
import M=C.sensact.comm;

function createwrover(b:flatbuffers.Builder):void{
    let vect=[];
    for(let output=1;output<=3;output++){
        let cfg=M.tSinglePwmConfig.createtSinglePwmConfig(builder, output, output, 0, 10000);
        let wCfg = M.tConfigWrapper.createtConfigWrapper(builder, C.sensact.comm.uConfig.tSinglePwmConfig, cfg);
        vect.push(wCfg);
    }
    let cfg_vect = C.sensact.comm.tIoConfig.createConfigsVector(builder, vect);
    let cfg= C.sensact.comm.tIoConfig.createtIoConfig(builder, 4713, cfg_vect);
    builder.finish(cfg);
}

let builder = new flatbuffers.Builder(1024);
createwrover(builder);

let bytes = builder.asUint8Array();
let full_path = process.argv[2];
fs.mkdirSync(path.dirname(full_path), { recursive: true });
fs.writeFileSync(full_path, bytes, "binary");

let bytesRead = fs.readFileSync(full_path);
let buf = new flatbuffers.ByteBuffer(bytesRead);

let cfgRead = C.sensact.comm.tIoConfig.getRootAstIoConfig(buf);
console.log(`Timestamp is ${cfgRead.timestamp()} and configsLength is ${cfgRead.configsLength()}. Flatbuffers file written to ${full_path}`);