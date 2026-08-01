// Ersetzt @klaus-liebler/vite-single-file (reines JS/CSS-Inlining) durch ein einzelnes Plugin, das
// zusaetzlich noch minifiziert und Brotli-komprimiert -- uebernommen aus dem Referenzprojekt
// (factory_in_a_box/firmware_factory_control_unit/builder/vite-plugin-single-file-firmware-asset.ts,
// s. docs/plan_v2/02-builder-migration-csharp.md, Abschnitt "BuildWebApp", fuer den Anlass der
// Uebernahme). Angepasst nur an sensacts Ausgabekonvention (s. "Angepasst" unten) -- die eigentliche
// Inlining-/Minifizierungs-/Kompressionslogik ist unveraendert:
//
//   1) Inlined JS+CSS in die index.html -- der Kern davon ist an
//      https://github.com/richardtallent/vite-plugin-singlefile (MIT-lizenziert, Autor Richard
//      Tallent) angelehnt/nachgebaut: replaceScript()/replaceCss() unten entsprechen dessen
//      gleichnamigen Funktionen, auf das hier tatsaechlich benoetigte Minimum (ein JS- und ein
//      CSS-Bundle) zugeschnitten.
//   2) minifyHtmlDocument() (singlefile-minify.ts) entfernt zusaetzliche Leerzeichen aus dem
//      inlinierten HTML/CSS/JS -- inklusive Lit-Tagged-Template-Inhalten (html`...`/css`...`), die
//      generische Minifizierer als beliebige String-Literale unangetastet liessen.
//   3) Brotli-komprimiert das Ergebnis direkt im selben Rutsch.
//
// ANGEPASST: das Referenzprojekt schreibt fest nach "../build/assets/index.html.br" (relativ zur
// Plugin-Datei, STM32-spezifische Konvention). sensact schreibt generierte Dateien stattdessen in
// ein gemeinsames, externes Verzeichnis (s. builder_cs/Paths.cs: GeneratedWebDir), das Vite selbst
// schon als Build-outDir bekommt -- deshalb wird HIER options.dir (der von Vite aufgeloeste
// tatsaechliche Ausgabeordner, unabhaengig davon ob per Config-Datei oder --outDir-CLI-Flag
// gesetzt) statt eines fest verdrahteten relativen Pfads verwendet, und der Dateiname ist
// parametrisierbar (main/CMakeLists.txt bindet das Ergebnis unter dem Namen "index.compressed.br"
// ein, nicht "index.html.br" wie in der Referenz).
import { brotliCompressSync, constants as zlibConstants } from "node:zlib";
import { mkdirSync, writeFileSync } from "node:fs";
import path from "node:path";
import type { Plugin, UserConfig } from "vite";
import type { OutputAsset, OutputChunk, OutputBundle } from "rollup";
import { minifyHtmlDocument } from "./singlefile-minify.ts";

const isJsFile = /\.[mc]?js$/;
const isCssFile = /\.css$/;
const isHtmlFile = /\.html?$/;

// Angelehnt an vite-plugin-singlefile's replaceScript() (s. Datei-Kommentar oben).
function replaceScript(html: string, scriptFilename: string, scriptCode: string): string {
	const escapedFilename = scriptFilename.replaceAll(".", "\\.");
	const scriptTagPattern = new RegExp(`<script([^>]*?) src="(?:[^"]*?/)?${escapedFilename}"([^>]*)></script>`);
	const preloadMarker = /"?__VITE_PRELOAD__"?/g;
	const newCode = scriptCode.replace(preloadMarker, "void 0").replace(/<(\/script>|!--)/g, "\\x3C$1");
	return html.replace(scriptTagPattern, (_match, beforeSrc, afterSrc) => `<script${beforeSrc}${afterSrc}>${newCode.trim()}</script>`);
}

// Angelehnt an vite-plugin-singlefile's replaceCss() (s. Datei-Kommentar oben).
function replaceCss(html: string, styleFilename: string, cssCode: string): string {
	const escapedFilename = styleFilename.replaceAll(".", "\\.");
	const linkTagPattern = new RegExp(`<link([^>]*?) href="(?:[^"]*?/)?${escapedFilename}"([^>]*)>`);
	const newCode = cssCode.replace(`@charset "UTF-8";`, "");
	return html.replace(linkTagPattern, (_match, beforeHref, afterHref) => `<style${beforeHref}${afterHref}>${newCode.trim()}</style>`);
}

function inlineBundleIntoHtml(bundle: OutputBundle): OutputAsset | undefined {
	let htmlAsset: OutputAsset | undefined;
	const jsChunks: OutputChunk[] = [];
	const cssAssets: OutputAsset[] = [];
	const toDelete: string[] = [];

	for (const [fileName, item] of Object.entries(bundle)) {
		if (isHtmlFile.test(fileName)) {
			htmlAsset = item as OutputAsset;
		} else if (isJsFile.test(fileName) && item.type === "chunk") {
			jsChunks.push(item);
		} else if (isCssFile.test(fileName) && item.type === "asset") {
			cssAssets.push(item as OutputAsset);
		}
	}

	if (!htmlAsset) return undefined;

	let html = htmlAsset.source as string;
	for (const chunk of jsChunks) {
		html = replaceScript(html, chunk.fileName, chunk.code);
		toDelete.push(chunk.fileName);
	}
	for (const asset of cssAssets) {
		html = replaceCss(html, asset.fileName, asset.source as string);
		toDelete.push(asset.fileName);
	}

	htmlAsset.source = html;
	for (const fileName of toDelete) {
		delete bundle[fileName];
	}
	return htmlAsset;
}

export function singleFileFirmwareAssetPlugin(compressedFileName: string = "index.compressed.br"): Plugin {
	return {
		name: "single-file-firmware-asset",
		enforce: "post",

		// Entspricht vite-plugin-singlefile's "useRecommendedBuildConfig": alle Assets/Chunks
		// muessen in EINEM JS- und EINEM CSS-Bundle landen, damit generateBundle unten ueberhaupt
		// etwas zum Inlinen hat.
		config(): UserConfig {
			return {
				build: {
					assetsInlineLimit: () => true,
					cssCodeSplit: false,
					assetsDir: "",
					rollupOptions: {
						output: {
							inlineDynamicImports: true,
						},
					},
				},
				base: "./",
			};
		},

		async generateBundle(options, bundle) {
			const htmlAsset = inlineBundleIntoHtml(bundle);
			if (!htmlAsset) return;

			const minified = await minifyHtmlDocument(htmlAsset.source as string);
			htmlAsset.source = minified;

			const htmlBuffer = Buffer.from(minified, "utf8");
			const compressed = brotliCompressSync(htmlBuffer, {
				params: {
					[zlibConstants.BROTLI_PARAM_QUALITY]: zlibConstants.BROTLI_MAX_QUALITY,
					[zlibConstants.BROTLI_PARAM_SIZE_HINT]: htmlBuffer.length,
				},
			});

			// options.dir ist der von Vite tatsaechlich aufgeloeste Ausgabeordner (Config-Datei ODER
			// --outDir-CLI-Flag, s. Datei-Kommentar oben) -- kein fest verdrahteter relativer Pfad.
			const outDir = options.dir ?? ".";
			mkdirSync(outDir, { recursive: true });
			const outFile = path.join(outDir, compressedFileName);
			writeFileSync(outFile, compressed);
			console.log(`[single-file-firmware-asset] ${htmlBuffer.length} B -> brotli ${compressed.length} B -> ${outFile}`);
		},
	};
}
