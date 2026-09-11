import { defineConfig} from 'vite'
// Geteiltes Plugin (s. npm-packages/@klaus-liebler/vite-firmware-single-file) -- frueher als
// vite-plugin-single-file-firmware-asset.ts/singlefile-minify.ts hier dupliziert (uebernommen aus
// factory_in_a_box), jetzt EIN gemeinsamer Code fuer factory_in_a_box/sensact/labathome.
import { singleFileFirmwareAssetPlugin } from "@klaus-liebler/vite-firmware-single-file"
import fs from "node:fs"
import path from "node:path"
import https from "node:https"
import * as weso from "ws"
import { visualizer } from 'rollup-plugin-visualizer'

const WEBSOCKET_HOST = "192.168.2.42";
const WEBSOCKET_USERNAME = "admin";
const WEBSOCKET_PASSWORD = "password";

// Browser and dev server run on a different origin than the microcontroller, so the
// real session cookie set by the device's /login endpoint never reaches it. Instead,
// the proxy itself logs in once and reuses the resulting session cookie for every
// upstream connection.
let cachedSessionCookie: string | null = null;

function loginToMicrocontroller(): Promise<string> {
  return new Promise((resolve, reject) => {
    const body = `username=${encodeURIComponent(WEBSOCKET_USERNAME)}&password=${encodeURIComponent(WEBSOCKET_PASSWORD)}`;
    const req = https.request(
      {
        host: WEBSOCKET_HOST,
        path: "/login",
        method: "POST",
        rejectUnauthorized: false,
        headers: {
          "Content-Type": "application/x-www-form-urlencoded",
          "Content-Length": Buffer.byteLength(body),
        },
      },
      (res) => {
        res.resume();
        const setCookie = res.headers["set-cookie"] ?? [];
        const sessionCookie = setCookie
          .map((c) => c.split(";")[0])
          .find((c) => c.startsWith("session="));
        if (sessionCookie) {
          resolve(sessionCookie);
        } else {
          reject(new Error(`Login at https://${WEBSOCKET_HOST}/login failed (HTTP ${res.statusCode})`));
        }
      }
    );
    req.on("error", reject);
    req.write(body);
    req.end();
  });
}

async function getSessionCookie(): Promise<string> {
  if (!cachedSessionCookie) {
    cachedSessionCookie = await loginToMicrocontroller();
    console.log("Logged in to microcontroller, obtained session cookie");
  }
  return cachedSessionCookie;
}

const devLoginCookiePlugin = (username:string) => {
  return {
    name: "dev-login-cookie",
    configureServer(server: any) {
      server.middlewares.use((req: any, res: any, next: any) => {
        if (req.url?.startsWith("/dev-login")) {
          res.setHeader("Set-Cookie", [
            `username=${username}; Path=/; Secure; SameSite=None`,
            "session=dev-session-123; Path=/; Secure; SameSite=None",
          ]);
          res.statusCode = 204;
          res.end();
          return;
        }
        next();
      });
    },
  };
};

const fileServerPlugin = (filesDir: string) => {
  return {
    name: "file-server",
    configureServer(server: any) {
      server.middlewares.use("/files", (req: any, res: any, _next: any) => {
        const filePath = path.join(filesDir, req.url.split('?')[0]);
        console.log(`Serving file request for: ${filePath}`);
        // Security: prevent directory traversal
        if (!path.resolve(filePath).startsWith(path.resolve(filesDir))) {
          res.statusCode = 403;
          res.end("Forbidden");
          return;
        }
        
        // Serve file if exists
        if (fs.existsSync(filePath) && fs.statSync(filePath).isFile()) {
          res.setHeader("Content-Type", "application/octet-stream");
          res.end(fs.readFileSync(filePath));
          return;
        }
        
        res.statusCode = 404;
        res.end("Not Found");
      });
    },
  };
};

