import * as gulp from "gulp";
import * as ts from "gulp-typescript";
import * as exec from 'gulp-exec';
import * as cp from "child_process";
import * as path from 'path';
import * as clean from "gulp-clean";
import { dest } from "gulp";
const inlinesource = require('gulp-inline-source');
const gzip = require('gulp-gzip');


const replace = require('gulp-replace');

const SRC_GLOB_FLATBUFFERS = "./flatbuffers/idl/*.fbs";
const SRC_GLOB_FLATBUFFERS_IOCFG_CREATORS = "./flatbuffers/creators/create_iocfg*.ts";
const GEN = "./generated/"
const GEN_FLATBUFFER_IDL = GEN + "flatbuffer_idl/";
const GEN_FLATBUFFER_CREATORS = GEN + "flatbuffer_creators/";
const GEN_FLATBUFFER_IOCFG = GEN + "flatbuffer_iocfg/";

const WEBUI_PROJECT_ROOT = "../sensactIO_webui/";
const DEST_FLATBUFFERS_TS = WEBUI_PROJECT_ROOT + "src/app/";
const DEST_FLATBUFFERS_CPP = "../sensactIO_firmware/main/generated/";

const DEST_SPIFFS_DIR = "../sensactIO_firmware/spiffs_filesystem/";
//const DEST_DEFAULT_IOCFG = DEST_SPIFFS_DIR+"defaultiocfg.bin";

const index_file = "index.html"; //must start with path separator
const angular_dist_file = WEBUI_PROJECT_ROOT + "dist/sensactIO_webui/" + index_file;
const inlined_directory = WEBUI_PROJECT_ROOT + "dist_compressed/";
const inlined_file = inlined_directory + index_file;


function inlineWebui() {
    let options = {
        attribute: false,
        compress: false
    };
    return gulp.src(angular_dist_file)
        .pipe(inlinesource(options))
        .pipe(gulp.dest(inlined_directory));
}

function compressWebui() {
    return gulp.src(inlined_file)
        .pipe(gzip())
        .pipe(gulp.dest(inlined_directory))
}

const exec_options = {
    continueOnError: false, // default = false, true means don't emit error event
    pipeStdout: false, // default = false, true means stdout is written to file.contents
};

const exec_reportOptions = {
    err: true, // default = true, false means don't write err
    stderr: true, // default = true, false means don't write stderr
    stdout: true // default = true, false means don't write stdout
};

function createFlatbuffersTsHeader() {
    return gulp.src(SRC_GLOB_FLATBUFFERS)
        .pipe(exec((file: any) => `flatc --ts -o ${GEN_FLATBUFFER_IDL}  ${file.path}`, exec_options))
        .pipe(exec.reporter(exec_reportOptions));
}

//Not necessary in recent version
function modifyFlatbuffersTSHeader() {
    return gulp.src(GEN_FLATBUFFER_IDL + "*.ts")
        .pipe(replace('import { flatbuffers } from "./flatbuffers"', 'import { flatbuffers } from "flatbuffers"'))
        .pipe(dest(GEN_FLATBUFFER_IDL));
}

function createFlatbuffersCppHeader() {
    return gulp.src(SRC_GLOB_FLATBUFFERS)
        .pipe(exec((file: any) => `flatc --cpp -o ${GEN_FLATBUFFER_IDL}  ${file.path}`, exec_options))
        .pipe(exec.reporter(exec_reportOptions));
}

function copyFlatbufferTSRessourceToProject() {
    return gulp.src(GEN_FLATBUFFER_IDL + "**/*.[tj]s")

        .pipe(gulp.dest(DEST_FLATBUFFERS_TS))
}

function copyFlatbufferCPPRessourceToProject() {
    return gulp.src(GEN_FLATBUFFER_IDL + "*.h").pipe(gulp.dest(DEST_FLATBUFFERS_CPP))
}

function transpileFlatbuffersTSHeader() {
    return gulp.src(GEN_FLATBUFFER_IDL + "**/*.ts")

        .pipe(ts({
            noImplicitAny: true,
            rootDir: "C:/repos/sensactIO/sensactIO_buildtools",
        }))
        .pipe(gulp.dest(GEN_FLATBUFFER_IDL));
}

function transpileFlatbufferCfgCreators() {
    return gulp.src(SRC_GLOB_FLATBUFFERS_IOCFG_CREATORS)
        .pipe(ts({
            noImplicitAny: true,
            rootDir: "C:/repos/sensactIO/sensactIO_buildtools",
        }))
        .pipe(gulp.dest(GEN_FLATBUFFER_CREATORS));
}

function executeFlatbufferCfgCreators() {
    return gulp.src(GEN_FLATBUFFER_CREATORS + "*.js")
        .pipe(exec((file: any) => `node ${file.path} ${GEN_FLATBUFFER_IOCFG + path.basename(file.path, ".js") + ".bin"}`, exec_options));
}

function cleanGen() {
    return gulp.src(GEN, { "read": false, "allowEmpty": true })
        .pipe(clean());
}

function copyFlatbuffersCfgToProject() {
    return gulp.src(GEN_FLATBUFFER_IOCFG + "*.bin").pipe(gulp.dest(DEST_SPIFFS_DIR))
}

function buildWebuiProd(cb: Function) {
    let options = {
        cwd: WEBUI_PROJECT_ROOT
    };
    return cp.exec("ng build", options, (err, stdout, stderr) => {
        if (err) {
            console.error(`exec error: ${err.code}`);
            console.error(`stderr: ${stderr}`);
        }
        console.log(`Number of files ${stdout}`);
        cb();
    });
}

function buildAndFlash(cb: Function) {
    cb();
}



exports.build = gulp.series(
    cleanGen,
    gulp.parallel(createFlatbuffersTsHeader, createFlatbuffersCppHeader),
    transpileFlatbuffersTSHeader,
    gulp.parallel(copyFlatbufferTSRessourceToProject, copyFlatbufferCPPRessourceToProject),
    transpileFlatbufferCfgCreators,
    executeFlatbufferCfgCreators,
    copyFlatbuffersCfgToProject,
    buildWebuiProd,
    inlineWebui,
    compressWebui,
    buildAndFlash
);

exports.webui=gulp.series(
    buildWebuiProd,
    inlineWebui,
    compressWebui,
);

exports.default = exports.build;