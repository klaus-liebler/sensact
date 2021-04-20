import { AppManagement } from "./Interfaces";
import { IOScreenController } from "./IOScreenController";
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
    let outputsContollerContainer = $.HtmlAfter(header, "main", []);//<main data-screenId="default"></main>
    this.screenControllers.push(new IOScreenController(this,outputsContollerContainer));
    this.setActiveScreen(0);
  }
}

let app: AppController;
document.addEventListener("DOMContentLoaded", (e) => {
  app = new AppController();
  app.startup();
});