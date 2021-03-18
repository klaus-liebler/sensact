declare var iro:any;

class AppController{
    public startup(){
        let width=(<HTMLDivElement>document.querySelector("#hsvPicker")).offsetWidth;
        let colorPicker = new iro.ColorPicker('#hsvPicker',{width: Math.min(400, width)});
        let kelvinPicker = new iro.ColorPicker("#kelvinPicker", {
            width: Math.min(400, width),
            layoutDirection: 'vertical',
            layout: [
              {
                component: iro.ui.Slider,
                options: {
                  sliderType: 'kelvin',
                }
              },
              {
                component: iro.ui.Slider,
                options: {
                  sliderType: 'saturation',
                }
              },
            ]
          });
        let dimmerPicker = new iro.ColorPicker("#dimmerPicker", {
            width: Math.min(400, width),
            layoutDirection: 'vertical',
            color: "#000",
            layout: [
              {
                component: iro.ui.Slider,
                options: {
                  sliderType: 'value',
                }
              },
            ]
          });
        
    }
}

let app: AppController;
document.addEventListener("DOMContentLoaded", (e) => {
    app = new AppController();
    app.startup();
});