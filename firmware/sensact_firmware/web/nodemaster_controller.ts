import { ScreenController } from "@klaus-liebler/web-components";
import { html, render } from "lit-html";
import { Ref, createRef, ref } from "lit-html/directives/ref.js";
import { classMap } from "lit-html/directives/class-map.js";

import { nodemaster } from "@generated/wsprotocol_ts/ws-protocol";
import { IAppManagement } from "@klaus-liebler/web-components/typescript/utils/interfaces";

export class NodemasterController extends ScreenController {

    protected OnRestart(): void {
        this.OnFirstStart();
    }

    OnPause(): void {
        this.stopInputPolling();
    }
    private inputRanges: Array<{start: number, end: number, name: string}> = [];
    private outputRanges: Array<{start: number, end: number, name: string}> = [];
    private inputValues: (number | null)[] = Array(32).fill(null);
    private outputValues: (number | null)[] = Array(32).fill(null);
    private inputSection: Ref<HTMLElement> = createRef();
    private outputSection: Ref<HTMLElement> = createRef();
    private modeSection: Ref<HTMLElement> = createRef();
    // true = Ein-/Ausgaenge werden von dieser Seite gesteuert (Sensact-Apps gestoppt), null = noch unbekannt
    private manualMode: boolean | null = null;
    private lastRequestedInputIndex: number = 0;
    private inputPollingInterval: NodeJS.Timeout | null = null;
    private currentRangeIndex: number = 0;
    private currentOffsetInRange: number = 0;

    public Template = () => html`
        <section ${ref(this.modeSection)}></section>
        <div class="buttons">
            <button class="withsvg" @click=${() => this.requestInputs()}>⟳ Eingänge aktualisieren</button>
            <button class="withsvg" @click=${() => this.requestOutputs()}>⟳ Ausgänge aktualisieren</button>
        </div>
        <h1>Eingänge</h1>
        <section ${ref(this.inputSection)}></section>
        <h1>Ausgänge</h1>
        <section ${ref(this.outputSection)}></section>
    `;

    public ModeTemplate = () => html`
        <div class="buttons">
            <span>Ein-/Ausgänge gesteuert von:</span>
            <label><input type="radio" name="nodemaster-mode" .checked=${this.manualMode === false} @change=${() => this.setMode(false)}/> Sensact-Apps</label>
            <label><input type="radio" name="nodemaster-mode" .checked=${this.manualMode === true} @change=${() => this.setMode(true)}/> dieser Seite (Apps gestoppt)</label>
        </div>
    `;

    private renderAll() {
        if (this.modeSection.value) render(this.ModeTemplate(), this.modeSection.value);
        if (this.inputSection.value) render(this.InputTableTemplate(), this.inputSection.value);
        if (this.outputSection.value) render(this.OutputTableTemplate(), this.outputSection.value);
    }

    private getInputRowClass = (idx: number) => {
        const value = this.inputValues[idx];
        return {
            'input-value-zero': value === 0,
            'input-value-active': value !== null && value !== undefined && value !== 0
        };
    };

    public InputTableTemplate = () => html`<div class="io-ranges">${this.inputRanges.map(({start, end, name}) => html`<div class="io-range">
            <h3>${name}</h3>
            <table class="autowidth">
                <thead><tr><th>Index</th><th>Wert</th><th>Aktion</th></tr></thead>
                <tbody>
                    ${Array.from({length: end - start}, (_, i) => start + i).map(idx => html`<tr><td>${idx}</td><td class=${classMap(this.getInputRowClass(idx))}>${this.inputValues[idx] ?? '-'}</td><td><button @click=${() => this.updateInput(idx)}>Lesen</button></td></tr>`)}
                </tbody>
            </table>
        </div>`)}</div>
    `;

public OutputTableTemplate = () => html`<div class="io-ranges">${this.outputRanges.map(({start, end, name}) => html`<div class="io-range">
            <h3>${name}</h3>
            <table class="autowidth">
                <thead><tr><th>Index</th><th>Wert</th><th>Aktionen</th></tr></thead>
                <tbody>
                    ${Array.from({length: end - start}, (_, i) => start + i).map(idx => {
                        const disabled = this.manualMode !== true;
                        return html`<tr><td>${idx}</td><td>${this.outputValues[idx] ?? '-'}</td><td><button ?disabled=${disabled} @click=${() => this.updateOutput(idx, 0)}>Aus</button><button ?disabled=${disabled} style="margin-left: 5px;" @click=${() => this.updateOutput(idx, 1)} title="Wert 1">Ein</button><button ?disabled=${disabled} style="margin-left: 15px;" @click=${(e: Event) => {
                            const input = (e.target as HTMLElement).parentElement?.querySelector('input[type="number"]') as HTMLInputElement;
                            const value = input ? Number(input.value) : 0;
                            this.updateOutput(idx, value);
                        }} title="Wert aus dem Feld daneben setzen (0..65535)">Setzen</button><input ?disabled=${disabled} type="number" min="0" max="65535" value="0" style="width:60px;"/></td></tr>`;
                    })}
                </tbody>
            </table>
        </div>`)}</div>
    `;


