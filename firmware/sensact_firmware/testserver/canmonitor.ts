
import { ISender, NamespaceAndHandler } from "@klaus-liebler/websocket_file_testserver/utils"
import { canmonitor } from "@generated/wsprotocol_ts/ws-protocol";

export class CanmonitorHandler extends NamespaceAndHandler {

    constructor() {
        super(canmonitor.NAMESPACE_ID);
        setInterval(() => {
            if (!this.sender) return;
            this.sender.send(canmonitor.NotifyCanMessage.encode({
                messageId: 0x123,
                data: { data: [0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88] },
                dataLen: 8,
            }));
            console.info(`Log a Can Message..success`);
        }, 3000)
    }

    public Handle(messageTypeId: number, view: DataView, sender: ISender) {
        this.SetSender(sender);
        console.log("CanmonitorHandler received data, but no requests are defined yet.");
    }
}
