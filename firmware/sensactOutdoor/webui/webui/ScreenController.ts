import { AppManagement } from "./Interfaces";

export enum ControllerState {
    CREATED,
    STARTED,
    STOPPED,
}

export abstract class ScreenController {
    private state: ControllerState;
    constructor(protected appManagement:AppManagement, protected container: HTMLElement) {
        this.hideDIV();
        this.state = ControllerState.CREATED;
    }
    get ElementId() { return this.container.id; }
    get State() { return this.state; }
    set State(value: ControllerState) { this.state = value; }
    abstract onCreate(): void;
    abstract onFirstStart(): void;
    abstract onRestart(): void;
    abstract onStop(): void;
    public showDIV() {
        this.container.style.display = "flex";
    }
    public hideDIV() {
        this.container.style.display = "none";
    }
}
