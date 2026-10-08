
import { ISender, NamespaceAndHandler } from "@klaus-liebler/websocket_file_testserver/utils"
import { sensact } from "@generated/wsprotocol_ts/ws-protocol";

const ApplicationId = sensact.ApplicationId;
const Command = sensact.Command;
type ApplicationId = sensact.ApplicationId;

function sendStatus(sender: ISender, appId: ApplicationId, state: number[]) {
    sender.send(sensact.NotifyStatus.encode({ requestId: 0, id: appId, status: { data: state } }));
}

abstract class aStatusProvider {
    protected state: Array<number> = [0, 0, 0, 0];
    public GetState(): Array<number> {
        return this.state;
    }
    abstract UpdateStateWithCommand(cmd: sensact.RequestCommand.Payload, sender: ISender): void;
    constructor(protected appId: ApplicationId) {}
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

    UpdateStateWithCommand(cmd: sensact.RequestCommand.Payload, sender: ISender): void {
        const p = cmd.payload.data;
        if (cmd.cmd == Command.SET_VERTICAL_TARGET) {
            this.state[1] = (p[0] << 8) + (p[1] << 0);
            this.state[0] = this.state[1] == 0 ? 0 : 1; // 1=ON, 0=OFF
        } else if (cmd.cmd == Command.TOGGLE) {
            this.state[0] = this.state[0] == 0 ? 1 : 0;
        }
        console.info(`SinglePwmTwin ${ApplicationId[this.appId]} got command ${Command[cmd.cmd]}. state[0]=${this.state[0]} state[1]=${this.state[1]}`);
        setTimeout(() => {
            console.log(`sendUpdateState ${this.appId}`)
            sendStatus(sender, this.appId, this.state);
        }, 500);
    }
    constructor(appId: ApplicationId) {
        super(appId);
    }
}

enum eCurrentBlindState {
    ENERGY_SAVE,
    STOP,
    PREPARE_UP,
    PREPARE_DOWN,
    UP,
    DOWN,
};

class BlindTwin extends aStatusProvider {

    private position: number = 50; // Simulated position of the blind (0-100%)
    private blindState: eCurrentBlindState = eCurrentBlindState.STOP;
    private sender: ISender | undefined = undefined;

    constructor(appId: ApplicationId) {
        super(appId);
        setInterval(() => {
            switch (this.blindState) {
                case eCurrentBlindState.UP:
                    this.position += 5; // Simulate moving up
                    if (this.position >= 100) {
                        this.blindState = eCurrentBlindState.STOP; // Stop when reaching the top
                        this.position = 100;
                    }
                    break;
                case eCurrentBlindState.DOWN:
                    this.position -= 5; // Simulate moving down
                    if (this.position < 0) {
                        this.blindState = eCurrentBlindState.STOP; // Stop when reaching the bottom
                        this.position = 0;
                    }
                    break;
                default:
                    break;
            }
            this.state[0] = this.blindState; // Update the blind state
            this.state[1] = this.position; // Update the position
            if (this.sender) {
                sendStatus(this.sender, this.appId, this.state);
            }
        }, 1000)
    }

    UpdateStateWithCommand(cmd: sensact.RequestCommand.Payload, sender: ISender): void {
        this.sender = sender;
        switch (cmd.cmd) {
            case Command.UP:
                this.blindState = eCurrentBlindState.UP;
                break
            case Command.DOWN:
                this.blindState = eCurrentBlindState.DOWN;
                break;
            default:
                this.blindState = eCurrentBlindState.STOP;
                break
        }
        console.info(`BlindTwin ${ApplicationId[this.appId]} got command ${Command[cmd.cmd]}. state[0]=${this.state[0]} state[1]=${this.state[1]}`);
    }
}

export class SensactHandler extends NamespaceAndHandler {
    private Map: Map<ApplicationId, aStatusProvider> = new Map<ApplicationId, aStatusProvider>();

    constructor() {
        super(sensact.NAMESPACE_ID);
        this.Map.set(ApplicationId.PWM___L0_PRTY_S, new SinglePwmTwin(ApplicationId.PWM___L0_PRTY_S));
        this.Map.set(ApplicationId.PWM___L1_BATH_S, new SinglePwmTwin(ApplicationId.PWM___L1_BATH_S));
        this.Map.set(ApplicationId.PWM___L2_BATH_S, new SinglePwmTwin(ApplicationId.PWM___L2_BATH_S));
        this.Map.set(ApplicationId.PWM___L3_WORK_S, new SinglePwmTwin(ApplicationId.PWM___L3_WORK_S));
        this.Map.set(ApplicationId.BLIND_L1_BATH_J1, new BlindTwin(ApplicationId.BLIND_L1_BATH_J1));
        this.Map.set(ApplicationId.BLIND_L2_BATH_J1, new BlindTwin(ApplicationId.BLIND_L2_BATH_J1));
        this.Map.set(ApplicationId.BLIND_L3_WORK_J1, new BlindTwin(ApplicationId.BLIND_L3_WORK_J1));
    }

    public Handle(messageTypeId: number, view: DataView, sender: ISender) {
        switch (messageTypeId) {
            case sensact.RequestStatus.TYPE_ID: {
                const req = sensact.RequestStatus.decode(view, 0);
                const ids = req.ids.map(v => v.value);
                console.info(`got RequestStatus for ids ${ids.join(",")}`);
                const states = ids.map(id => {
                    const payload = this.Map.get(id)?.GetState() ?? [0, 0, 0, 0];
                    console.info(`Sensact Status Request ${id} [${ApplicationId[id]}] ${payload[0]} ${payload[1]} ${payload[2]} ${payload[3]}`);
                    return { id, status: { data: payload } };
                });
                sender.send(sensact.ResponseStatus.encode({ requestId: req.requestId, states }));
                break;
            }
            case sensact.RequestCommand.TYPE_ID: {
                const req = sensact.RequestCommand.decode(view, 0);
                console.log(`Sensact Command ${Command[req.cmd]} to ${ApplicationId[req.id]}`)
                this.Map.get(req.id)?.UpdateStateWithCommand(req, sender);
                sender.send(sensact.ResponseCommand.encode({ requestId: req.requestId }));
                break;
            }
        }
    }
}
