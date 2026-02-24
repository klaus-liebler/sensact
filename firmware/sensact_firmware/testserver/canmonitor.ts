
import * as flatbuffers from "flatbuffers"

import { ISender, NamespaceAndHandler } from "@klaus-liebler/websocket_file_testserver/utils"
import { NotifyCanMessage, CanData, Namespace} from "@generated/flatbuffers_ts/Canmonitor";






export class CanmonitorHandler extends NamespaceAndHandler {
    
    constructor() {
        super(Namespace.Value);
                setInterval(() => {
                    console.info(`Log a Can Message`);
                    if(!this.sender) return;
                    let b = new flatbuffers.Builder(1024);
                    const dataVector  =b.createByteVector(new Uint8Array([0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88]));
                    NotifyCanMessage.startNotifyCanMessage(b);
                    NotifyCanMessage.addMessageId(b, 0x123);
                    const dataOffset=CanData.createCanData(b, [0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88])
                    NotifyCanMessage.addData(b, dataOffset);
                    NotifyCanMessage.addDataLen(b, 8);
                    b.finish(NotifyCanMessage.endNotifyCanMessage(b));
                    this.sender.send(Namespace.Value, b)
                    console.info(`Log a Can Message..success`);
                    
                }, 3000)
    }

    public Handle(buffer: flatbuffers.ByteBuffer, sender: ISender) {
        this.SetSender(sender);
        console.log("CanmonitorHandler received data, but no requests are defined yet.");
        
    }
}

