"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.$ = exports.StringNumberTuple = exports.Utils = void 0;
class Utils {
    static EventCoordinatesInSVG(evt, element, positionRatio = 1) {
        let rect = element.getBoundingClientRect();
        return { x: (evt.clientX - rect.left) / positionRatio, y: (evt.clientY - rect.top) / positionRatio };
    }
}
exports.Utils = Utils;
class StringNumberTuple {
    constructor(s, n) {
        this.s = s;
        this.n = n;
    }
}
exports.StringNumberTuple = StringNumberTuple;
class $ {
    static Svg(parent, type, classes, attributes) {
        return parent.appendChild($.Elem($.SVGNS, type, classes, "", attributes));
    }
    static SvgIcon(parent, iconname, classes = []) {
        let svg = $.Svg(parent, "svg", classes, ["icon", "icon-" + iconname]);
        let use = $.Svg(svg, "use", [], []);
        use.setAttributeNS(this.XLINKNS, "href", "#icon-" + iconname);
        parent.appendChild(svg);
        return svg;
    }
    static ColorNumColor2ColorDomString(num) {
        let str = num.toString(16);
        while (str.length < 6)
            str = "0" + num;
        return "#" + str;
    }
    static ColorDomString2ColorNum(colorString) {
        return parseInt(colorString.substring(1), 16);
    }
    static Html(parent, type, classes, textContent, attributes) {
        return parent.appendChild($.Elem($.HTMLNS, type, classes, textContent, attributes));
    }
    static HtmlAfter(previous, type, classes, textContent, attributes) {
        return previous.parentElement.insertBefore($.Elem($.HTMLNS, type, classes, textContent, attributes), previous.nextSibling);
    }
    static HtmlAsFirstChild(parent, type, classes, textContent, attributes) {
        if (parent.firstChild)
            return parent.insertBefore($.Elem($.HTMLNS, type, classes, textContent, attributes), parent.firstChild);
        else
            return parent.appendChild($.Elem($.HTMLNS, type, classes, textContent, attributes));
    }
    static Elem(ns, type, classes = [], textContent = "", attributes = []) {
        let element = document.createElementNS(ns, type);
        if (classes) {
            for (const clazz of classes) {
                element.classList.add(clazz);
            }
        }
        let i;
        for (i = 0; i < attributes.length; i += 2) {
            element.setAttribute(attributes[i], attributes[i + 1]);
        }
        if (textContent) {
            element.textContent = textContent;
        }
        return element;
    }
    static InputNumber(table, min, max, key, cfg) {
        let value = 0;
        if (cfg != null) {
            for (const e of cfg) {
                if (e.key == key && !isNaN(e.value)) {
                    value = e.value;
                    break;
                }
            }
        }
        let tr = $.Html(table, "tr", ["develop-propertygrid-tr"]);
        $.Html(tr, "td", ["develop-propertygrid-td"], key);
        let inputContainer = $.Html(tr, "td", ["develop-propertygrid-td"]);
        return $.Html(inputContainer, "input", [], "", ["type", "number", "min", "" + Math.round(min), "max", "" + Math.round(max), "value", "" + Math.round(value),]);
    }
    static LabelSpanInput(parent, label, value, min, max) {
        let lbl = $.Html(parent, "label");
        $.Html(lbl, "span", [], label);
        return $.Html(lbl, "input", [], "", ["type", "number", "value", value + "", "min", min + "", "max", max + ""]);
    }
    static LabelSpanSelectFillEnum(parent, label, enumeration, defaultEnum) {
        const names = Object.keys(enumeration).filter(k => typeof enumeration[k] === "number");
        const numbers = names.map(k => enumeration[k]);
        let lbl = $.Html(parent, "label");
        $.Html(lbl, "span", [], label);
        let select = $.Html(lbl, "select");
        for (let i = 0; i < names.length; i++) {
            let option = $.Html(select, "option", [], names[i], ["value", numbers[i]]);
            if (i == defaultEnum)
                option.selected = true;
        }
        return select;
    }
    static InputSelect(table, displayValueAndKeyvalues, key, cfg) {
        let value = 0;
        if (cfg != null) {
            for (const e of cfg) {
                if (e.key == key && !isNaN(e.value)) {
                    value = e.value;
                    break;
                }
            }
        }
        let tr = $.Html(table, "tr", ["develop-propertygrid-tr"]);
        $.Html(tr, "td", ["develop-propertygrid-td"], key);
        let inputContainer = $.Html(tr, "td", ["develop-propertygrid-td"]);
        let select = $.Html(inputContainer, "select");
        for (let i = 0; i < displayValueAndKeyvalues.length; i++) {
            let option = $.Html(select, "option", [], displayValueAndKeyvalues[i].s, ["value", "" + displayValueAndKeyvalues[i].n]);
            if (i == value) {
                option.setAttribute("selected", "");
            }
        }
        return select;
    }
    static InputColor(table, key, cfg) {
        let value = "#ff0000";
        if (cfg != null) {
            for (const e of cfg) {
                if (e.key == key) {
                    value = e.value;
                    break;
                }
            }
        }
        let tr = $.Html(table, "tr", ["develop-propertygrid-tr"]);
        $.Html(tr, "td", ["develop-propertygrid-td"], key);
        let inputContainer = $.Html(tr, "td", ["develop-propertygrid-td"]);
        return $.Html(inputContainer, "input", [], "", ["type", "color", "value", value]);
    }
}
exports.$ = $;
$.SVGNS = "http://www.w3.org/2000/svg";
$.XLINKNS = "http://www.w3.org/1999/xlink";
$.HTMLNS = "http://www.w3.org/1999/xhtml";
//# sourceMappingURL=Utils.js.map