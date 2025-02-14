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
import {mac_12char, mac_6char, writeFileCreateDirLazy } from "@klaus-liebler/espidf-vite-secure-build-tools/utils";
import * as vite_helper from "@klaus-liebler/espidf-vite-secure-build-tools/vite_helper";
import { MyFavouriteDateTimeFormat, StringBuilderImpl, strInterpolator } from "@klaus-liebler/commons";
import * as sensact from "./sensact_code_generator";
//Default Board Type

export const DEFAULT_BOARD_NAME="SENSACT_OUTDOOR"
export const DEFAULT_BOARD_VERSION=0


//Paths
export const IDF_PATH=globalThis.process.env.IDF_PATH as string;
export const USERPROFILE =globalThis.process.env.USERPROFILE as string;

//Config
const IDF_PROJECT_ROOT = `C:\\repos\\sensact\\firmware\\sensact_firmware`
const IDF_COMPONENT_WEBMANAGER_ROOT = "C:\\repos\\espidf-component-webmanager";
const GENERATED_ROOT = "c:\\repos\\generated";
const SENSACT_COMPONENT_GENERATED_PATH = "C:\\repos\\generated\\sensact_model";


const BOARDS_BASE_DIR= path.join(USERPROFILE, "netcase/esp32_boards");
const CERTIFICATES = path.join(USERPROFILE, "netcase/certificates");


const HOSTNAME_TEMPLATE = "sensact_${mac_6char}"
const APPLICATION_NAME = "sensact"
const APPLICATION_VERSION = "1.0"

const contextConfig = new ContextConfig(GENERATED_ROOT, IDF_PROJECT_ROOT, BOARDS_BASE_DIR, DEFAULT_BOARD_NAME, DEFAULT_BOARD_VERSION);


export const buildForCurrent = gulp.series(
  prepare_board_specific_files,
  compileAndDistributeFlatbuffers,
  buildAndCompressWebProject,
  createBoardCertificatesLazily,
  //createRandomFlashEncryptionKeyLazily,
  createCppConfigurationHeader,
  copyMostRecentlyConnectedBoardFilesToCurrent,
  buildFirmware,
  //encryptFirmware,
)

export default gulp.series(
  addOrUpdateConnectedBoard,
  buildForCurrent,
  //flashEncryptedFirmware,
  flashFirmware
)

async function createRandomFlashEncryptionKeyLazily(cb: gulp.TaskFunctionCallback) {
  await idf.createRandomFlashEncryptionKeyLazily(await Context.get(contextConfig));
  cb();
}

async function buildFirmware(cb: gulp.TaskFunctionCallback) {
  const c=await Context.get(contextConfig)
  const p = new P.Paths(c)
  await idf.buildFirmware(c);
  //await idf.nvs_partition_gen_encrypt(c, ()=>true)

  cb();
} 

async function encryptFirmware(cb: gulp.TaskFunctionCallback){
  await idf.encryptPartitions_Bootloader_App_PartitionTable_OtaData(await Context.get(contextConfig));
  cb();
}

async function flashEncryptedFirmware(cb: gulp.TaskFunctionCallback){
  await idf.flashEncryptedFirmware(await Context.get(contextConfig), false, false);
  cb();
}

async function flashFirmware(cb: gulp.TaskFunctionCallback){
  await idf.flashFirmware(await Context.get(contextConfig), true, true);
  cb();
}

export async function addOrUpdateConnectedBoard(cb: gulp.TaskFunctionCallback){
  await Context.get(contextConfig, true);
  return cb();
}

export async function prepare_board_specific_files(cb: gulp.TaskFunctionCallback){
  const c = await Context.get(contextConfig);
  var s= new sensact.Sensact(c, SENSACT_COMPONENT_GENERATED_PATH)
  s.PrepareSensactFiles()

  return cb()
}

export async function compileAndDistributeFlatbuffers(cb: gulp.TaskFunctionCallback) {
  const c= await Context.get(contextConfig)
  const pa = new P.Paths(c);
  fs.rmSync(pa.GENERATED_FLATBUFFERS_CPP, {recursive:true, force:true})
  fs.rmSync(pa.GENERATED_FLATBUFFERS_TS, {recursive:true, force:true})
  console.info(`${pa.GENERATED_FLATBUFFERS_FBS}`)
  await flatbuffers_generate_c(
    [path.join(IDF_COMPONENT_WEBMANAGER_ROOT, "flatbuffers"), pa.P_FLATBUFFERS],
    pa.GENERATED_FLATBUFFERS_CPP
  );
  
  await flatbuffers_generate_ts(
    [path.join(IDF_COMPONENT_WEBMANAGER_ROOT, "flatbuffers"), pa.P_FLATBUFFERS],
    path.join(c.c.generatedDirectory, "flatbuffers_ts")
  );
  
  cb();
}



