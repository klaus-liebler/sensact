import { AppManagement } from "./Interfaces";
import { ControlScreenController } from "./ControlScreenController";
import { ConfigScreenController } from "./ConfigScreenController";
import { ScreenController, ControllerState } from "./ScreenController";
import { $ } from "./Utils";


class AppController implements AppManagement {
  private screenControllers: ScreenController[];
  private activeControllerIndex: number;

  constructor() {

    this.screenControllers = [];
    this.activeControllerIndex = 0;
  }

  private setActiveScreen(newIndex: number) {
    if(newIndex==this.activeControllerIndex) return;
    this.screenControllers.forEach((controller, i) => {
      if (i == newIndex) {
        controller.showDIV();
        if (controller.State == ControllerState.CREATED) {
          controller.onFirstStart();
          controller.State = ControllerState.STARTED;
        }
        else {
          controller.onRestart();
          controller.State = ControllerState.STARTED;
        }
      } else {
        controller.hideDIV();
        if (controller.State == ControllerState.STARTED) {
          controller.onStop();
          controller.State = ControllerState.STOPPED;
        }
      }
    });
    this.activeControllerIndex = newIndex;
  }

  public startup() {
    let header = <HTMLElement>document.querySelector("header");
    $.Html(<HTMLElement>header, "button", ["small"], "CMD", ["type", "button"]).onclick = (e) => this.setActiveScreen(0);
    $.Html(<HTMLElement>header, "button", ["small"], "CFG", ["type", "button"]).onclick = (e) => this.setActiveScreen(1);
    
    this.screenControllers.push(new ControlScreenController(this,$.HtmlAfter(header, "main", ["ControlScreen"])));
    this.screenControllers.push(new ConfigScreenController(this,$.HtmlAfter(header, "main", ["ConfigScreen"])));
    this.setActiveScreen(0);
  }
}

let app: AppController;
document.addEventListener("DOMContentLoaded", (e) => {
  app = new AppController();
  app.startup();
});