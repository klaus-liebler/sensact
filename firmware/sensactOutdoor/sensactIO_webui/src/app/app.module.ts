import { NgModule } from '@angular/core';
import { BrowserModule } from '@angular/platform-browser';
import { HttpClientModule } from '@angular/common/http';
import { FormsModule } from '@angular/forms';

import { AppComponent } from './app.component';
import { SinglePwmCommandWidgetComponent } from './single-pwm-command-widget/single-pwmcommand-widget.component';
import { BlindCommandWidgetComponent } from './blind-command-widget/blind-command-widget.component';
import { OnOffCommandWidgetComponent } from './on-off-command-widget/on-off-command-widget.component';
import { AppRoutingModule } from './app-routing.module';
import { CommandUiComponent } from './command-ui/command-ui.component';
import { ConfigUiComponent } from './config-ui/config-ui.component';
import { BlindConfigWidgetComponent } from './blind-config-widget/blind-config-widget.component';
import { SinglePwmConfigWidgetComponent } from './single-pwm-config-widget/single-pwm-config-widget.component';
import { OnOffConfigWidgetComponent } from './on-off-config-widget/on-off-config-widget.component';
import { EnumToArrayPipe } from './enum-to-array.pipe';


@NgModule({
  declarations: [
    AppComponent,
    SinglePwmCommandWidgetComponent,
    BlindCommandWidgetComponent,
    OnOffCommandWidgetComponent,
    CommandUiComponent,
    ConfigUiComponent,
    BlindConfigWidgetComponent,
    SinglePwmConfigWidgetComponent,
    OnOffConfigWidgetComponent,
    EnumToArrayPipe,
  ],
  imports: [
    BrowserModule,
    FormsModule,
    HttpClientModule,
    AppRoutingModule,
  ],
  providers: [],
  bootstrap: [AppComponent]
})
export class AppModule { }
