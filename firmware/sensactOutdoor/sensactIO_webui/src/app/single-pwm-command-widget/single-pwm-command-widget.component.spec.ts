import { ComponentFixture, TestBed } from '@angular/core/testing';

import { SinglePwmCommandWidgetComponent } from './single-pwmcommand-widget.component';

describe('SinglePwmCommandWidgetComponent', () => {
  let component: SinglePwmCommandWidgetComponent;
  let fixture: ComponentFixture<SinglePwmCommandWidgetComponent>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      declarations: [ SinglePwmCommandWidgetComponent ]
    })
    .compileComponents();
  });

  beforeEach(() => {
    fixture = TestBed.createComponent(SinglePwmCommandWidgetComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
