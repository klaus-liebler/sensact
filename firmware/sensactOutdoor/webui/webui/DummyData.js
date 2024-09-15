"use strict";
var __createBinding = (this && this.__createBinding) || (Object.create ? (function(o, m, k, k2) {
    if (k2 === undefined) k2 = k;
    Object.defineProperty(o, k2, { enumerable: true, get: function() { return m[k]; } });
}) : (function(o, m, k, k2) {
    if (k2 === undefined) k2 = k;
    o[k2] = m[k];
}));
var __setModuleDefault = (this && this.__setModuleDefault) || (Object.create ? (function(o, v) {
    Object.defineProperty(o, "default", { enumerable: true, value: v });
}) : function(o, v) {
    o["default"] = v;
});
var __importStar = (this && this.__importStar) || function (mod) {
    if (mod && mod.__esModule) return mod;
    var result = {};
    if (mod != null) for (var k in mod) if (k !== "default" && Object.prototype.hasOwnProperty.call(mod, k)) __createBinding(result, mod, k);
    __setModuleDefault(result, mod);
    return result;
};
Object.defineProperty(exports, "__esModule", { value: true });
exports.DummyGenerator = void 0;
const flatbuffers_1 = require("flatbuffers"); // located in node_modules
const C = __importStar(require("../generated/webui_core_comm_generated"));
class DummyGenerator {
    static CreateIoConfig() {
        let builder = new flatbuffers_1.flatbuffers.Builder(1024);
        let cfg1 = C.sensact.comm.tBlindConfig.createtBlindConfig(builder, 1, 2, C.sensact.comm.eRelayInterlockMode.RELAY1_UP__RELAY2_DOWN, 30000, 30000);
        let wCfg1 = C.sensact.comm.tConfigWrapper.createtConfigWrapper(builder, C.sensact.comm.uConfig.tBlindConfig, cfg1);
        let cfg2 = C.sensact.comm.tSinglePwmConfig.createtSinglePwmConfig(builder, 3, 3, 3, 20000);
        let wCfg2 = C.sensact.comm.tConfigWrapper.createtConfigWrapper(builder, C.sensact.comm.uConfig.tSinglePwmConfig, cfg2);
        let cfg3 = C.sensact.comm.tOnOffConfig.createtOnOffConfig(builder, 4, 20000, C.sensact.comm.eOnOffState.AUTO_OFF);
        let wCfg3 = C.sensact.comm.tConfigWrapper.createtConfigWrapper(builder, C.sensact.comm.uConfig.tOnOffConfig, cfg3);
        let cfg_vect = C.sensact.comm.tIoConfig.createConfigsVector(builder, [wCfg1, wCfg2, wCfg3]);
        let cfg = C.sensact.comm.tIoConfig.createtIoConfig(builder, 0, cfg_vect);
        builder.finish(cfg);
        let bytes = builder.asUint8Array();
        let buf = new flatbuffers_1.flatbuffers.ByteBuffer(bytes);
        return C.sensact.comm.tIoConfig.getRootAstIoConfig(buf);
    }
}
exports.DummyGenerator = DummyGenerator;
//# sourceMappingURL=DummyData.js.map