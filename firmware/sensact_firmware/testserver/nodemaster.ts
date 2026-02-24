
import * as flatbuffers from "flatbuffers";
import { ISender, NamespaceAndHandler } from "@klaus-liebler/websocket_file_testserver/utils";
import { Namespace, Requests, Responses, RequestWrapper, ResponseWrapper, RequestGetInput, ResponseInputs, ResponseOutputs, RequestSetOutput, ResponseSetOutput, ResponseGetInput, Range } from "@generated/flatbuffers_ts/nodemaster";

const UINT16_MAX = 0xFFFF;

export class NodemasterHandler extends NamespaceAndHandler {
    private inputs: Uint16Array;
    private outputs: Uint16Array;
    private inputRanges: Array<{start: number, end: number, name: string}>;
    private outputRanges: Array<{start: number, end: number, name: string}>;

    constructor() {
        super(Namespace.Value);
        // Simulate 32 inputs/outputs across 2 groups of 16 each
        this.inputs = new Uint16Array(32);
        this.outputs = new Uint16Array(32);
        for (let i = 0; i < this.inputs.length; i++) {
            this.inputs[i] = 0;
            this.outputs[i] = 0;
        }
        // Define valid ranges: two groups of 16 each
        this.inputRanges = [
            {start: 0, end: 16, name: "Input Group 0"},
            {start: 16, end: 32, name: "Input Group 1"}
        ];
        this.outputRanges = [
            {start: 0, end: 16, name: "Output Group 0"},
            {start: 16, end: 32, name: "Output Group 1"}
        ];
    }

    private sendResponseInputs(sender: ISender) {
        const b = new flatbuffers.Builder(1024);
        const rangeOffsets = this.inputRanges.map(({start, end, name}) => {
            const nameOffset = b.createString(name);
            return Range.createRange(b, start, end, nameOffset);
        });
        const rangesVec = ResponseInputs.createRangesVector(b, rangeOffsets);
        const resp = ResponseInputs.createResponseInputs(b, rangesVec);
        b.finish(ResponseWrapper.createResponseWrapper(b, Responses.ResponseInputs, resp));
        sender.send(Namespace.Value, b);
    }

    private sendResponseOutputs(sender: ISender) {
        const b = new flatbuffers.Builder(1024);
        const rangeOffsets = this.outputRanges.map(({start, end, name}) => {
            const nameOffset = b.createString(name);
            return Range.createRange(b, start, end, nameOffset);
        });
        const rangesVec = ResponseOutputs.createRangesVector(b, rangeOffsets);
        const resp = ResponseOutputs.createResponseOutputs(b, rangesVec);
        b.finish(ResponseWrapper.createResponseWrapper(b, Responses.ResponseOutputs, resp));
        sender.send(Namespace.Value, b);
    }

    private sendResponseSetOutput(sender: ISender, index: number, value: number, success: boolean) {
        const b = new flatbuffers.Builder(256);
        const resp = ResponseSetOutput.createResponseSetOutput(b, success, index, value);
        b.finish(ResponseWrapper.createResponseWrapper(b, Responses.ResponseSetOutput, resp));
        sender.send(Namespace.Value, b);
    }


    public Handle(buffer: flatbuffers.ByteBuffer, sender: ISender) {
        this.SetSender(sender);
        const rw = RequestWrapper.getRootAsRequestWrapper(buffer);
        switch (rw.requestType()) {
            case Requests.RequestInputs: {
                console.info(`Nodemaster: RequestInputs -> ranges=${JSON.stringify(this.inputRanges)}`);
                this.sendResponseInputs(sender);
                break;
            }
            case Requests.RequestOutputs: {
                console.info(`Nodemaster: RequestOutputs -> ranges=${JSON.stringify(this.outputRanges)}`);
                this.sendResponseOutputs(sender);
                break;
            }
            case Requests.RequestSetOutput: {
                const r = rw.request(new RequestSetOutput());
                const idx = r.index();
                const val = r.value();
                let ok = false;
                if (idx >= 0 && idx < this.outputs.length) {
                    this.outputs[idx] = val;
                    // Loopback: mirror outputs to inputs
                    if (idx < this.inputs.length) {
                        this.inputs[idx] = val;
                    }
                    ok = true;
                }
                console.info(`Nodemaster: RequestSetOutput idx=${idx} value=${val} ok=${ok}`);
                this.sendResponseSetOutput(sender, idx, val, ok);
                break;
            }
            case Requests.RequestGetInput: {
                const req = rw.request(new RequestGetInput());
                const index = req.index();
    
                var randomValue = Math.floor(Math.random() * 2 * UINT16_MAX); // Simulate a random input index
                if(randomValue > UINT16_MAX) {
                    randomValue=0; //50% chance to return 0 to simulate inactive input
                }
                this.inputs[index] = randomValue;
                console.info(`Nodemaster: RequestGetInput -> returning value=${randomValue} from index=${index}`);
                const b = new flatbuffers.Builder(256);
                const resp = ResponseGetInput.createResponseGetInput(b, index, randomValue);
                b.finish(ResponseWrapper.createResponseWrapper(b, Responses.ResponseGetInput, resp));
                sender.send(Namespace.Value, b);
                break;
            }
            default:
                // Ignore unknowns
                break;
        }
    }
}
