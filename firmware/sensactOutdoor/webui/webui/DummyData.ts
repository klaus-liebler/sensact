import { flatbuffers } from 'flatbuffers'; // located in node_modules
import * as C from '../generated/webui_core_comm_generated';

export class DummyGenerator{
    public static CreateIoConfig():C.sensact.comm.tIoConfig{
        let builder = new flatbuffers.Builder(1024);
        let cfg1=C.sensact.comm.tBlindConfig.createtBlindConfig(builder, 1,2,C.sensact.comm.eRelayInterlockMode.RELAY1_UP__RELAY2_DOWN, 30000, 30000);
        let wCfg1 = C.sensact.comm.tConfigWrapper.createtConfigWrapper(builder, C.sensact.comm.uConfig.tBlindConfig, cfg1);
        let cfg2=C.sensact.comm.tSinglePwmConfig.createtSinglePwmConfig(builder, 3, 3, 3, 20000);
        let wCfg2 = C.sensact.comm.tConfigWrapper.createtConfigWrapper(builder, C.sensact.comm.uConfig.tSinglePwmConfig, cfg2);
        let cfg3=C.sensact.comm.tOnOffConfig.createtOnOffConfig(builder, 4, 20000, C.sensact.comm.eOnOffState.AUTO_OFF);
        let wCfg3 = C.sensact.comm.tConfigWrapper.createtConfigWrapper(builder, C.sensact.comm.uConfig.tOnOffConfig, cfg3);
        let cfg_vect = C.sensact.comm.tIoConfig.createConfigsVector(builder, [wCfg1, wCfg2, wCfg3]);
        let cfg= C.sensact.comm.tIoConfig.createtIoConfig(builder, 0, cfg_vect);
        builder.finish(cfg);
        let bytes = builder.asUint8Array();
        let buf = new flatbuffers.ByteBuffer(bytes);
        return C.sensact.comm.tIoConfig.getRootAstIoConfig(buf);
    }
}