import { ComponentFixture, TestBed } from '@angular/core/testing';

import { SinglePwmConfigWidgetComponent } from './single-pwm-config-widget.component';

describe('SinglePwmConfigWidgetComponent', () => {
  let component: SinglePwmConfigWidgetComponent;
  let fixture: ComponentFixture<SinglePwmConfigWidgetComponent>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      declarations: [ SinglePwmConfigWidgetComponent ]
    })
    .compileComponents();
  });

  beforeEach(() => {
    fixture = TestBed.createComponent(SinglePwmConfigWidgetComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
