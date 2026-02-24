import * as utils from "@klaus-liebler/websocket_file_testserver/utils"

import { SystemHandler } from "@klaus-liebler/websocket_file_testserver/system"
import { WebmanagerHandler } from "@klaus-liebler/websocket_file_testserver/webmanager"
import { JournalHandler } from "@klaus-liebler/websocket_file_testserver/journal"
import { FingerprintHandler } from "@klaus-liebler/websocket_file_testserver/fingerprint"
import { SensactHandler } from "./sensact"
import { CanmonitorHandler } from "./canmonitor"
import { NodemasterHandler } from "./nodemaster"
import path from "node:path"

utils.StartServers(path.join(process.env.USERPROFILE, "OneDrive - HSOS", "certificates"), [
    new SystemHandler(),
    new WebmanagerHandler(),
    new JournalHandler(),
    new SensactHandler(),
    new FingerprintHandler(),
    new CanmonitorHandler(),
    new NodemasterHandler()
])