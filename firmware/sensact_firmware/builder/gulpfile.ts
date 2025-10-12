import * as gulp from "gulp";
import fs from "node:fs";
import path from "node:path";
import * as cert from "@klaus-liebler/espidf-vite-secure-build-tools/certificates";
import * as P from "@klaus-liebler/espidf-vite-secure-build-tools/paths";
import * as idf from "@klaus-liebler/espidf-vite-secure-build-tools/espidf";
import { getLastCommit } from "@klaus-liebler/espidf-vite-secure-build-tools/git";
import * as ascii_art from "@klaus-liebler/espidf-vite-secure-build-tools/ascii_art";
import { flatbuffers_generate_c, flatbuffers_generate_ts } from "@klaus-liebler/espidf-vite-secure-build-tools/flatbuffers";
import {Context, ContextConfig} from "@klaus-liebler/espidf-vite-secure-build-tools/context"
import {cleanNpmExcept_PackageJson_node_modules } from "@klaus-liebler/espidf-vite-secure-build-tools/utils";
import * as vite_helper from "@klaus-liebler/espidf-vite-secure-build-tools/vite_helper";
import { eEncryptionMode, MyFavouriteDateTimeFormat } from "@klaus-liebler/commons";
import * as cfg from "@klaus-liebler/espidf-vite-secure-build-tools/key_value_file_helper"
import * as sensact from "./sensact_code_generator";

//Reset
const OVERWRITE_NVS_TO_DELETE_WIFI_SETTINGS_AND_ALL_OTHER_SETTINGS=false;

//Default Board Type
export const DEFAULT_BOARD_NAME="SENSACT_UP_HS"
export const DEFAULT_BOARD_VERSION=0

//Sensact specific config
const APPLICATION_NAME = "sensact"
const APPLICATION_VERSION = "1.0"

//Security
export const DEFAULT_ENCRYPTION_MODE = eEncryptionMode.NON_ENCRYPTED as eEncryptionMode
export const FLASH_ENCYRPTION_STRENGTH=idf.EncryptionStrength.AES256

//Paths
export const IDF_PATH=globalThis.process.env.IDF_PATH as string;
export const USERPROFILE =globalThis.process.env.USERPROFILE as string;
const IDF_PROJECT_ROOT = "C:\\repos\\sensact\\firmware\\sensact_firmware"
const IDF_COMPONENT_WEBMANAGER_ROOT = "C:\\repos\\espidf-component-webmanager"
const GENERATED_ROOT = "c:\\repos\\generated"
const SENSACT_COMPONENT_GENERATED_PATH = "C:\\repos\\generated\\sensact_model"
//hier werden alle board-spezifischen Daten abgelegt, z.B. Zertifikate, Sounds, Usersettings
const BOARDS_BASE_DIR= path.join(USERPROFILE, "OneDrive - HSOS\\esp32_boards")
//hier wird der RootCA und die Testzertifikate abgelegt
const CERTIFICATES = path.join(USERPROFILE, "OneDrive - HSOS\\certificates")



const contextConfig = new ContextConfig(GENERATED_ROOT, IDF_PROJECT_ROOT, BOARDS_BASE_DIR, DEFAULT_BOARD_NAME, DEFAULT_BOARD_VERSION, DEFAULT_ENCRYPTION_MODE);

export const buildWebForCurrent = gulp.series(
  createFiles,
  buildAndCompressWebProject,
)

export const buildForCurrent = gulp.series(
  createFiles,
  buildAndCompressWebProject,
  buildFirmware,
)

export default gulp.series(
  addOrUpdateConnectedBoard,
  buildForCurrent,
  encryptFirmwareIfNecessary,
  flashFirmware,
)

export async function addOrUpdateConnectedBoard(_cb: gulp.TaskFunctionCallback){
  return Context.get(contextConfig, true);
}

export async function info(cb: gulp.TaskFunctionCallback){
  await Context.printInfo(contextConfig);
  return cb();
}

async function buildFirmware(_cb: gulp.TaskFunctionCallback) {
  const c=await Context.get(contextConfig)
  return idf.buildFirmware(c);
}

async function encryptFirmwareIfNecessary(cb: gulp.TaskFunctionCallback) {
  const c=await Context.get(contextConfig);
  if(c.b.flash_encryption_key_burned_and_activated || c.c.defaultEncryptionMode===eEncryptionMode.ENCRYPTED) {
    return idf.encryptPartitions_Bootloader_App_PartitionTable_OtaData(c);
  }else{
    return cb();
  }
} 

