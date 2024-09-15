import { ComponentFixture, TestBed } from '@angular/core/testing';

import { BlindConfigWidgetComponent } from './blind-config-widget.component';

describe('BlindConfigWidgetComponent', () => {
  let component: BlindConfigWidgetComponent;
  let fixture: ComponentFixture<BlindConfigWidgetComponent>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      declarations: [ BlindConfigWidgetComponent ]
    })
    .compileComponents();
  });

  beforeEach(() => {
    fixture = TestBed.createComponent(BlindConfigWidgetComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
