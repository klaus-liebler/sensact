import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import {CommandUiComponent} from './command-ui/command-ui.component'
import {ConfigUiComponent} from './config-ui/config-ui.component'
import { RouterModule, Routes } from '@angular/router';

const routes: Routes = [
  { path: 'config', component: ConfigUiComponent },
  { path: 'command', component: CommandUiComponent },
  { path: '', redirectTo: '/config', pathMatch: 'full' },
];

@NgModule({
  imports: [RouterModule.forRoot(routes)],
  exports: [RouterModule]
})
export class AppRoutingModule { }
