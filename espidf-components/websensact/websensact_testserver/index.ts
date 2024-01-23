import * as http from "node:http";
import * as fs from "node:fs";
import { CommandMessage } from "../websensact_typescript/generated/websensact";
import { ApplicationId } from "../websensact_typescript/generated/application-id";
import { Command } from "../websensact_typescript/generated/command";
import * as flatbuffers from "flatbuffers";
import { WebSocketServer } from "ws";

const PORT = 3000;
const PORT_WS = 3001;
const BUNDLED_FILE = "../websensact_builder/dist_browser_bundeled/SNSCT_NODE_HS/websensact.html";

const wss = new WebSocketServer({ noServer: true });

wss.on('connection', (ws) => {
    ws.on('error', console.error);

    ws.on('message', (data) => {
        console.log('received: %s', data);
    });

    ws.send('something');
});

function process(res: http.ServerResponse<http.IncomingMessage>, local_path: string, buffer: Buffer) {
    let data = new Uint8Array(buffer);
    let buf = new flatbuffers.ByteBuffer(data);
    let m = CommandMessage.getRootAsCommandMessage(buf);
    console.log(`Received buffer length ${buffer.byteLength} and AppId is ${m.id()} and Command is ${Command[m.cmd()]}`);
    res.writeHead(200, { "access-control-allow-origin": "*" });
    res.write(`Received buffer length ${buffer.byteLength} and AppId is ${m.id()} and Command is ${Command[m.cmd()]}`);
    res.end();
}

let server = http.createServer((req, res) => {
    console.log(`Request received for '${req.url}'`);
    //var local_path = new URL(req.url).pathname;

    if (req.url == "/sensact") {
        const req_body_chunks: any[] = [];
        req.on("data", (chunk: any) => req_body_chunks.push(chunk));
        req.on("end", () => {
            process(res, req.url!, Buffer.concat(req_body_chunks));
        });

    } else if (req.url == "/") {
        fs.readFile(BUNDLED_FILE, (err, data) => {
            res.writeHead(200, { 'Content-Type': 'text/html' });
            res.end(data);
        });
    } else {
        res.writeHead(404);
        res.end("Not found");
        return;
    }
});

server.on('upgrade', (req, sock, head) => {
    if (req.url == '/log') {
        wss.handleUpgrade(req, sock, head, ws => wss.emit('connection', ws, req));
    } else {
        sock.destroy();
    }
});

var messageChanger=0;

server.listen(3000, () => {
    console.log(`Server is running on port ${PORT}`);
    const interval = setInterval(()=>{
        switch (messageChanger) {
            case 0:
                wss.clients.forEach(ws=>ws.send(`ICurrent Time is ${new Date().getTime()}`));
                break;
            case 1:
                wss.clients.forEach(ws=>ws.send(`WCurrent Time is ${new Date().getTime()}`));
                break;
            case 2:
                wss.clients.forEach(ws=>ws.send(`ECurrent Time is ${new Date().getTime()}`));
                break;        
            default:
                break;
        }
        messageChanger++;
        messageChanger%=3;

      }, 1000);
      
});