async function flashFirmware(cb: gulp.TaskFunctionCallback){
  const c = await Context.get(contextConfig)
  if(c.b.flash_encryption_key_burned_and_activated || c.c.defaultEncryptionMode===eEncryptionMode.ENCRYPTED){
    await idf.burnFlashEncryptionKeyAndActivateEncryptedFlash(c, FLASH_ENCYRPTION_STRENGTH)
    return idf.flashEncryptedFirmware(c, OVERWRITE_NVS_TO_DELETE_WIFI_SETTINGS_AND_ALL_OTHER_SETTINGS, true, false);
  }else{
    return idf.flashFirmware(c, OVERWRITE_NVS_TO_DELETE_WIFI_SETTINGS_AND_ALL_OTHER_SETTINGS, true);
  }
}


export async function createFiles(cb: gulp.TaskFunctionCallback) {
  const c= await Context.get(contextConfig)

  //Sensact
  var s= new sensact.Sensact(c, SENSACT_COMPONENT_GENERATED_PATH)
  s.PrepareSensactFiles()

  //Flatbuffers
  fs.rmSync(c.p.GENERATED_FLATBUFFERS_CPP, {recursive:true, force:true})
  cleanNpmExcept_PackageJson_node_modules(c.p.GENERATED_FLATBUFFERS_TS);
  await flatbuffers_generate_c([path.join(IDF_COMPONENT_WEBMANAGER_ROOT, "flatbuffers"), c.p.P_FLATBUFFERS], c.p.GENERATED_FLATBUFFERS_CPP);
  await flatbuffers_generate_ts([path.join(IDF_COMPONENT_WEBMANAGER_ROOT, "flatbuffers"), c.p.P_FLATBUFFERS], c.p.GENERATED_FLATBUFFERS_TS);

  //Certificates (lazy)
  if (!c.p.existsBoardSpecificPath(P.CERTIFICATES_SUBDIR, P.ESP32_CERT_PEM_PRVTKEY_FILE)
    || !c.p.existsBoardSpecificPath(P.CERTIFICATES_SUBDIR, P.ESP32_CERT_PEM_CRT_FILE)) {
    const hostname = s.GetNodeId();
    let esp32Cert = cert.CreateAndSignCert(hostname, "192.168.4.1", [hostname, hostname+".local", hostname+".fritz.box"], path.join(CERTIFICATES, P.ROOT_CA_PEM_CRT_FILE), path.join(CERTIFICATES, P.ROOT_CA_PEM_PRVTKEY_FILE));
    c.p.writeBoardSpecificFileCreateDirLazy(P.CERTIFICATES_SUBDIR, P.ESP32_CERT_PEM_PRVTKEY_FILE, esp32Cert.privateKey);
    c.p.writeBoardSpecificFileCreateDirLazy(P.CERTIFICATES_SUBDIR, P.ESP32_CERT_PEM_CRT_FILE, esp32Cert.certificate);
    }
  
  //Flash Encryption Key
  idf.createRandomFlashEncryptionKeyLazily(await Context.get(contextConfig), FLASH_ENCYRPTION_STRENGTH);

  //Config files
  const defs = await createObjectWithDefines(c);
  cfg.createCMakeJsonConfigFile(c, defs);
  cfg.createCppConfigurationHeader(c, defs);
  cfg.createTypeScriptRuntimeConfigProject(c, defs);
  cb();
}

async function createObjectWithDefines(c:Context) {
  var defines: Record<string, string|Array<string>|number> = {};
  
  var s= new sensact.Sensact(c, SENSACT_COMPONENT_GENERATED_PATH)
  defines=s.AddSensactNodeAndModelDescriptorToDefines(defines);
  
  for (const [k, v] of Object.entries(c.b.board_settings?.web ?? {})) {
    defines[k] = JSON.stringify(v);;
  }
  
  for (const [k, v] of Object.entries(c.b.board_settings?.firmware ?? {})) {
    defines[k] = JSON.stringify(v);;
  }

  const now = new Date();
  defines.NODE_ID=s.GetNodeId();//Specific for Sensact
  defines.HOSTNAME=s.GetNodeId();
  defines.BOARD_NAME = c.b.board_name;
  defines.BOARD_VERSION = c.b.board_version;
  defines.BOARD_ROLES=c.b.board_roles;
  defines.BOARD_MAC = c.b.mac;
  defines.BOARD_DIRECTORY = c.p.boardSpecificPath();
  defines.APP_NAME = APPLICATION_NAME;
  defines.APP_VERSION = APPLICATION_VERSION;
  defines.CREATION_DT = Math.floor(now.valueOf() / 1000);
  defines.CREATION_DT_STR = now.toLocaleString("de-DE", MyFavouriteDateTimeFormat)
  defines.GIT_SHORT_HASH = (await getLastCommit(true)).shortHash;
  defines.BANNER = ascii_art.createAsciiArt(`${APPLICATION_NAME} ${s.GetNodeId()}`);
  return defines;
}

export async function buildAndCompressWebProject(cb: gulp.TaskFunctionCallback) {
  const c = await Context.get(contextConfig);
  await vite_helper.buildAndCompressWebProject(path.join(c.c.idfProjectDirectory, "web"), c.p.GENERATED_WEB);
  cb()
}