const wsProxyPlugin = () => {
  return {
    name: "ws-proxy",
    configureServer(server: any) {
      const wss = new weso.WebSocketServer({ noServer: true });
      
      wss.on('connection', async (clientWs: weso.WebSocket) => {
        console.log('Client WebSocket connected to proxy');

        let sessionCookie: string;
        try {
          sessionCookie = await getSessionCookie();
        } catch (error) {
          console.error('Login to microcontroller failed:', error);
          clientWs.close();
          return;
        }

        // Create connection to upstream server without TLS verification
        const upstreamWs = new weso.WebSocket(`wss://${WEBSOCKET_HOST}/webmanager_ws`, {
          rejectUnauthorized: false,
          perMessageDeflate: false,
          headers: { Cookie: sessionCookie },
        } as any);

        upstreamWs.on('unexpected-response', (_req: any, res: any) => {
          console.error(`Upstream handshake rejected: HTTP ${res.statusCode}`);
          if (res.statusCode === 401) {
            cachedSessionCookie = null; // force re-login on next connection attempt
          }
          clientWs.close();
        });

        upstreamWs.on('open', () => {
          console.log('Connected to upstream WebSocket server');
          
          // Forward client -> upstream
          clientWs.on('message', (data: any) => {
            if (upstreamWs.readyState === weso.WebSocket.OPEN) {
              upstreamWs.send(data);
            }
          });
          
          // Forward upstream -> client
          upstreamWs.on('message', (data: any) => {
            if (clientWs.readyState === weso.WebSocket.OPEN) {
              clientWs.send(data);
            }
          });
          
          // Handle errors
          upstreamWs.on('error', (error: any) => {
            console.error('Upstream WebSocket error:', error);
            clientWs.close();
          });
          
          clientWs.on('error', (error: any) => {
            console.error('Client WebSocket error:', error);
            upstreamWs.close();
          });
          
          // Handle close
          upstreamWs.on('close', () => {
            console.log('Upstream WebSocket closed');
            clientWs.close();
          });
          
          clientWs.on('close', () => {
            console.log('Client WebSocket closed');
            upstreamWs.close();
          });
        });
        
        upstreamWs.on('error', (error: any) => {
          console.error('WebSocket connection error:', error);
          clientWs.close();
        });
      });
      
      server.httpServer?.on('upgrade', (req: any, socket: any, head: any) => {
        if (req.url === '/webmanager_ws') {
          wss.handleUpgrade(req, socket, head, (ws: any) => {
            wss.emit('connection', ws, req);
          });
        }
      });
    },
  };
};



// https://vitejs.dev/config/
// @ts-ignore
export default defineConfig(({ command, mode, isSsrBuild, isPreview }) => {
  const isAnalyze = mode === 'analyze';
  
  return {
    resolve: {
      dedupe: ['flatbuffers', 'lit-html'],
    },
    plugins: [
      isAnalyze && visualizer({
        filename: './dist/stats.html',
        open: true,
        gzipSize: true,
        brotliSize: true,
        template: 'treemap',
      }),
      !isAnalyze && singleFileFirmwareAssetPlugin("index.compressed.br"),
      devLoginCookiePlugin("devuser"),
      fileServerPlugin(path.join(process.cwd(),"..", "testserver", "files")),
      wsProxyPlugin(),
    ].filter(Boolean),
    build: {
      //minify: false,
      cssCodeSplit: false,
    },
    esbuild: {
      //drop:["console", 'debugger'],
      legalComments: 'none',

    },
    server: {
      host: 'localhost',  // oder '0.0.0.0' für Netzwerkzugriff
      port: 5173,
      hmr: {
        host: 'localhost',  // Wichtig: explizit HMR-Host setzen
        port: 5173,
        protocol: 'wss'     // oder 'wss' wenn HTTPS
      },
      open: "https://protzklotz:5173",
      https: {
        key: fs.readFileSync(path.join(process.env.USERPROFILE!, "OneDrive - HSOS", "certificates", "testserver.pem.key")),
        cert: fs.readFileSync(path.join(process.env.USERPROFILE!, "OneDrive - HSOS", "certificates", "testserver.pem.crt")),
      },

      proxy: {
        "/labathome": {
          target: "https://localhost:3001",
        }
      }
    }
  }
})
