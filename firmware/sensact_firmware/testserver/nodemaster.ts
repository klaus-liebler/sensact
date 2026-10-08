
import { ISender, NamespaceAndHandler } from "@klaus-liebler/websocket_file_testserver/utils"
import { nodemaster } from "@generated/wsprotocol_ts/ws-protocol";

const UINT16_MAX = 0xFFFF;

type Range = { start: number, end: number, name: string };

export class NodemasterHandler extends NamespaceAndHandler {
    // Simuliert einen I2CBusmaster an Busmaster-Index 1 (Bits 15..14 = 01): ein konfigurierter und ein
    // per Discovery gefundener Chip je Typ, Namensschema wie in der Firmware (busmaster.hh).
    private static readonly BUSMASTER_BASE = 1 << 14;
    private inputRanges: Range[] = [
        { start: NodemasterHandler.BUSMASTER_BASE + 0, end: NodemasterHandler.BUSMASTER_BASE + 16, name: "pca9555@0x20" },
        { start: NodemasterHandler.BUSMASTER_BASE + 16, end: NodemasterHandler.BUSMASTER_BASE + 32, name: "pca9555@0x21 (discovered)" },
    ];
    private outputRanges: Range[] = [
        { start: NodemasterHandler.BUSMASTER_BASE + 0, end: NodemasterHandler.BUSMASTER_BASE + 16, name: "pca9685@0x41" },
        { start: NodemasterHandler.BUSMASTER_BASE + 16, end: NodemasterHandler.BUSMASTER_BASE + 32, name: "pca9685@0x42 (discovered)" },
    ];
    private outputs = new Map<number, number>();
    private manualMode = false;

    constructor() {
        super(nodemaster.NAMESPACE_ID);
    }

    private static toWire(ranges: Range[]) {
        return ranges.map(r => ({ classId: nodemaster.Range.CLASS_ID, ...r } as const));
    }

    public Handle(messageTypeId: number, view: DataView, sender: ISender) {
        this.SetSender(sender);
        switch (messageTypeId) {
            case nodemaster.RequestInputs.TYPE_ID: {
                const req = nodemaster.RequestInputs.decode(view, 0);
                console.info(`Nodemaster: RequestInputs -> ranges=${JSON.stringify(this.inputRanges)}`);
                sender.send(nodemaster.ResponseInputs.encode({ requestId: req.requestId, ranges: NodemasterHandler.toWire(this.inputRanges) }));
                break;
            }
            case nodemaster.RequestOutputs.TYPE_ID: {
                const req = nodemaster.RequestOutputs.decode(view, 0);
                console.info(`Nodemaster: RequestOutputs -> ranges=${JSON.stringify(this.outputRanges)}`);
                sender.send(nodemaster.ResponseOutputs.encode({ requestId: req.requestId, ranges: NodemasterHandler.toWire(this.outputRanges) }));
                break;
            }
            case nodemaster.RequestSetOutput.TYPE_ID: {
                const req = nodemaster.RequestSetOutput.decode(view, 0);
                const ok = this.outputRanges.some(r => req.index >= r.start && req.index < r.end);
                if (ok) this.outputs.set(req.index, req.value);
                console.info(`Nodemaster: RequestSetOutput idx=${req.index} value=${req.value} ok=${ok} (manualMode=${this.manualMode})`);
                sender.send(nodemaster.ResponseSetOutput.encode({ requestId: req.requestId, success: ok, index: req.index, value: req.value }));
                break;
            }
            case nodemaster.RequestGetInput.TYPE_ID: {
                const req = nodemaster.RequestGetInput.decode(view, 0);
                // Zufallswert, in 50% der Faelle 0 (inaktiver Eingang)
                const value = Math.random() < 0.5 ? 0 : Math.floor(Math.random() * UINT16_MAX);
                console.info(`Nodemaster: RequestGetInput -> returning value=${value} from index=${req.index}`);
                sender.send(nodemaster.ResponseGetInput.encode({ requestId: req.requestId, index: req.index, value }));
                break;
            }
            case nodemaster.RequestSetMode.TYPE_ID: {
                const req = nodemaster.RequestSetMode.decode(view, 0);
                if (!req.queryOnly) this.manualMode = req.manual;
                console.info(`Nodemaster: RequestSetMode manual=${req.manual} queryOnly=${req.queryOnly} -> manualMode=${this.manualMode}`);
                sender.send(nodemaster.ResponseMode.encode({ requestId: req.requestId, manual: this.manualMode }));
                break;
            }
        }
    }
}
