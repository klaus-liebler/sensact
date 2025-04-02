
import * as flatbuffers from "flatbuffers"

import { ISender, NamespaceAndHandler } from "@klaus-liebler/websocket_file_testserver/utils"
import { ApplicationId, Command, Namespace, NotifyStatus, Payload, RequestCommand, Requests, RequestStatus, RequestWrapper, ResponseCommand, Responses, ResponseStatus, ResponseStatusItem, ResponseWrapper, StatusPayload } from "@generated/flatbuffers_ts/sensact";

abstract class aStatusProvider {
    protected state:Array<number> = new Array<number>(4);
    public GetState(): Array<number>{
        return this.state;
    }
    abstract UpdateStateWithCommand(cmd: RequestCommand, sender: ISender): void;
    constructor(protected appId: ApplicationId){
        this.state[0] = 0;
        this.state[1] = 0;
        this.state[2] = 0;
        this.state[3] = 0;
    }
};

  /*
	eAppCallResult cSinglePWM::FillStatus(iSensactContext &ctx, std::array<uint16_t, 4>& buf){
			buf[0]=currentLevel==0?0:1; // 1=ON, 0=OFF
			buf[1]=currentLevel;
			buf[2]=autoDim;
			buf[3]=targetLevel;;
			return eAppCallResult::OK;
		}
  */

class SinglePwmTwin extends aStatusProvider {

    UpdateStateWithCommand(cmd: RequestCommand, sender: ISender): void {
        const p=cmd.payload(new Payload())
        if(cmd.cmd() == Command.SET_VERTICAL_TARGET) {
            this.state[1]=(p.data(0)<<8)+(p.data(1)<<0);
            this.state[0]=this.state[1]==0?0:1; // 1=ON, 0=OFF
        } else if(cmd.cmd() == Command.TOGGLE   ) {
            this.state[0]=this.state[0]==0?1:0;
        }
        console.info(`SinglePwmTwin ${this.appId} got command ${Command[cmd.cmd()]}. state[0]=${this.state[0]} state[1]=${this.state[1]}`);
        setTimeout(() => {
                    console.log(`sendUpdateState ${this.appId}`)
                    let b = new flatbuffers.Builder(1024);
                    NotifyStatus.startNotifyStatus(b);
                    NotifyStatus.addId(b, this.appId);
                    NotifyStatus.addStatus(b, StatusPayload.createStatusPayload(b, this.state));
                    const notifyStatusOffset = NotifyStatus.endNotifyStatus(b);
                    b.finish(ResponseWrapper.createResponseWrapper(b, Responses.NotifyStatus, notifyStatusOffset));
                    sender.send(Namespace.Value, b)
                   
                }, 500);
        
    }
    constructor(appId: ApplicationId) {
        super(appId); 
    }
}

export class SensactHandler extends NamespaceAndHandler {
    private Map: Map<ApplicationId, aStatusProvider> = new Map<ApplicationId, aStatusProvider>();

    constructor() {
        super(Namespace.Value);
        this.Map.set(ApplicationId.PWM___X1_XX1_42, new SinglePwmTwin(ApplicationId.PWM___X1_XX1_42));
      
       
        //console.log(`Die id im ersten item ist ${s.states(0).id()} ${s.states(0).status()!.data(0)} ${s.states(0).status()!.data(1)} ${s.states(0).status()!.data(2)} ${s.states(0).status()!.data(3)}`);
        //console.log(`Die id im zweiten item ist ${s.states(1).id()} ${s.states(1).status()!.data(0)} ${s.states(1).status()!.data(1)} ${s.states(1).status()!.data(2)} ${s.states(1).status()!.data(3)}`);
    }

    public Handle(buffer: flatbuffers.ByteBuffer, sender: ISender) {
        let b = new flatbuffers.Builder(1024);
        var rw=RequestWrapper.getRootAsRequestWrapper(buffer)
        switch(rw.requestType()){
            case Requests.NONE:return;
            case Requests.RequestStatus:{
                const rs=rw.request(new RequestStatus()) as RequestStatus;
                const ids = rs.idsArray();
                console.info(`got RequestStatus for ids ${ids.join(",")}`);
                if (!ids) return; // Falls keine IDs vorhanden sind, beende die Verarbeitung
                ResponseStatus.startStatesVector(b, ids.length);
                
                for (const id of ids) {
                    const payload = this.Map.get(id)?.GetState() ?? [0, 0, 0, 0];
                    console.info(`Sensact Status Request ${id} [${ApplicationId[id]}] ${payload[0]} ${payload[1]} ${payload[2]} ${payload[3]}`);
                    ResponseStatusItem.createResponseStatusItem(b,  id, payload)
                }
                const statesOffset=b.endVector();
            	
                b.finish(
                    ResponseWrapper.createResponseWrapper(
                        b,
                        Responses.ResponseStatus, 
                        ResponseStatus.createResponseStatus(b, statesOffset)
                    )
                );
                sender.send(Namespace.Value, b);


                break;
            }
            case Requests.RequestCommand:{
                const rs=rw.request(new RequestCommand()) as RequestCommand;
                console.log(`Sensact Command ${Command[rs.cmd()]} to ${ApplicationId[rs.id()]}`)
                if(this.Map.has(rs.id())) {
                    const app=this.Map.get(rs.id())!;
                    app.UpdateStateWithCommand(rs, sender);
                }
                b.finish(
                    ResponseWrapper.createResponseWrapper(
                        b,
                        Responses.ResponseCommand, 
                        ResponseCommand.createResponseCommand(
                            b
                        )
                    )
                );
                sender.send(Namespace.Value, b);
                break;
            }
        }
    }
}

