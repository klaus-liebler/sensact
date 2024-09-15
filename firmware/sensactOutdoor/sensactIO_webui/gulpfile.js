const gulp = require("gulp");
const inlinesource = require('gulp-inline-source-html');
var gzip = require('gulp-gzip');
const gulp_clean = require('gulp-clean');


const index_file = "/index.html"; //must start with path separator
const angular_dist_file = "./dist/sensactIO_webui" + index_file;
const inlined_directory = "./dist_compressed";
const inlined_file = inlined_directory + index_file;

function clean(cb) {
  return gulp.src('{dist_compressed}', { read: false, allowEmpty: true }).pipe(gulp_clean());
}

function inlineFiles(cb) {
  let options = {
    attribute: false,
    compress: false
  };
  return gulp.src(angular_dist_file)
    .pipe(inlinesource(options))
    .pipe(gulp.dest(inlined_directory));
}

function compress(cb) {
  return gulp.src(inlined_file)
    //.pipe(gulpBrotli({extension:"br", quality:11}))
    .pipe(gzip())
    .pipe(gulp.dest(inlined_directory))
}

exports.build = gulp.series(
  clean,
  inlineFiles,
  compress
);

exports.default = exports.build;