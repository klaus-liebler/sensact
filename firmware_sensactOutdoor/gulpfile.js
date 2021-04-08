var gulp = require("gulp");
var browserify = require("browserify");
var source = require("vinyl-source-stream");
var tsify = require("tsify");
const minify = require('gulp-minify');
var sourcemaps = require("gulp-sourcemaps");
var buffer = require("vinyl-buffer");
const { series, parallel } = require("gulp");
const sass = require('gulp-sass');
const gulp_clean = require('gulp-clean');
const gzip = require('gulp-gzip');
const inlinesource = require('gulp-inline-source');

sass.compiler = require('node-sass');

var paths = {
    pages: ["webui/*.html"],
  };
  
function clean(cb)
{
    return gulp.src('{dist,dist_compressed}', {read: false, allowEmpty:true}).pipe(gulp_clean());
}


function tsTranspileAndBundleAndMinify(cb)
{
    return browserify({
        basedir: ".",
        debug: true,
        entries: ["webui/Main.ts"],
        cache: {},
        packageCache: {},
      })
        .plugin(tsify)
        .bundle()
        .pipe(source("bundle.js"))
        .pipe(buffer())
        .pipe(sourcemaps.init({ loadMaps: true }))
        .pipe(minify())
        .pipe(sourcemaps.write("./"))
        .pipe(gulp.dest("webui_dist"));
}

function cssTranspile(cb)
{
    return gulp.src('./webui/main.scss')
    .pipe(sourcemaps.init())
    .pipe(sass({outputStyle: 'compressed'}).on('error', sass.logError))
    .pipe(sourcemaps.write('./'))
    .pipe(gulp.dest('./webui_dist'));
}



function htmlCopy(cb)
{
    return gulp.src(paths.pages).pipe(gulp.dest("webui_dist"));
}

function htmlInline(cb)
{
    return gulp.src('./webui_dist/index.html')
    .pipe(inlinesource())
    .pipe(gzip())
    .pipe(gulp.dest('./webui_dist_compressed'));
}



exports.build =  series(
    clean,
    cssTranspile,
    tsTranspileAndBundleAndMinify,
    htmlCopy,
    htmlInline
  );
exports.default = exports.build;
exports.cssTranspile=cssTranspile;