    OnMessage(namespaceId: number, messageTypeId: number, view: DataView): void {
        if (namespaceId != nodemaster.NAMESPACE_ID) return;

        switch (messageTypeId) {
            case nodemaster.ResponseInputs.TYPE_ID: {
                const ri = nodemaster.ResponseInputs.decode(view, 0);
                this.inputRanges = ri.ranges.map((range, i) => ({
                    start: range.start,
                    end: range.end,
                    name: range.name || `Bereich ${i}`
                }));
                this.currentRangeIndex = 0;
                this.currentOffsetInRange = 0;
                console.info(`Received input ranges: ${JSON.stringify(this.inputRanges)}`);
                if (this.inputSection.value) {
                    render(this.InputTableTemplate(), this.inputSection.value);
                }
                break;
            }
            case nodemaster.ResponseOutputs.TYPE_ID: {
                const ro = nodemaster.ResponseOutputs.decode(view, 0);
                this.outputRanges = ro.ranges.map((range, i) => ({
                    start: range.start,
                    end: range.end,
                    name: range.name || `Bereich ${i}`
                }));
                console.info(`Received output ranges: ${JSON.stringify(this.outputRanges)}`);
                if (this.outputSection.value) {
                    render(this.OutputTableTemplate(), this.outputSection.value);
                }
                break;
            }
            case nodemaster.ResponseSetOutput.TYPE_ID: {
                const rso = nodemaster.ResponseSetOutput.decode(view, 0);
                console.info(`Output set response: idx=${rso.index} val=${rso.value} ok=${rso.success}`);
                this.outputValues[rso.index] = rso.value;
                if (this.outputSection.value) {
                    render(this.OutputTableTemplate(), this.outputSection.value);
                }
                break;
            }
            case nodemaster.ResponseMode.TYPE_ID: {
                const rm = nodemaster.ResponseMode.decode(view, 0);
                this.manualMode = rm.manual;
                console.info(`Nodemaster IO mode: ${rm.manual ? 'manual' : 'apps'}`);
                this.renderAll();
                break;
            }
            case nodemaster.ResponseGetInput.TYPE_ID: {
                const rgi = nodemaster.ResponseGetInput.decode(view, 0);
                const idx = rgi.index;
                console.info(`Received input value at index ${idx}: ${rgi.value}`);

                // Update the value in our data store
                this.inputValues[idx] = rgi.value;

                // Re-render the table
                if (this.inputSection.value) {
                    render(this.InputTableTemplate(), this.inputSection.value);
                }
                break;
            }
        }
    }
    public constructor(appManagement: IAppManagement){
        super(appManagement)
    }


    OnCreate(): void {
        this.appManagement.RegisterNamespace(this, nodemaster.NAMESPACE_ID);
    }



    OnFirstStart(): void {
        this.queryMode();
        this.requestInputs();
        this.requestOutputs();
        this.startInputPolling();
    }

    private requestInputs() {
        const bytes = nodemaster.RequestInputs.encode({}, 0);
        this.appManagement.SendFrame(nodemaster.NAMESPACE_ID, bytes);
    }

    private requestOutputs() {
        const bytes = nodemaster.RequestOutputs.encode({}, 0);
        this.appManagement.SendFrame(nodemaster.NAMESPACE_ID, bytes);
    }

    private queryMode() {
        const bytes = nodemaster.RequestSetMode.encode({ manual: false, queryOnly: true }, 0);
        this.appManagement.SendFrame(nodemaster.NAMESPACE_ID, bytes);
    }

    private setMode(manual: boolean) {
        const bytes = nodemaster.RequestSetMode.encode({ manual, queryOnly: false }, 0);
        this.appManagement.SendFrame(nodemaster.NAMESPACE_ID, bytes);
    }

    private updateOutput(index: number, value: number) {
        const bytes = nodemaster.RequestSetOutput.encode({ index, value }, 0);
        this.appManagement.SendFrame(nodemaster.NAMESPACE_ID, bytes);
    }

  private updateInput(index: number) {
        console.info(`Requesting input value at index ${index}`);
        this.lastRequestedInputIndex = index;
        // Don't reset to null - keep the old value until new one arrives to avoid flickering
        const bytes = nodemaster.RequestGetInput.encode({ index }, 0);
        this.appManagement.SendFrame(nodemaster.NAMESPACE_ID, bytes);
    }

    private startInputPolling() {
        if (this.inputPollingInterval) return; // Already polling

        console.info('Starting input polling');
        this.currentRangeIndex = 0;
        this.currentOffsetInRange = 0;

        this.inputPollingInterval = setInterval(() => {
            const rangeCount = this.inputRanges.length;
            const range = this.inputRanges[this.currentRangeIndex];
            if (!range) return;
            const actualIndex = range.start + this.currentOffsetInRange;
            this.updateInput(actualIndex);
            const rangeLength = range.end - range.start;
            this.currentOffsetInRange++;
            if (this.currentOffsetInRange >= rangeLength) {
                this.currentOffsetInRange = 0;
                this.currentRangeIndex++;
                if (this.currentRangeIndex >= rangeCount) {
                    this.currentRangeIndex = 0;
                }
            }
        }, 500); // Poll every 500ms
    }

    private stopInputPolling() {
        if (this.inputPollingInterval) {
            clearInterval(this.inputPollingInterval);
            this.inputPollingInterval = null;
            console.info('Stopped input polling');
        }
    }

}
