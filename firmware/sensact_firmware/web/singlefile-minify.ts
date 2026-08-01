// Whitespace-/Lit-Template-Minifizierung fuer das bereits zu einer einzigen index.html inlinete
// Bundle (s. vite-plugin-single-file-firmware-asset.ts). Unveraendert aus dem Referenzprojekt
// (factory_in_a_box/firmware_factory_control_unit/builder/singlefile-minify.ts) uebernommen --
// reine Text-/AST-Verarbeitung ohne STM32-Spezifika, s. docs/plan_v2/02-builder-migration-csharp.md
// (Abschnitt "BuildWebApp") fuer den Anlass der Uebernahme.
//
// Explizite Referenz statt Verlass auf tsc's Default-Datei-Discovery: html-minifier-terser.d.ts
// liegt zwar im selben Ordner, wird aber NICHT automatisch eingesammelt.
/// <reference path="./html-minifier-terser.d.ts" />
import { minify as minifyHtml } from "html-minifier-terser";
import { minify as minifyJs } from "terser";

// html-minifier-terser liefert keine eigenen Typdeklarationen (anders als terser) und auch keine
// @types/html-minifier-terser -- "shorthand ambient module"-Deklaration (s. html-minifier-terser.d.ts)
// behandelt den Import als "any" statt eines harten TS7016-Fehlers.

function isIdentChar(char: string): boolean {
	return /[A-Za-z0-9_$]/.test(char);
}

function skipQuotedString(code: string, startIndex: number, quote: string): number {
	let i = startIndex + 1;
	while (i < code.length) {
		const ch = code[i];
		if (ch === "\\") {
			i += 2;
			continue;
		}
		if (ch === quote) return i + 1;
		i += 1;
	}
	return i;
}

function skipBlockComment(code: string, startIndex: number): number {
	let i = startIndex + 2;
	while (i + 1 < code.length) {
		if (code[i] === "*" && code[i + 1] === "/") return i + 2;
		i += 1;
	}
	return i;
}

function skipLineComment(code: string, startIndex: number): number {
	let i = startIndex + 2;
	while (i < code.length && code[i] !== "\n") i += 1;
	return i;
}

function skipRegexLiteral(code: string, startIndex: number): number {
	let i = startIndex + 1;
	let inCharClass = false;

	while (i < code.length) {
		const ch = code[i];
		if (ch === "\\") {
			i += 2;
			continue;
		}
		if (ch === "[") {
			inCharClass = true;
			i += 1;
			continue;
		}
		if (ch === "]") {
			inCharClass = false;
			i += 1;
			continue;
		}
		if (ch === "/" && !inCharClass) {
			i += 1;
			while (i < code.length && /[A-Za-z]/.test(code[i]!)) i += 1;
			return i;
		}
		i += 1;
	}

	return i;
}

function canStartRegexLiteral(code: string, slashIndex: number): boolean {
	let i = slashIndex - 1;
	while (i >= 0 && /\s/.test(code[i]!)) i -= 1;
	if (i < 0) return true;

	const prev = code[i]!;
	if ("([{:;,=!&|?+-*%^~<>".includes(prev)) return true;

	let end = i;
	while (i >= 0 && /[A-Za-z_$]/.test(code[i]!)) i -= 1;
	if (i !== end) {
		const word = code.slice(i + 1, end + 1);
		if (
			word === "return" ||
			word === "throw" ||
			word === "case" ||
			word === "delete" ||
			word === "void" ||
			word === "typeof" ||
			word === "instanceof" ||
			word === "in"
		) {
			return true;
		}
	}

	return false;
}

function skipTemplateLiteral(code: string, startIndex: number): number {
	let i = startIndex + 1;
	while (i < code.length) {
		const ch = code[i];
		if (ch === "\\") {
			i += 2;
			continue;
		}
		if (ch === "`") return i + 1;
		if (ch === "$" && code[i + 1] === "{") {
			i = skipJsExpression(code, i + 2);
			continue;
		}
		i += 1;
	}
	return i;
}

