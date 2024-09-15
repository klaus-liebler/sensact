import { ComponentFixture, TestBed } from '@angular/core/testing';

import { CommandUiComponent } from './command-ui.component';

describe('CommandUiComponent', () => {
  let component: CommandUiComponent;
  let fixture: ComponentFixture<CommandUiComponent>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      declarations: [ CommandUiComponent ]
    })
    .compileComponents();
  });

  beforeEach(() => {
    fixture = TestBed.createComponent(CommandUiComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
