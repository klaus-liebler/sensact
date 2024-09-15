export class Utils
{
    public static EventCoordinatesInSVG(evt:MouseEvent, element:Element, positionRatio:number=1):Location2D {
        let rect = element.getBoundingClientRect();
        return {x: (evt.clientX - rect.left)/positionRatio, y:(evt.clientY - rect.top)/positionRatio}
    }
}

export interface Location2D {
    x: number;
    y: number;
}

export interface KeyValueTuple {
    key: string;
    value: any;
}

export class StringNumberTuple{
    public constructor(public s:string, public n:number){}
}


export class $
{
    
    public static readonly SVGNS = "http://www.w3.org/2000/svg";
    public static readonly XLINKNS = "http://www.w3.org/1999/xlink";
    public static readonly HTMLNS = "http://www.w3.org/1999/xhtml";

    public static Svg(parent: Element, type:string, classes?: string[],  attributes?:string[]):SVGElement {
        return  parent.appendChild(<SVGElement>$.Elem($.SVGNS, type, classes, "", attributes));
    }

    public static SvgIcon(parent: Element, iconname:string, classes:string[]=[]):SVGSVGElement
    {
        let svg = <SVGSVGElement>$.Svg(parent, "svg", classes, ["icon", "icon-"+iconname]);
        let use =$.Svg(svg, "use", [], []);
        use.setAttributeNS(this.XLINKNS, "href", "#icon-"+iconname);
        parent.appendChild(svg);
        return svg;
    }

    public static ColorNumColor2ColorDomString(num:number):string {
        let str = num.toString(16);
        while (str.length < 6) str = "0" + num;
        return "#"+str;
    }

    public static ColorDomString2ColorNum(colorString: string):number {
        return parseInt(colorString.substring(1), 16);
    }

    public static Html(parent: Element, type:string,  classes?: string[], textContent?:string, attributes?:string[]):HTMLElement {
        return parent.appendChild(<HTMLElement>$.Elem($.HTMLNS, type, classes, textContent, attributes));
    }

    public static HtmlAfter(previous: HTMLElement, type:string,  classes?: string[], textContent?:string, attributes?:string[]):HTMLElement {
        return previous.parentElement!.insertBefore(<HTMLElement>$.Elem($.HTMLNS, type, classes, textContent, attributes), previous.nextSibling);
    }

    public static HtmlAsFirstChild(parent: Element, type:string, classes?: string[], textContent?:string, attributes?:string[]):HTMLElement {
        if(parent.firstChild)
            return parent.insertBefore(<HTMLElement>$.Elem($.HTMLNS, type, classes, textContent, attributes), parent.firstChild);
        else
            return parent.appendChild(<HTMLElement>$.Elem($.HTMLNS, type, classes, textContent, attributes));
    }

    private static Elem(ns:string, type:string, classes: string[]=[], textContent:string="", attributes:string[]=[]):Element
    {
        let element = document.createElementNS(ns, type);
        if(classes)
        {
            for (const clazz of classes) {
                element.classList.add(clazz);
            }
        }
        let i:number;
        for(i=0;i<attributes.length;i+=2)
        {
            element.setAttribute(attributes[i], attributes[i+1]);
        }
        if(textContent)
        {
            element.textContent=textContent;
        }
        return element;
    }

    public static InputNumber(table:HTMLTableSectionElement, min:number, max:number, key:string, cfg:KeyValueTuple[]|null):HTMLInputElement
    {
        let value:number=0;
        if(cfg!=null)
        {
            for (const e of cfg) {
                if(e.key==key && !isNaN(e.value)){
                    value=e.value;
                    break;
                }
            }
        }
        let tr=$.Html(table, "tr", ["develop-propertygrid-tr"]);
        $.Html(tr, "td", ["develop-propertygrid-td"], key);
        let inputContainer = $.Html(tr, "td", ["develop-propertygrid-td"]);
        return <HTMLInputElement>$.Html(inputContainer, "input", [], "", ["type", "number", "min", ""+Math.round(min), "max", ""+Math.round(max), "value", ""+Math.round(value),]);
    }

    public static LabelSpanInput(parent:HTMLDivElement, label:string, value:number, min:number, max:number):HTMLInputElement{
        let lbl= <HTMLLabelElement>$.Html(parent, "label");
        $.Html(lbl, "span", [], label);
        return <HTMLInputElement>$.Html(lbl, "input", [], "", ["type", "number", "value", value+"", "min", min+"", "max", max+""]);
    }
    public static LabelSpanSelectFillEnum(parent:HTMLDivElement, label:string, enumeration:any, defaultEnum:number):HTMLSelectElement{
        const names= Object.keys(enumeration).filter(k => typeof enumeration[k as any] === "number");
        const numbers = names.map(k => enumeration[k as any]);
        let lbl= <HTMLLabelElement>$.Html(parent, "label");
        $.Html(lbl, "span", [], label);
        let select= <HTMLSelectElement>$.Html(lbl, "select");
        for(let i=0;i<names.length;i++){
            let option=<HTMLOptionElement>$.Html(select, "option", [], names[i], ["value", numbers[i]]);
            if(i==defaultEnum) option.selected=true;
        }
        return select;
    }

    public static InputSelect(table:HTMLTableSectionElement, displayValueAndKeyvalues:StringNumberTuple[], key:string, cfg:KeyValueTuple[]|null):HTMLSelectElement
    {
        let value:number=0;
        if(cfg!=null)
        {
            for (const e of cfg) {
                if(e.key==key && !isNaN(e.value)){
                    value=e.value;
                    break;
                }
            }
        }
        let tr=$.Html(table, "tr", ["develop-propertygrid-tr"]);
        $.Html(tr, "td", ["develop-propertygrid-td"], key);
        let inputContainer = $.Html(tr, "td", ["develop-propertygrid-td"]);
        let select = <HTMLSelectElement>$.Html(inputContainer, "select");
        for(let i=0;i<displayValueAndKeyvalues.length;i++)
        {
            let option = $.Html(select, "option", [], displayValueAndKeyvalues[i].s, ["value", ""+displayValueAndKeyvalues[i].n]);
            if(i==value){
                option.setAttribute("selected", "");
            }
        }
        return select;
    }

   

    public static InputColor(table:HTMLTableSectionElement, key:string, cfg:KeyValueTuple[]|null):HTMLInputElement
    {
        let value:string="#ff0000";
        if(cfg!=null)
        {
            for (const e of cfg) {
                if(e.key==key){
                    value=e.value;
                    break;
                }
            }
        }
        let tr=$.Html(table, "tr", ["develop-propertygrid-tr"]);
        $.Html(tr, "td", ["develop-propertygrid-td"], key);
        let inputContainer = $.Html(tr, "td", ["develop-propertygrid-td"]);
        return <HTMLInputElement>$.Html(inputContainer, "input", [], "", ["type", "color",  "value", value]);
    }
}