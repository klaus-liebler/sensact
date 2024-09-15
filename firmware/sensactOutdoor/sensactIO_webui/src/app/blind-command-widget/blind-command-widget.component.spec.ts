import { ComponentFixture, TestBed } from '@angular/core/testing';

import { BlindCommandWidgetComponent } from './blind-command-widget.component';

describe('BlindCommandWidgetComponent', () => {
  let component: BlindCommandWidgetComponent;
  let fixture: ComponentFixture<BlindCommandWidgetComponent>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      declarations: [ BlindCommandWidgetComponent ]
    })
    .compileComponents();
  });

  beforeEach(() => {
    fixture = TestBed.createComponent(BlindCommandWidgetComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
