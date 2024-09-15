import { ComponentFixture, TestBed } from '@angular/core/testing';

import { OnOffConfigWidgetComponent } from './on-off-config-widget.component';

describe('OnOffConfigWidgetComponent', () => {
  let component: OnOffConfigWidgetComponent;
  let fixture: ComponentFixture<OnOffConfigWidgetComponent>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      declarations: [ OnOffConfigWidgetComponent ]
    })
    .compileComponents();
  });

  beforeEach(() => {
    fixture = TestBed.createComponent(OnOffConfigWidgetComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