function skipJsExpression(code: string, startIndex: number): number {
	let i = startIndex;
	let depth = 1;

	while (i < code.length && depth > 0) {
		const ch = code[i]!;
		const next = code[i + 1];

		if (ch === '"' || ch === "'" || ch === "`") {
			i = ch === "`" ? skipTemplateLiteral(code, i) : skipQuotedString(code, i, ch);
			continue;
		}

		if (ch === "/" && next === "*") {
			i = skipBlockComment(code, i);
			continue;
		}

		if (ch === "/" && next !== "*" && next !== "/" && canStartRegexLiteral(code, i)) {
			i = skipRegexLiteral(code, i);
			continue;
		}

		if (ch === "/" && next === "/") {
			i = skipLineComment(code, i);
			continue;
		}

		if (ch === "{") depth += 1;
		if (ch === "}") depth -= 1;
		i += 1;
	}

	return i;
}

interface ParsedTemplate {
	endIndex: number;
	parts: string[];
	expressions: string[];
}

function parseTaggedTemplate(code: string, backtickIndex: number): ParsedTemplate | null {
	let i = backtickIndex + 1;
	let literal = "";
	const parts: string[] = [];
	const expressions: string[] = [];

	while (i < code.length) {
		const ch = code[i]!;

		if (ch === "\\") {
			literal += code.slice(i, i + 2);
			i += 2;
			continue;
		}

		if (ch === "`") {
			parts.push(literal);
			return { endIndex: i + 1, parts, expressions };
		}

		if (ch === "$" && code[i + 1] === "{") {
			parts.push(literal);
			literal = "";
			const expressionStart = i + 2;
			const afterExpression = skipJsExpression(code, expressionStart);
			const expressionEnd = afterExpression - 1;
			expressions.push(code.slice(expressionStart, expressionEnd));
			i = afterExpression;
			continue;
		}

		literal += ch;
		i += 1;
	}

	return null;
}

function minifyCssFragment(cssText: string): string {
	return cssText
		.replace(/\s+/g, " ")
		.replace(/\s*([{}:;,>])\s*/g, "$1")
		.replace(/;}/g, "}")
		.trim();
}

function minifyHtmlFragment(htmlText: string): string {
	return htmlText
		.replace(/[\r\n\t]+/g, " ")
		.replace(/>\s+</g, "><")
		.replace(/\s{2,}/g, " ")
		.trim();
}

async function minifyTemplateByTag(tagName: string, parts: string[], expressions: string[]): Promise<string> {
	const placeholders = expressions.map((_, index) => `__EXPR_${index}__`);
	let withPlaceholders = parts[0] ?? "";
	for (let i = 0; i < expressions.length; i += 1) {
		withPlaceholders += placeholders[i] + (parts[i + 1] ?? "");
	}

	const looksLikeHtmlTemplate = (literal: string): boolean => {
		const trimmed = literal.trim();
		return /<\/?[A-Za-z][\w:-]*(\s|\/?>)/.test(trimmed);
	};

	let minifiedLiteral: string;
	const shouldMinifyCss = tagName === "css" || tagName === "P";
	const shouldMinifyHtml = tagName === "html" || tagName === "o" || looksLikeHtmlTemplate(withPlaceholders);

	if (shouldMinifyCss) {
		minifiedLiteral = minifyCssFragment(withPlaceholders);
	} else if (shouldMinifyHtml) {
		minifiedLiteral = minifyHtmlFragment(withPlaceholders);
	} else {
		minifiedLiteral = withPlaceholders;
	}

	let rebuilt = minifiedLiteral;
	for (let i = 0; i < expressions.length; i += 1) {
		rebuilt = rebuilt.replace(placeholders[i]!, `\${${expressions[i]}}`);
	}

	return `\`${rebuilt}\``;
}

async function minifyLitTaggedTemplates(scriptCode: string): Promise<string> {
	let result = "";
	let i = 0;
	const isIdentStart = (char: string): boolean => /[A-Za-z_$]/.test(char);

	while (i < scriptCode.length) {
		const ch = scriptCode[i]!;

		if (ch === '"' || ch === "'") {
			const end = skipQuotedString(scriptCode, i, ch);
			result += scriptCode.slice(i, end);
			i = end;
			continue;
		}

		if (ch === "/" && scriptCode[i + 1] === "*") {
			const end = skipBlockComment(scriptCode, i);
			result += scriptCode.slice(i, end);
			i = end;
			continue;
		}

		if (ch === "/" && scriptCode[i + 1] === "/") {
			const end = skipLineComment(scriptCode, i);
			result += scriptCode.slice(i, end);
			i = end;
			continue;
		}

		if (ch === "`") {
			const end = skipTemplateLiteral(scriptCode, i);
			result += scriptCode.slice(i, end);
			i = end;
			continue;
		}

		const prev = scriptCode[i - 1] ?? "";
		if (isIdentStart(ch) && (!prev || !isIdentChar(prev))) {
			let endIdent = i + 1;
			while (endIdent < scriptCode.length && isIdentChar(scriptCode[endIdent]!)) {
				endIdent += 1;
			}

			const ident = scriptCode.slice(i, endIdent);
			if (scriptCode[endIdent] === "`") {
				const parsed = parseTaggedTemplate(scriptCode, endIdent);
				if (parsed) {
					const template = await minifyTemplateByTag(ident, parsed.parts, parsed.expressions);
					result += ident + template;
					i = parsed.endIndex;
					continue;
				}
			}

			result += ident;
			i = endIdent;
			continue;
		}

		result += ch;
		i += 1;
	}

	return result;
}

