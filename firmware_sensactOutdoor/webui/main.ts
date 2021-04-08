import { AppManagement } from "./AppManagement";
import { BlindsAndLEDController } from "./BlindsAndLEDController";
import { ScreenController, ControllerState } from "./ScreenController";

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
    this.screenControllers.push(new BlindsAndLEDController(this, <HTMLElement>document.querySelector("main[data-screenId='default'")));
    this.setActiveScreen(0);
  }
}

let app: AppController;
document.addEventListener("DOMContentLoaded", (e) => {
  app = new AppController();
  app.startup();
});