export async function createBoardCertificatesLazily(cb: gulp.TaskFunctionCallback) {
  const c=await Context.get(contextConfig);
  const pa = new P.Paths(c);
  if (pa.existsBoardSpecificPath(P.CERTIFICATES_SUBDIR, P.ESP32_CERT_PEM_PRVTKEY_FILE)
    && pa.existsBoardSpecificPath(P.CERTIFICATES_SUBDIR, P.ESP32_CERT_PEM_CRT_FILE)) {
    return cb();
  }
  const hostname = strInterpolator(HOSTNAME_TEMPLATE, {mac_6char:mac_6char(c.b.mac), mac_12char:mac_12char(c.b.mac)});
  let esp32Cert = cert.CreateAndSignCert(hostname, hostname, path.join(CERTIFICATES, P.ROOT_CA_PEM_CRT_FILE), path.join(CERTIFICATES, P.ROOT_CA_PEM_PRVTKEY_FILE));
  pa.writeBoardSpecificFileCreateDirLazy(P.CERTIFICATES_SUBDIR, P.ESP32_CERT_PEM_PRVTKEY_FILE, esp32Cert.privateKey);
  pa.writeBoardSpecificFileCreateDirLazy(P.CERTIFICATES_SUBDIR, P.ESP32_CERT_PEM_CRT_FILE, esp32Cert.certificate);
  cb();
}

async function createObjectWithDefines(c:Context) {
  var defines: any = {};
  
  var s= new sensact.Sensact(c, SENSACT_COMPONENT_GENERATED_PATH)
  defines=s.AddSensactNodeAndModelDescriptorToDefines(defines);
  
  
  for (const [k, v] of Object.entries(c.b.board_settings?.web ?? {})) {
    defines[k] = v;
  }
  
  for (const [k, v] of Object.entries(c.b.board_settings?.firmware ?? {})) {
    defines[k] = v;
  }
  
  
  const now = new Date();
  defines.NODE_ID=s.GetNodeId();
  defines.BOARD_NAME = c.b.board_name;
  defines.BOARD_VERSION = c.b.board_version;
  defines.BOARD_MAC = c.b.mac;
  defines.APP_NAME = APPLICATION_NAME;
  defines.APP_VERSION = APPLICATION_VERSION;
  defines.CREATION_DT = Math.floor(now.valueOf() / 1000);
  defines.CREATION_DT_STR = now.toLocaleString("de-DE", MyFavouriteDateTimeFormat)
  defines.GIT_SHORT_HASH = (await getLastCommit(true)).shortHash;
  defines.BANNER = ascii_art.createAsciiArt(APPLICATION_NAME);
  return defines;
}

export async function buildAndCompressWebProject(cb: gulp.TaskFunctionCallback) {
  const c = await Context.get(contextConfig);
  const pa = new P.Paths(c);
  await vite_helper.buildAndCompressWebProject(pa, path.join(c.c.idfProjectDirectory, "web"), pa.GENERATED_WEB,  await createObjectWithDefines(c));
  return cb();
}


export async function createCppConfigurationHeader(cb: gulp.TaskFunctionCallback) {
  const c=await Context.get(contextConfig);
  const p = new P.Paths(c);
  const defines = await createObjectWithDefines(c);
  var s = new StringBuilderImpl("#pragma once\n");
  for (const [k, v] of Object.entries(defines)) {
    s.AppendLine(`#define ${k} ${JSON.stringify(v)}`);
  }
  writeFileCreateDirLazy(path.join(p.GENERATED_RUNTIMECONFIG_CPP,"runtimeconfig_defines.hh"), s.Code);
  s = new StringBuilderImpl("#pragma once\nnamespace cfg{");
  for (const [k, v] of Object.entries(defines)) {

    s.AppendLine(`\tconstexpr auto ${k}{${JSON.stringify(v)}};`);
  }
  s.AppendLine("}//namespace")
  writeFileCreateDirLazy(path.join(p.GENERATED_RUNTIMECONFIG_CPP,"runtimeconfig.hh"), s.Code);

  return cb();
}

export async function copyMostRecentlyConnectedBoardFilesToCurrent(cb: gulp.TaskFunctionCallback) {
  const c=await Context.get(contextConfig);
  const p = new P.Paths(c);
  fs.cpSync(p.boardSpecificPath(), p.GENERATED_CURRENT_BOARD, { recursive: true });
  return cb();
}




