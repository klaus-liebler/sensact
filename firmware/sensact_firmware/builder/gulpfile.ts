import * as gulp from "gulp";
import fs from "node:fs";
import path from "node:path";
import * as cert from "@klaus-liebler/espidf-vite/certificates";
import * as P from "@klaus-liebler/espidf-vite/paths";
import * as idf from "@klaus-liebler/espidf-vite/espidf";
import { getLastCommit } from "@klaus-liebler/espidf-vite/git";
import { flatbuffers_generate_c, flatbuffers_generate_ts } from "@klaus-liebler/espidf-vite/flatbuffers";
import {Context, ContextConfig} from "@klaus-liebler/espidf-vite/context"
import {mac_12char, mac_6char, writeFileCreateDirLazy } from "@klaus-liebler/espidf-vite/utils";
import * as vite_helper from "@klaus-liebler/espidf-vite/vite_helper";
import { strInterpolator } from "@klaus-liebler/commons";
import * as sensact from "./sensact";
//Default Board Type

export const DEFAULT_BOARD_NAME="SENSACT_OUTDOOR"
export const DEFAULT_BOARD_VERSION=0


//Paths
export const IDF_PATH=globalThis.process.env.IDF_PATH as string;
export const USERPROFILE =globalThis.process.env.USERPROFILE as string;

//Config
const IDF_PROJECT_ROOT = `C:\\repos\\sensact\\firmware\\sensact_firmware`
const IDF_COMPONENT_WEBMANAGER_ROOT = "C:/repos/espidf-component-webmanager";
const GENERATED_ROOT = "c:\\repos\\generated";
const FLATBUFFER_OBJECT_DEFINITIONS_NPM_PROJECT = "C:/repos/npm-packages/@klaus-liebler/flatbuffer-object-definitions"

const SENSACT_COMPONENT_GENERATED_PATH = "C:/repos/sensact/espidf-components/generated";
const BOARDS_BASE_DIR= path.join(USERPROFILE, "netcase/esp32_boards");
const CERTIFICATES = path.join(USERPROFILE, "netcase/certificates");


const HOSTNAME_TEMPLATE = "sensact_${mac_6char}"
const APPLICATION_NAME = "labathome"
const APPLICATION_VERSION = "1.0"

const contextConfig = new ContextConfig(GENERATED_ROOT, IDF_PROJECT_ROOT, BOARDS_BASE_DIR, DEFAULT_BOARD_NAME, DEFAULT_BOARD_VERSION);


export const buildForCurrent = gulp.series(
  deleteGenerated,
  prepare_board_specific_files,
  compileAndDistributeFlatbuffers,
  buildAndCompressWebProject,
  createBoardCertificatesLazily,
  createRandomFlashEncryptionKeyLazily,
  createCppConfigurationHeader,
  copyMostRecentlyConnectedBoardFilesToCurrent,
  buildFirmware,
  encryptFirmware,
)

export default gulp.series(
  addOrUpdateConnectedBoard,
  buildForCurrent,
  flashEncryptedFirmware,
)
async function deleteGenerated(cb: gulp.TaskFunctionCallback) {
  const c = await Context.get(contextConfig);
  fs.rmSync(c.c.generatedDirectory, {recursive:true, force:true})
}

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

export async function addOrUpdateConnectedBoard(cb: gulp.TaskFunctionCallback){
  await Context.get(contextConfig, true);
  return cb();
}

export async function prepare_board_specific_files(cb: gulp.TaskFunctionCallback){
  const c = await Context.get(contextConfig);
  var s= new sensact.Sensact(c, SENSACT_COMPONENT_GENERATED_PATH)
  s.prepare_sensact_files()

  return cb()
}

export async function compileAndDistributeFlatbuffers(cb: gulp.TaskFunctionCallback) {
  const c= await Context.get(contextConfig)
  const pa = new P.Paths(c);
  console.info(`${pa.GENERATED_FLATBUFFERS_FBS}`)
  await flatbuffers_generate_c(
    [path.join(IDF_COMPONENT_WEBMANAGER_ROOT, "flatbuffers"), pa.P_FLATBUFFERS],
    pa.GENERATED_FLATBUFFERS_CPP
  );
  
  await flatbuffers_generate_ts(
    [path.join(IDF_COMPONENT_WEBMANAGER_ROOT, "flatbuffers"), pa.P_FLATBUFFERS],
    c.c.generatedDirectory, "flatbuffers_ts"
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
  pa.writeBoardSpecificFileCreateDirLazy(P.CERTIFICATES_SUBDIR, P.ESP32_CERT_PEM_CRT_FILE, esp32Cert.certificate, cb);
}

async function createObjectWithDefines(c:Context) {
  var defines: any = {};
  for (const [k, v] of Object.entries(c.b.board_settings?.web ?? {})) {
    defines[k] = JSON.stringify(v);
  }

  for (const [k, v] of Object.entries(c.b.board_settings?.firmware ?? {})) {
    defines[k] = JSON.stringify(v);
  }

  defines.__BOARD_NAME__ = JSON.stringify(c.b.board_name);
  defines.__BOARD_VERSION__ = JSON.stringify(c.b.board_version);
  defines.__BOARD_MAC__ = JSON.stringify(c.b.mac);
  defines.__APP_NAME__ = JSON.stringify(APPLICATION_NAME);
  defines.__APP_VERSION__ = JSON.stringify(APPLICATION_VERSION);
  defines.__CREATION_DT__ = JSON.stringify(Math.floor(Date.now() / 1000));
  defines.__GIT_SHORT_HASH__ = JSON.stringify((await getLastCommit(true)).shortHash);
  return defines;
}

export async function buildAndCompressWebProject(cb: gulp.TaskFunctionCallback) {
  const c = await Context.get(contextConfig);
  const pa = new P.Paths(c);
  await vite_helper.buildAndCompressWebProject(path.join(c.c.idfProjectDirectory, "web"), pa.GENERATED_WEB,  await createObjectWithDefines(c));
  return cb();
}


export async function createCppConfigurationHeader(cb: gulp.TaskFunctionCallback) {
  const c=await Context.get(contextConfig);
  const p = new P.Paths(c);
  const defines = await createObjectWithDefines(c);
  var s = "#pragma once\n";
  for (const [k, v] of Object.entries(defines)) {
    s += `#define ${k} ${v}\n`
  }
  p.writeBoardSpecificFileCreateDirLazy("cpp", "__build_config.hh", s);
  return cb();
}

export async function copyMostRecentlyConnectedBoardFilesToCurrent(cb: gulp.TaskFunctionCallback) {
  const c=await Context.get(contextConfig);
  const p = new P.Paths(c);
  fs.cpSync(p.boardSpecificPath(), p.GENERATED_CURRENT_BOARD, { recursive: true });
  return cb();
}




