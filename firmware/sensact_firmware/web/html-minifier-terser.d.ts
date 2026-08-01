// "Shorthand ambient module"-Deklaration: html-minifier-terser liefert keine eigenen
// Typdeklarationen und es gibt kein @types/html-minifier-terser -- ohne diese Datei meldet tsc
// (strict/noImplicitAny) einen harten TS7016-Fehler beim Import in singlefile-minify.ts. Bewusst
// die von TS selbst vorgeschlagene Leerform (kein Body) statt einer Modul-Augmentation mit
// eigenen Typen -- letztere scheitert an TS2665, weil das Modul bereits auf eine echte (nur
// untypisierte) Datei aufloest.
declare module "html-minifier-terser";
