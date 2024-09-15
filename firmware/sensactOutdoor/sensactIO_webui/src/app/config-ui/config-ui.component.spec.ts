import { ComponentFixture, TestBed } from '@angular/core/testing';

import { ConfigUiComponent } from './config-ui.component';

describe('ConfigUiComponent', () => {
  let component: ConfigUiComponent;
  let fixture: ComponentFixture<ConfigUiComponent>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      declarations: [ ConfigUiComponent ]
    })
    .compileComponents();
  });

  beforeEach(() => {
    fixture = TestBed.createComponent(ConfigUiComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
