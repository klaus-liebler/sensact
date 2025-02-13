
import * as flatbuffers from "flatbuffers"
import {ApplicationId, Command, Namespace, RequestCommand, RequestStatus, RequestWrapper, Requests, ResponseCommand, ResponseStatus, ResponseStatusItem, ResponseWrapper, Responses} from "@generated/flatbuffers_ts/sensact"
import { ISender, NamespaceAndHandler } from "@klaus-liebler/websocket_file_testserver/utils"

export class SensactHandler extends NamespaceAndHandler {

    public Handle(buffer: flatbuffers.ByteBuffer, sender: ISender) {
        let b = new flatbuffers.Builder(1024);
        var rw=RequestWrapper.getRootAsRequestWrapper(buffer)
        switch(rw.requestType()){
            case Requests.NONE:return;
            case Requests.RequestStatus:{
                const rs=rw.request(new RequestStatus()) as RequestStatus;
                ResponseStatus.startStatesVector(b, rs.idsLength())
                for(var id of rs.idsArray()!){
                    
                    ResponseStatusItem.createResponseStatusItem(b, id, 0) 
                }
                let vectOffset= b.endVector()
                
                b.finish(
                    ResponseWrapper.createResponseWrapper(
                        b,
                        Responses.ResponseStatus, 
                        vectOffset
                    )
                );
                sender.send(Namespace.Value, b);
                break;
            }
            case Requests.RequestCommand:{
                const rs=rw.request(new RequestCommand()) as RequestCommand;
                console.log(`Sensact Command ${Command[rs.cmd()]} to ${ApplicationId[rs.id()]}`)
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
    constructor() {
        super(Namespace.Value)
        
    }
   

}