async function minifyRenderMethodTemplates(scriptCode: string): Promise<string> {
	const renderTemplatePattern = /render\(\)\{return\s+([A-Za-z_$][A-Za-z0-9_$]*)`/g;
	let result = "";
	let lastIndex = 0;
	let match: RegExpExecArray | null;

	while ((match = renderTemplatePattern.exec(scriptCode)) !== null) {
		const tagName = match[1]!;
		const backtickIndex = renderTemplatePattern.lastIndex - 1;
		const tagStart = backtickIndex - tagName.length;
		const parsed = parseTaggedTemplate(scriptCode, backtickIndex);

		if (!parsed) continue;

		result += scriptCode.slice(lastIndex, tagStart);
		const minifiedTemplate = await minifyTemplateByTag("html", parsed.parts, parsed.expressions);
		result += tagName + minifiedTemplate;

		lastIndex = parsed.endIndex;
		renderTemplatePattern.lastIndex = parsed.endIndex;
	}

	result += scriptCode.slice(lastIndex);
	return result;
}

async function minifyInlineModuleScripts(htmlSource: string): Promise<string> {
	const scriptPattern = /<script\b([^>]*)>([\s\S]*?)<\/script>/gi;
	let result = "";
	let lastIndex = 0;
	let match: RegExpExecArray | null;

	while ((match = scriptPattern.exec(htmlSource)) !== null) {
		const fullMatch = match[0];
		const attrs = match[1] ?? "";
		const scriptCode = match[2] ?? "";
		const start = match.index;
		const end = start + fullMatch.length;
		const isModule = /\btype\s*=\s*(?:"module"|'module'|module)(?=\s|$)/i.test(attrs);

		result += htmlSource.slice(lastIndex, start);

		if (!isModule) {
			result += fullMatch;
			lastIndex = end;
			continue;
		}

		const literalMinifiedCode = await minifyLitTaggedTemplates(scriptCode);
		const renderTemplateMinifiedCode = await minifyRenderMethodTemplates(literalMinifiedCode);

		const jsMinified = await minifyJs(renderTemplateMinifiedCode, {
			ecma: 2020,
			module: true,
			compress: { passes: 2 },
			mangle: true,
			format: { comments: false },
		});

		const finalScriptCode = jsMinified.code ?? renderTemplateMinifiedCode;
		result += `<script${attrs}>${finalScriptCode}</script>`;
		lastIndex = end;
	}

	result += htmlSource.slice(lastIndex);
	return result;
}

// Nimmt die bereits zu einem einzigen <script type="module">/<style> inlinete index.html
// (s. vite-plugin-single-file-firmware-asset.ts) und entfernt zusaetzliche Leerzeichen aus
// HTML/CSS/JS -- inklusive Lit-Tagged-Template-Inhalten, die generische Minifizierer sonst als
// beliebige String-Literale unangetastet lassen wuerden.
export async function minifyHtmlDocument(htmlSource: string): Promise<string> {
	const htmlWithMinifiedScripts = await minifyInlineModuleScripts(htmlSource);
	return minifyHtml(htmlWithMinifiedScripts, {
		collapseWhitespace: true,
		collapseInlineTagWhitespace: true,
		removeComments: true,
		removeRedundantAttributes: true,
		removeScriptTypeAttributes: true,
		removeTagWhitespace: true,
		removeAttributeQuotes: true,
		useShortDoctype: true,
		minifyCSS: true,
		minifyJS: true,
		caseSensitive: true,
	});
}
