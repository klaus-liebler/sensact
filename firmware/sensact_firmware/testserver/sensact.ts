
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
        console.info(`SinglePwmTwin ${ApplicationId[this.appId]} got command ${Command[cmd.cmd()]}. state[0]=${this.state[0]} state[1]=${this.state[1]}`);
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

enum  eCurrentBlindState
		{
			ENERGY_SAVE,
			STOP,
			PREPARE_UP,
			PREPARE_DOWN,
			UP,
			DOWN,
		};

class BlindTwin extends aStatusProvider {

    private position:number = 50; // Simulated position of the blind (0-100%)
    private blindState:eCurrentBlindState = eCurrentBlindState.STOP;
    private sender:ISender | undefined = undefined;
    
    constructor(appId: ApplicationId) {
        super(appId);
        setInterval(() => {
            switch (this.blindState) {
                case eCurrentBlindState.UP:
                    this.position += 5; // Simulate moving up
                    if(this.position >= 100) {
                        this.blindState = eCurrentBlindState.STOP; // Stop when reaching the top
                        this.position = 100;
                    }
                    break;
                case eCurrentBlindState.DOWN:
                    this.position -= 5; // Simulate moving up
                    if(this.position <0) {
                        this.blindState = eCurrentBlindState.STOP; // Stop when reaching the top
                        this.position = 0;
                    }
                    break;
                default:
                    break;
            }
            this.state[0] = this.blindState; // Update the blind state
            this.state[1] = this.position; // Update the position
            console.info(`BlindTwin ${ApplicationId[this.appId]} updates state. state[0]=${eCurrentBlindState[this.state[0]]} state[1]=${this.state[1]}`);
            if(this.sender){
                let b = new flatbuffers.Builder(1024);
                NotifyStatus.startNotifyStatus(b);
                NotifyStatus.addId(b, this.appId);
                NotifyStatus.addStatus(b, StatusPayload.createStatusPayload(b, this.state));
                const notifyStatusOffset = NotifyStatus.endNotifyStatus(b);
                b.finish(ResponseWrapper.createResponseWrapper(b, Responses.NotifyStatus, notifyStatusOffset));
                this.sender.send(Namespace.Value, b)
            }
        }, 1000)
    }

    UpdateStateWithCommand(cmd: RequestCommand, sender: ISender): void {
        this.sender = sender;;
        const p=cmd.payload(new Payload())
        switch(cmd.cmd()) {
            case Command.UP:
                this.blindState=eCurrentBlindState.UP;
                break
            case Command.DOWN:
                this.blindState=eCurrentBlindState.DOWN;
                break;
            default:
                this.blindState=eCurrentBlindState.STOP;
                break

        }
        console.info(`BlindTwin ${ApplicationId[this.appId]} got command ${Command[cmd.cmd()]}. state[0]=${this.state[0]} state[1]=${this.state[1]}`);

        
    }
   
}

export class SensactHandler extends NamespaceAndHandler {
    private Map: Map<ApplicationId, aStatusProvider> = new Map<ApplicationId, aStatusProvider>();

    constructor() {
        super(Namespace.Value);
        this.Map.set(ApplicationId.PWM___L0_PRTY_S, new SinglePwmTwin(ApplicationId.PWM___L0_PRTY_S));
        this.Map.set(ApplicationId.PWM___L1_BATH_S, new SinglePwmTwin(ApplicationId.PWM___L1_BATH_S));
        this.Map.set(ApplicationId.PWM___L2_BATH_S, new SinglePwmTwin(ApplicationId.PWM___L2_BATH_S));
        this.Map.set(ApplicationId.PWM___L3_WORK_S, new SinglePwmTwin(ApplicationId.PWM___L3_WORK_S));
        this.Map.set(ApplicationId.BLIND_L1_BATH_J1, new BlindTwin(ApplicationId.BLIND_L1_BATH_J1));
        this.Map.set(ApplicationId.BLIND_L2_BATH_J1, new BlindTwin(ApplicationId.BLIND_L2_BATH_J1));
        this.Map.set(ApplicationId.BLIND_L3_WORK_J1, new BlindTwin(ApplicationId.BLIND_L3_WORK_J1));
       
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

