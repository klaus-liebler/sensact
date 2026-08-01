import { ISensactContext } from "@klaus-liebler/sensact-base/interfaces";
import * as Apps from "@klaus-liebler/web-components-sensact/sensactapps_base";
import { sensact } from "@generated/wsprotocol_ts/ws-protocol";
import ApplicationId = sensact.ApplicationId;

export default function Build(ctx:ISensactContext):Array<Apps.SensactApplicationAndLocalFlag>{
    var ret = new Array<Apps.SensactApplicationAndLocalFlag>();
    //TEMPLATE_HERE
    return ret;
}