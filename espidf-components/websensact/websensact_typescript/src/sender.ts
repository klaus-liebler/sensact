import { CommandMessage } from "../generated/websensact";
import { ApplicationId } from "../generated/application-id";
import { Command } from "../generated/command";
import * as flatbuffers from 'flatbuffers';

//const URL_PREFIX = "http://192.168.2.124";
const URL_PREFIX = "";
//const URL_PREFIX = "http://127.0.0.1:3000";
const SENSACT_URL = URL_PREFIX + "/sensact";
const INFO_URL = URL_PREFIX + "/systeminfo";
const MESSAGELOG_URL = URL_PREFIX + "/messagelog";
const RESTART_URL = URL_PREFIX + "/restart";

const GET_TEXT: RequestInit = { method: "GET", mode: "no-cors"};


export async function sendCommandMessage(id: ApplicationId, cmd: Command, payload: Uint8Array) {
    let b = new flatbuffers.Builder(1024);
    let payloadOffset = CommandMessage.createPayloadVector(b, payload);
    CommandMessage.startCommandMessage(b);
    CommandMessage.addId(b, id);
    CommandMessage.addCmd(b, cmd);
    CommandMessage.addPayload(b, payloadOffset);
    let x = CommandMessage.endCommandMessage(b);
    b.finish(x);
    let buf = b.asUint8Array();
    let post_binary: RequestInit = { method: "POST", headers: { "Content-Type": "application/octet-stream" }, body:buf};
    const response= await fetch(SENSACT_URL, post_binary);
    if (!response.ok) {
        throw new Error(`HTTP error! Status: ${response.status}`);
    }
    const txt = await response.text();
    console.log(txt);
}