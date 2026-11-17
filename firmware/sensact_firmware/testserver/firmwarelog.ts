// Simuliert das FirmwareLogPlugin des Webmanagers (Namespace 'firmwarelog'): ein Puffer mit
// Beispielzeilen, alle 2 s eine neue; nach RequestLogHistory erst der Puffer, dann jede neue Zeile.
import { ISender, NamespaceAndHandler } from "@klaus-liebler/websocket_file_testserver/utils"
import { firmwarelog } from "@generated/wsprotocol_ts/ws-protocol";

const CAPACITY = 48;
const SAMPLES: Array<[firmwarelog.LogLevel, string, string]> = [
    [firmwarelog.LogLevel.Info, "MAIN", "SNSCT_NODE_SIDEDOOR is booting up"],
    [firmwarelog.LogLevel.Info, "WEBMANAGER", "Websocket connected, fd 54"],
    [firmwarelog.LogLevel.Debug, "CAN", "RX id=0x123 len=8"],
    [firmwarelog.LogLevel.Warn, "BLIND", "BLD_L1_BATH_1: no end position after 60 s"],
    [firmwarelog.LogLevel.Info, "PWM", "PWM___L0_TECH_S1 -> 42 %"],
    [firmwarelog.LogLevel.Error, "I2C", "PCA9685 at 0x40 does not answer (ESP_ERR_TIMEOUT)"],
    [firmwarelog.LogLevel.Verbose, "SENSACT", "loop took 3 ms"],
];

export class FirmwareLogHandler extends NamespaceAndHandler {
    private readonly ring: firmwarelog.NotifyLogMessage.Payload[] = [];
    private next = 0;
    private streaming = false;
    private readonly start = Date.now();

    constructor() {
        super(firmwarelog.NAMESPACE_ID);
        for (let i = 0; i < 10; i++) this.add();
        setInterval(() => {
            const line = this.add();
            if (this.streaming && this.sender) this.sender.send(firmwarelog.NotifyLogMessage.encode(line));
        }, 2000);
    }

    private add(): firmwarelog.NotifyLogMessage.Payload {
        const [level, tag, text] = SAMPLES[this.next % SAMPLES.length];
        const line = { sequence: this.next++, level, uptimeMs: Date.now() - this.start, tag, text };
        this.ring.push(line);
        if (this.ring.length > CAPACITY) this.ring.shift();
        return line;
    }

    public Handle(messageTypeId: number, view: DataView, sender: ISender) {
        this.SetSender(sender);
        switch (messageTypeId) {
            case firmwarelog.RequestLogHistory.TYPE_ID:
                sender.respond(firmwarelog.ResponseLogHistory.encode, { oldestSequence: this.ring[0]?.sequence ?? this.next, nextSequence: this.next });
                for (const line of this.ring) sender.send(firmwarelog.NotifyLogMessage.encode(line));
                this.streaming = true;
                break;
            case firmwarelog.RequestSetLogLevel.TYPE_ID: {
                const req = firmwarelog.RequestSetLogLevel.decode(view, 0);
                console.info(`firmwarelog: esp_log_level_set("${req.tag || "*"}", ${req.level})`);
                sender.respond(firmwarelog.ResponseSetLogLevel.encode, {});
                break;
            }
        }
    }
}
