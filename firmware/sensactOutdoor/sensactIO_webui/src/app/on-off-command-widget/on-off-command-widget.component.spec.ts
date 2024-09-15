import { ComponentFixture, TestBed } from '@angular/core/testing';

import { OnOffCommandWidgetComponent } from './on-off-command-widget.component';

describe('OnOffCommandWidgetComponent', () => {
  let component: OnOffCommandWidgetComponent;
  let fixture: ComponentFixture<OnOffCommandWidgetComponent>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      declarations: [ OnOffCommandWidgetComponent ]
    })
    .compileComponents();
  });

  beforeEach(() => {
    fixture = TestBed.createComponent(OnOffCommandWidgetComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
