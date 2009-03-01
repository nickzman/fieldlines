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


/* each drawn line is represented as a LineSegment */
typedef struct xpoint_s
{
    int x;
    int y;
} XPoint;

typedef struct {
    float red;
    float green;
    float blue;
} RGBcolor;

class ion{
public:
    float charge;
    float xyz[3];
    float vel[3];
    float angle;
    float anglevel;

    inline ion(int dSpeed, int min, int max);
    inline ~ion();
    inline void update(int dSpeed);
};


@interface FieldLinesView : ScreenSaverView {
    
    BOOL mainMonitorOnly;
    BOOL thisScreenIsOn;

    NSOpenGLView *_view;
    BOOL _viewAllocated;
    BOOL _initedGL;
    BOOL initialized;
    time_t targetTime;
    
    
    /****** non-user-modifiable immutable definitions */


    /* width and height of the window */
    int windowWidth;
    int windowHeight;

    /* center position of the window */
    int centerX;
    int centerY;

    RGBcolor *gcs;           /* array of colors */

    ion* ions;
    
    /* parameters that are user configurable */

    int dIons, ionsNo;
    IBOutlet id IBdIons;
    IBOutlet id IBdIonsTxt;

    int minCharge, maxCharge;
    IBOutlet id IBminCharge;
    IBOutlet id IBmaxCharge;
    IBOutlet id IBChargeTxt;
    
    int dSpeed;
    IBOutlet id IBdSpeed;
    IBOutlet id IBdSpeedTxt;

    BOOL dConstwidth;
    IBOutlet id IBdConstwidth;

    int dWidth;
    IBOutlet id IBdWidth;
    IBOutlet id IBdWidthTxt;

    int dStepSize;
    IBOutlet id IBdStepSize;
    IBOutlet id IBdStepSizeTxt;

    int dMaxSteps;
    IBOutlet id IBdMaxSteps;
    IBOutlet id IBdMaxStepsTxt;
    
    BOOL dElectric;
    IBOutlet id IBdElectric;

    int updateDelay;
    IBOutlet id IBupdateDelay;
    IBOutlet id IBupdateDelayTxt;

    int colorMode;
    IBOutlet id IBColorsTxt;
    IBOutlet id IBColors;
    // int maxcharge; // max charge in the system;
    // float minRep, maxRep;
    
    IBOutlet id configureSheet;
    IBOutlet id IBversionNumberField;
    IBOutlet id IBmainMonitorOnly;

    IBOutlet id IBCancel;
    IBOutlet id IBSave;
}

- (IBAction) closeSheet_save:(id) sender;
- (IBAction) closeSheet_cancel:(id) sender;
- (IBAction) sheet_update:(id) sender;

- (GLvoid) InitGL;

- (void) drawfieldline:(int) source the_x:(float) x the_y:(float) y the_z:(float) z;
- (void) setup_all;
@end
