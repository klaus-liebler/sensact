const debug=true;

import * as gulp from "gulp"
import path from "node:path";
import * as child_process from "node:child_process";
const gulp_clean = require("gulp-clean");
const gzip = require("gulp-gzip");
const inlinesource = require("gulp-inline-source");
const sass = require('gulp-sass')(require('sass'));
const htmlmin = require("gulp-htmlmin");
const browserify = require('browserify');
const source = require('vinyl-source-stream');
const buffer = require('vinyl-buffer');
const tsify = require('tsify');
const rename = require('gulp-rename');


import templateSpecial from "./templateSpecial";
import { SearchReplace } from "./templateSpecial";



const html_spa_file = "websensact.html";
const scss_spa_file = "app.scss";
const tsconfig_path = "../websensact_typescript/tsconfig.json";
const flatbuffers_schema_path="../websensact_flatbuffers/websensact.fbs";
const htmlscss_base_path = "../websensact_htmlscss";
const htmlinc_generated_path = "../../generated";
const dist_base_path = "./dist_browser";
const dist_bundeled_base_path = "./dist_browser_bundeled";
const dist_compressed_base_path = "./dist_browser_compressed";
const typescript_generated_path = "../websensact_typescript/generated";
const dist_flatbuffers_cpp = "../include/generated";
const app_basename = "app";

export function clean(cb: gulp.TaskFunctionCallback) {
  return gulp.src([dist_base_path, dist_bundeled_base_path, dist_compressed_base_path, typescript_generated_path, dist_flatbuffers_cpp], { read: false, allowEmpty: true,  }).pipe(gulp_clean({force:true}));
}

function flatbuffers_c(cb: gulp.TaskFunctionCallback){
  child_process.exec(`flatc -c --gen-all -o ${dist_flatbuffers_cpp} ${flatbuffers_schema_path}`, (err, stdout, stderr) =>{
    cb(err);
  });
}

function flatbuffers_ts(cb: gulp.TaskFunctionCallback){
  child_process.exec(`flatc -T --gen-all --ts-no-import-ext -o ${typescript_generated_path} ${flatbuffers_schema_path}`, (err, stdout, stderr) =>{
    cb(err);
  });
}

function copyAndTemplateTypescriptSendCommandImplementation(cb: gulp.TaskFunctionCallback){
  return gulp.src(htmlinc_generated_path + "/common/sendCommandImplementation.ts.inc")
  .pipe(templateSpecial("../websensact_typescript/templates/sendCommandImplementation.template.ts", "//TEMPLATE_HERE", []))
  .pipe(rename("sendCommandImplementation.ts"))
  .pipe(gulp.dest(typescript_generated_path));
}

function typescriptCompile(cb: gulp.TaskFunctionCallback) {
  return browserify({
    standalone: "MyApp",
    debug: debug,
  })
    .add("../websensact_typescript/src/app.ts")

    .plugin(tsify, {
      noImplicitAny: true,
      project: tsconfig_path,
    })
    .bundle()

    .pipe(source('app.js'))
    .pipe(buffer())
    .pipe(gulp.dest(dist_base_path));
}

function scssTranspile(cb: gulp.TaskFunctionCallback) {
  return gulp.src(htmlscss_base_path + "/" + scss_spa_file)
    .pipe(sass({ outputStyle: 'compressed' }).on('error', sass.logError))
    .pipe(gulp.dest(dist_base_path));
}


const sr1 = new SearchReplace("<!--TEMPLATE_COMMON-->", path.join(htmlinc_generated_path, "common", "websensact.html.inc"));

function htmlIncludeAndMinify(cb: gulp.TaskFunctionCallback) {
  
  return gulp.src([htmlinc_generated_path + "/**/*.html.inc", "!" + htmlinc_generated_path + "/common/**"])
    .pipe(templateSpecial(htmlscss_base_path + "/websensact.template.html", "<!--TEMPLATE_SPECIAL-->", [sr1]))
    .pipe(htmlmin({ collapseWhitespace: true }))
    .pipe(gulp.dest(dist_base_path));
}


function htmlInline(cb: gulp.TaskFunctionCallback) {
  
  return gulp.src(dist_base_path + "/**/*.html")
    .pipe(inlinesource({ compress: !debug }))
    .pipe(gulp.dest(dist_bundeled_base_path));
}

function htmlGzip(cb: gulp.TaskFunctionCallback) {
  return gulp.src(dist_bundeled_base_path + "/**/*.html")
    .pipe(gzip())
    .pipe(gulp.dest(dist_compressed_base_path));
}

exports.build = gulp.series(
  clean,
  flatbuffers_c,
  flatbuffers_ts,
  copyAndTemplateTypescriptSendCommandImplementation,
  scssTranspile,
  typescriptCompile,
  htmlIncludeAndMinify,
  htmlInline,
  htmlGzip
);
exports.default = exports.build;