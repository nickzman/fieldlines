//
//  FieldLinesView.h
//  FieldLines
//
//  Created by pecos on Tue Oct 02 2001.
//  Copyright (c) 2001 __CompanyName__. All rights reserved.
//

#import <AppKit/AppKit.h>
#import <ScreenSaver/ScreenSaver.h>
#include <time.h>
#import <OpenGL/gl.h>
#import <OpenGL/glu.h>
#import "FieldLines.h"
#import "rsTimer.h"

@interface FieldLinesView : ScreenSaverView
{
	BOOL mainMonitor;
    BOOL mainMonitorOnly;
    BOOL thisScreenIsOn;

    NSOpenGLView *_view;
	BOOL _configuring;
    time_t targetTime;
    
	FieldLinesSaverSettings settings;
	rsTimer timer;
    
    /****** non-user-modifiable immutable definitions */

    IBOutlet id IBdIons;
    IBOutlet id IBdIonsTxt;

    IBOutlet id IBminCharge;
    IBOutlet id IBmaxCharge;
    IBOutlet id IBChargeTxt;
    
    IBOutlet id IBdSpeed;
    IBOutlet id IBdSpeedTxt;

    IBOutlet id IBdConstwidth;

    IBOutlet id IBdWidth;
    IBOutlet id IBdWidthTxt;

    IBOutlet id IBdStepSize;
    IBOutlet id IBdStepSizeTxt;

    IBOutlet id IBdMaxSteps;
    IBOutlet id IBdMaxStepsTxt;
    
    IBOutlet id IBdElectric;

    int updateDelay;
    IBOutlet id IBupdateDelay;
    IBOutlet id IBupdateDelayTxt;
    
    IBOutlet id configureSheet;
    IBOutlet id IBversionNumberField;
    IBOutlet id IBmainMonitorOnly;

    IBOutlet id IBCancel;
    IBOutlet id IBSave;
}

- (IBAction) closeSheet_save:(id) sender;
- (IBAction) closeSheet_cancel:(id) sender;
- (IBAction) sheet_update:(id) sender;

@end
