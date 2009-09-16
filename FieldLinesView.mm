//
//  FieldLinesView.mm
//  FieldLines
//
//  Created by pecos on Tue Oct 02 2001.
//  Copyright (c) 2001 __CompanyName__. All rights reserved.
//

/*
* Copyright (C) 2002  Terence M. Welsh
 *
 * Field Lines is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as 
 * published by the Free Software Foundation.
 *
 * Field Lines is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


// Field Lines screensaver

#import "FieldLinesView.h"
#include <stdlib.h>
#include <time.h>
#import <GLUT/glut.h>
#import <OpenGL/OpenGL.h>

//#define kVersion	@"1.1.0"
#define kCurrentVersionsFile @"http://spazioinwind.libero.it/tpecorella/uselesssoft/saversVersions.plist"

#define PIx2 6.28318530718f
#define wide 200
#define high 150


// #define LOG_DEBUG


@implementation FieldLinesView

- (id)initWithFrame:(NSRect)frameRect isPreview:(BOOL) preview
{
    NSString* version;

    ScreenSaverDefaults *defaults = [ScreenSaverDefaults defaultsForModuleWithName:@"fieldlines"];

#ifdef LOG_DEBUG
    NSLog( @"initWithFrame" );
#endif

    if (![super initWithFrame:frameRect isPreview:preview]) return nil;

    if (self) {
        NSOpenGLPixelFormatAttribute attribs[] =
    {	NSOpenGLPFAAccelerated, (NSOpenGLPixelFormatAttribute)1,
        // NSOpenGLPFADepthSize, (NSOpenGLPixelFormatAttribute)24,
        NSOpenGLPFAColorSize, (NSOpenGLPixelFormatAttribute)16,
        NSOpenGLPFAMinimumPolicy, (NSOpenGLPixelFormatAttribute)1,
        NSOpenGLPFAMaximumPolicy, (NSOpenGLPixelFormatAttribute)1,
        // NSOpenGLPFAClosestPolicy,
        (NSOpenGLPixelFormatAttribute)0
    };

        NSOpenGLPixelFormat *format = [[[NSOpenGLPixelFormat alloc] initWithAttributes:attribs] autorelease];

        _view = [[[NSOpenGLView alloc] initWithFrame:NSZeroRect pixelFormat:format] autorelease];

        [self addSubview:_view];
        _viewAllocated = TRUE;
        _initedGL = NO;
    }
	NSString *kVersion = [[[NSBundle bundleForClass:[self class]] infoDictionary] objectForKey:@"CFBundleShortVersionString"];

    // Do your subclass initialization here
    version   = [defaults stringForKey:@"version"];
    mainMonitorOnly = [defaults boolForKey:@"mainMonitorOnly"];
    dIons = int([defaults integerForKey:@"dIons"]);
    dStepSize = int([defaults integerForKey:@"dStepSize"]);
    dMaxSteps = int([defaults integerForKey:@"dMaxSteps"]);
    dWidth = int([defaults integerForKey:@"dWidth"]);
    dSpeed = int([defaults integerForKey:@"dSpeed"]);
    dConstwidth = [defaults boolForKey:@"dConstwidth"];
    dElectric = [defaults boolForKey:@"dElectric"];
    minCharge = int([defaults integerForKey:@"minCharge"]);
    maxCharge = int([defaults integerForKey:@"maxCharge"]);
    if( minCharge < 1 ) minCharge = 1;
    if( maxCharge < minCharge ) maxCharge = minCharge + 1;
    updateDelay = int([defaults integerForKey:@"updateDelay"]);
    colorMode = int([defaults integerForKey:@"colorMode"]);
        
    if( ![version isEqualToString:kVersion] || (version == NULL) ) {
        // first time ever !! 
        [defaults setObject: kVersion forKey: @"version"];
        [defaults setBool: NO forKey: @"mainMonitorOnly"];
        [defaults setInteger: 4 forKey: @"dIons"];
        [defaults setInteger: 15 forKey: @"dStepSize"];
        [defaults setInteger: 100 forKey: @"dMaxSteps"];
        [defaults setInteger: 30 forKey: @"dWidth"];
        [defaults setInteger: 10 forKey: @"dSpeed"];
        [defaults setBool: NO forKey: @"dConstwidth"];
        [defaults setBool: NO forKey: @"dElectric"];
        [defaults setInteger: 1 forKey: @"minCharge"];
        [defaults setInteger: 2 forKey: @"maxCharge"];
        [defaults setInteger: 5*60 forKey:@"updateDelay"];
        [defaults setInteger: 0 forKey:@"colorMode"];
        
        [defaults synchronize];

        mainMonitorOnly = NO;
        dIons = 4;
        dStepSize = 15;
        dMaxSteps = 100;
        dWidth = 30;
        dSpeed = 10;
        dConstwidth = NO;
        dElectric = NO;
        minCharge = 1;
        maxCharge = 2;
        updateDelay = 5*60;
        colorMode = 0;
    }
    
    windowWidth = int([self bounds].size.width);
    windowHeight = int([self bounds].size.height);
    centerX = windowWidth / 2;
    centerY = windowHeight / 2;
    
    thisScreenIsOn = TRUE;
    initialized = NO;
    
    return self;
}

- (void)animateOneFrame
{
    // Do your animation stuff here.
    // If you want to use drawRect: just add setNeedsDisplay:YES to this method

    int i;
    float s = sqrtf(float(dStepSize)*float(dStepSize) * 0.333f);

    if( thisScreenIsOn == FALSE ) {
        [self stopAnimation];
        return;
    }

    [[_view openGLContext] makeCurrentContext];

    if (!_initedGL) {
        [self InitGL];
        _initedGL = YES;
    }

    if( (!initialized) || (updateDelay && (targetTime < time(0))) ) {
        [self setup_all];
        initialized = TRUE;
    }
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for(i=0; i<ionsNo; i++)
        ions[i].update(dSpeed);

    for(i=0; i<ionsNo; i++){
        [self drawfieldline:i the_x: s the_y: s the_z: s];
        [self drawfieldline:i the_x: s the_y: s the_z:-s];
        [self drawfieldline:i the_x: s the_y:-s the_z: s];
        [self drawfieldline:i the_x: s the_y:-s the_z:-s];
        [self drawfieldline:i the_x:-s the_y: s the_z: s];
        [self drawfieldline:i the_x:-s the_y: s the_z:-s];
        [self drawfieldline:i the_x:-s the_y:-s the_z: s];
        [self drawfieldline:i the_x:-s the_y:-s the_z:-s];
    }

    glFlush();
}

- (void)startAnimation
{
    // Do your animation initialization here
    NSOpenGLContext *context;
	GLint interval = 1;

    int mainScreen;
    int thisScreen;
    
#ifdef LOG_DEBUG
    NSLog( @"startAnimation" );
#endif
    
    thisScreenIsOn = TRUE;
    if( mainMonitorOnly ) {
        thisScreen = [[[[[self window] screen] deviceDescription] objectForKey:@"NSScreenNumber"] intValue];
        mainScreen = [[[[NSScreen mainScreen] deviceDescription] objectForKey:@"NSScreenNumber"] intValue];
        // NSLog( @"test this %d - main %d", thisScreen, mainScreen );
        if( thisScreen != mainScreen ) {
            thisScreenIsOn = FALSE;
        }
    }

    context = [_view openGLContext];
    [context makeCurrentContext];
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glFlush();
	CGLSetParameter(CGLGetCurrentContext(), kCGLCPSwapInterval, &interval);	// don't allow screen tearing
    [super startAnimation];
}

- (void)stopAnimation
{
    // Do your animation termination here
    
    [super stopAnimation];
}

- (BOOL) hasConfigureSheet
{
    // Return YES if your screensaver has a ConfigureSheet
    return YES;
}

- (NSWindow*)configureSheet
{
    NSBundle *thisBundle = [NSBundle bundleForClass:[self class]];
	NSString *kVersion = [[thisBundle infoDictionary] objectForKey:@"CFBundleShortVersionString"];

#ifdef LOG_DEBUG
    NSLog( @"configureSheet" );
#endif
    if( ! configureSheet ) [NSBundle loadNibNamed:@"FieldLines" owner:self];
    // [NSBundle loadNibNamed:@"Localizable" owner:self];
    
    [IBversionNumberField setStringValue:kVersion];

    [IBmainMonitorOnly setState:(mainMonitorOnly ? NSOnState : NSOffState)];
    
    [IBdIons setIntValue:dIons];
    [IBdIonsTxt setStringValue:[NSString stringWithFormat:
        [thisBundle localizedStringForKey:@"Ions number (%d)" value:@"" table:@""], dIons]];

    [IBdSpeed setIntValue:dSpeed];
    [IBdSpeedTxt setStringValue:[NSString stringWithFormat:
        [thisBundle localizedStringForKey:@"Speed (%d)" value:@"" table:@""], dSpeed]];
    
    [IBdConstwidth setState:(dConstwidth ? NSOnState : NSOffState)];
    
    [IBdWidth setIntValue:dWidth];
    [IBdWidthTxt setStringValue:[NSString stringWithFormat:
        [thisBundle localizedStringForKey:@"Line Width (%d)" value:@"" table:@""], dWidth]];

    [IBdStepSize setIntValue:dStepSize];
    [IBdStepSizeTxt setStringValue:[NSString stringWithFormat:
        [thisBundle localizedStringForKey:@"Step size (%d)" value:@"" table:@""], dStepSize]];

    [IBdMaxSteps setIntValue:dMaxSteps];
    [IBdMaxStepsTxt setStringValue:[NSString stringWithFormat:
        [thisBundle localizedStringForKey:@"Max Steps (%d)" value:@"" table:@""], dMaxSteps]];

    [IBdElectric setState:(dElectric ? NSOnState : NSOffState)];

    [IBminCharge setIntValue:minCharge];
    [IBmaxCharge setIntValue:maxCharge];
    [IBChargeTxt setStringValue:[NSString stringWithFormat:
        [thisBundle localizedStringForKey:@"Charge value (%d-%d)" value:@"" table:@""], minCharge, maxCharge]];

    [IBupdateDelay setIntValue:updateDelay/60];
    if( updateDelay ) {
        [IBupdateDelayTxt setStringValue:[NSString stringWithFormat:
            [thisBundle localizedStringForKey:@"Refresh after %d min." value:@"" table:@""], updateDelay/60]];
    }
    else {
        [IBupdateDelayTxt setStringValue:
            [thisBundle localizedStringForKey:@"Refresh: never" value:@"" table:@""]];
    }
    
    [IBdConstwidth setTitle:[thisBundle localizedStringForKey:@"Constant Width" value:@"" table:@""]];
    [IBdElectric setTitle:[thisBundle localizedStringForKey:@"Electric mode" value:@"" table:@""]];

    [IBColorsTxt setStringValue:
        [thisBundle localizedStringForKey:@"Colors" value:@"" table:@""]];
    [IBColors removeAllItems];
    [IBColors addItemWithTitle:
        [thisBundle localizedStringForKey:@"Original" value:@"" table:@""]];
    [IBColors addItemWithTitle:
        [thisBundle localizedStringForKey:@"Electric potential" value:@"" table:@""]];
    [IBColors selectItemAtIndex:colorMode];
    
    [IBmainMonitorOnly setTitle:[thisBundle localizedStringForKey:@"Main monitor only" value:@"" table:@""]];
    [IBCancel setTitle:[thisBundle localizedStringForKey:@"Cancel" value:@"" table:@""]];
    [IBSave setTitle:[thisBundle localizedStringForKey:@"Save" value:@"" table:@""]];
    
    return configureSheet;
}

- (IBAction) sheet_update:(id) sender
{
    int fooInt;
    NSBundle *thisBundle = [NSBundle bundleForClass:[self class]];

    if( sender == IBdIons ) {
        fooInt = [IBdIons intValue];
        if( fooInt == 0 ) fooInt = 1;
        [IBdIonsTxt setStringValue:[NSString stringWithFormat:
            [thisBundle localizedStringForKey:@"Ions number (%d)" value:@"" table:@""], fooInt]];
    }
    else if( sender == IBdSpeed ) {
        fooInt = [IBdSpeed intValue];
        if( fooInt == 0 ) fooInt = 1;
        [IBdSpeedTxt setStringValue:[NSString stringWithFormat:
            [thisBundle localizedStringForKey:@"Speed (%d)" value:@"" table:@""], fooInt]];
    }
    else if( sender == IBdWidth ) {
        fooInt = [IBdWidth intValue];
        if( fooInt == 0 ) fooInt = 1;
        [IBdWidthTxt setStringValue:[NSString stringWithFormat:
            [thisBundle localizedStringForKey:@"Line Width (%d)" value:@"" table:@""], fooInt]];
    }
    else if( sender == IBdStepSize ) {
        fooInt = [IBdStepSize intValue];
        if( fooInt == 0 ) fooInt = 1;
        [IBdStepSizeTxt setStringValue:[NSString stringWithFormat:
            [thisBundle localizedStringForKey:@"Step size (%d)" value:@"" table:@""], fooInt]];
    }
    else if( sender == IBdMaxSteps ) {
        fooInt = [IBdMaxSteps intValue];
        if( fooInt == 0 ) fooInt = 1;
        [IBdMaxStepsTxt setStringValue:[NSString stringWithFormat:
            [thisBundle localizedStringForKey:@"Max Steps (%d)" value:@"" table:@""], fooInt]];
    }
    else if( sender == IBminCharge ) {
        fooInt = [IBminCharge intValue];
		if (fooInt >= int([IBminCharge maxValue]))
		{
			fooInt = int([IBminCharge maxValue])-1;
			[IBminCharge setIntValue:fooInt];
		}
        if( fooInt >= [IBmaxCharge intValue] )
            [IBmaxCharge setIntValue:fooInt+1];
        [IBChargeTxt setStringValue:[NSString stringWithFormat:
            [thisBundle localizedStringForKey:@"Charge value (%d-%d)" value:@"" table:@""], fooInt, [IBmaxCharge intValue]]];
    }
    else if( sender == IBmaxCharge ) {
        fooInt = [IBmaxCharge intValue];
		if (fooInt <= int([IBmaxCharge minValue]))
		{
			fooInt = int([IBmaxCharge minValue])+1;
			[IBmaxCharge setIntValue:fooInt];
		}
        if( fooInt <= [IBminCharge intValue] )
            [IBminCharge setIntValue:fooInt-1];
        [IBChargeTxt setStringValue:[NSString stringWithFormat:
            [thisBundle localizedStringForKey:@"Charge value (%d-%d)" value:@"" table:@""], [IBminCharge intValue], fooInt]];
    }
    else if( sender == IBupdateDelay ) {
        fooInt = [IBupdateDelay intValue];
        if( fooInt ) {
            [IBupdateDelayTxt setStringValue:[NSString stringWithFormat:
                [thisBundle localizedStringForKey:@"Refresh after %d min." value:@"" table:@""], fooInt/60]];
        }
        else {
            [IBupdateDelayTxt setStringValue:
                [thisBundle localizedStringForKey:@"Refresh: never" value:@"" table:@""]];
        }
    }
}

- (IBAction) closeSheet_save:(id) sender {

    int thisScreen;
    int mainScreen;
    
    ScreenSaverDefaults *defaults = [ScreenSaverDefaults defaultsForModuleWithName:@"fieldlines"];
    
#ifdef LOG_DEBUG
    NSLog( @"closeSheet_save" );
#endif
    
    mainMonitorOnly = ( [IBmainMonitorOnly state] == NSOnState ) ? true : false;

    dIons = [IBdIons intValue];
    dSpeed = [IBdSpeed intValue];
    dConstwidth = ( [IBdConstwidth state] == NSOnState ) ? true : false;
    dWidth = [IBdWidth intValue];
    dStepSize = [IBdStepSize intValue];
    dMaxSteps = [IBdMaxSteps intValue];
    dElectric = ( [IBdElectric state] == NSOnState ) ? true : false;
    minCharge = [IBminCharge intValue];
    maxCharge = [IBmaxCharge intValue];
    updateDelay = [IBupdateDelay intValue]*60;
    colorMode = int([IBColors indexOfSelectedItem]);
        
    [defaults setBool: mainMonitorOnly forKey: @"mainMonitorOnly"];
    [defaults setInteger: dIons forKey: @"dIons"];
    [defaults setInteger: dSpeed forKey: @"dSpeed"];
    [defaults setBool: dConstwidth forKey: @"dConstwidth"];
    [defaults setInteger: dWidth forKey: @"dWidth"];
    [defaults setInteger: dStepSize forKey: @"dStepSize"];
    [defaults setInteger: dMaxSteps forKey: @"dMaxSteps"];
    [defaults setBool: dElectric forKey: @"dElectric"];
    [defaults setInteger: minCharge forKey: @"minCharge"];
    [defaults setInteger: maxCharge forKey: @"maxCharge"];
    [defaults setInteger: updateDelay forKey: @"updateDelay"];
    [defaults setInteger: colorMode forKey: @"colorMode"];
    
    [defaults synchronize];

#ifdef LOG_DEBUG
    NSLog(@"Canged params" );
#endif

    initialized = NO;

    if( mainMonitorOnly ) {
        thisScreen = [[[[[self window] screen] deviceDescription] objectForKey:@"NSScreenNumber"] intValue];
        mainScreen = [[[[NSScreen mainScreen] deviceDescription] objectForKey:@"NSScreenNumber"] intValue];
        // NSLog( @"test this %d - main %d", thisScreen, mainScreen );
        if( thisScreen != mainScreen ) {
            thisScreenIsOn = FALSE;
        }
    }
    if( (thisScreenIsOn == FALSE) && (mainMonitorOnly == FALSE) ) {
        thisScreenIsOn = TRUE;
        [self startAnimation];
    }
    
    [NSApp endSheet:configureSheet];
}

- (IBAction) closeSheet_cancel:(id) sender {

#ifdef LOG_DEBUG
    NSLog( @"closeSheet_cancel" );
#endif
    
    [NSApp endSheet:configureSheet];
}

- (void) dealloc {

#ifdef LOG_DEBUG
    NSLog( @"dealloc" );
#endif

    if( ions ) {
        delete[] ions;
        ions = 0;
    }
    
    [super dealloc];
}


// InitGL ---------------------------------------------------------------------

- (GLvoid) InitGL
{

    // glViewport(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
    glViewport( 0, 0, windowWidth, windowHeight );

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glShadeModel(GL_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_COLOR);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // gluPerspective(80.0, float(rect.right) / float(rect.bottom), 50, 3000);
    gluPerspective(80.0, (GLfloat)windowWidth/(GLfloat)windowHeight, 50, 3000);
    glTranslatef(0.0f, 0.0f, -(wide * 2));
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if(dConstwidth)
        glLineWidth(dWidth * 0.1f);
}



- (void) setFrameSize:(NSSize)newSize
{
    [super setFrameSize:newSize];
    if( _viewAllocated )
        [_view setFrameSize:newSize];
    _initedGL = NO;
}

- (void) drawfieldline:(int) source the_x:(float) x the_y:(float) y the_z:(float) z
{
    int i, j;
    float charge;
    float repulsion;
    float dist, distsquared, distrec;
    float xyz[3];
    float lastxyz[3];
    float dir[3];
    float end[3];
    float tempvec[3];
    float r, g, b;
    float lastr, lastg, lastb;
    static float brightness = 10000.0f;
    // NSColor* theColor;
    // float chargePot;
    
    if( ions[source].charge > 0 )
        charge = 1;
    else
        charge = -1;
    
    lastxyz[0] = ions[source].xyz[0];
    lastxyz[1] = ions[source].xyz[1];
    lastxyz[2] = ions[source].xyz[2];
    dir[0] = x;
    dir[1] = y;
    dir[2] = z;

    // Do the first segment
    // if( colorMode == 0 ) {
        r = fabsf(dir[2]) * brightness;
        g = fabsf(dir[0]) * brightness;
        b = fabsf(dir[1]) * brightness;
        if(r > 1.0f)
            r = 1.0f;
        if(g > 1.0f)
            g = 1.0f;
        if(b > 1.0f)
            b = 1.0f;
    /* }
    else {
        float hue;
        if( charge > 0 )	hue = 0.0;
        else 			hue = 225.0/360.0;
        theColor = [NSColor colorWithCalibratedHue:hue
                                        saturation:1.0
                                        brightness:1.0
                                             alpha:1.0];
        r = [theColor redComponent];
        g = [theColor greenComponent];
        b = [theColor blueComponent];
    } */
    lastr = r;
    lastg = g;
    lastb = b;
    glColor3f(r, g, b);
    xyz[0] = lastxyz[0] + dir[0];
    xyz[1] = lastxyz[1] + dir[1];
    xyz[2] = lastxyz[2] + dir[2];
    if(dElectric){
        xyz[0] += float(SSRandomFloatBetween(0.0f, dStepSize * 0.3f)) - (dStepSize * 0.15f);
        xyz[1] += float(SSRandomFloatBetween(0.0f, dStepSize * 0.3f)) - (dStepSize * 0.15f);
        xyz[2] += float(SSRandomFloatBetween(0.0f, dStepSize * 0.3f)) - (dStepSize * 0.15f);
    }
    if(!dConstwidth)
        glLineWidth((xyz[2] + 300.0f) * 0.000333f * dWidth);
    glBegin(GL_LINE_STRIP);
    glColor3f(lastr, lastg, lastb);
    glVertex3fv(lastxyz);
    glColor3f(r, g, b);
    glVertex3fv(xyz);
    if(!dConstwidth)
        glEnd();

    for(i=0; i<dMaxSteps; i++){
        // chargePot = 0;
        dir[0] = 0.0f;
        dir[1] = 0.0f;
        dir[2] = 0.0f;
        for(j=0; j<dIons; j++){
            repulsion = charge * ions[j].charge;
            tempvec[0] = xyz[0] - ions[j].xyz[0];
            tempvec[1] = xyz[1] - ions[j].xyz[1];
            tempvec[2] = xyz[2] - ions[j].xyz[2];
            distsquared = tempvec[0] * tempvec[0] + tempvec[1] * tempvec[1] + tempvec[2] * tempvec[2];
            dist = sqrtf(distsquared);
            if(dist < dStepSize && i > 2){
                end[0] = ions[j].xyz[0];
                end[1] = ions[j].xyz[1];
                end[2] = ions[j].xyz[2];
                i = 10000;
            }
            repulsion /= dist * distsquared;
            // chargePot += repulsion*dist;
            dir[0] += tempvec[0] * repulsion;
            dir[1] += tempvec[1] * repulsion;
            dir[2] += tempvec[2] * repulsion;
        }
        lastr = r;
        lastg = g;
        lastb = b;

        // if( colorMode == 0 ) {
            r = fabsf(dir[2]) * brightness;
            g = fabsf(dir[0]) * brightness;
            b = fabsf(dir[1]) * brightness;
        /* }
        else {
            float hue;
            hue = fabs(chargePot);
            // hue = -pow(atan(chargePot),1/4.)/M_PI + 0.5;
            // hue *= 225.0/360.0;
            
            if( hue < minRep )
                minRep = hue;
            if( hue > maxRep )
                maxRep = hue;

            hue /= maxRep;
            
            if( hue > 1 )
                hue = 0.5;
            
            theColor = [NSColor colorWithCalibratedHue:hue
                                            saturation:1.0
                                            brightness:1.0
                                                 alpha:1.0];
            r = [theColor redComponent];
            g = [theColor greenComponent];
            b = [theColor blueComponent];
        } */
        
        if(dElectric){
            r *= 10.0f;
            g *= 10.0f;
            b *= 10.0f;;
            if(r > b * 0.5f)
                r = b * 0.5f;
            if(g > b * 0.3f)
                g = b * 0.3f;
        }
        if(r > 1.0f)
            r = 1.0f;
        if(g > 1.0f)
            g = 1.0f;
        if(b > 1.0f)
            b = 1.0f;
        distsquared = dir[0] * dir[0] + dir[1] * dir[1] + dir[2] * dir[2];
        distrec = dStepSize / sqrtf(distsquared);
        dir[0] *= distrec;
        dir[1] *= distrec;
        dir[2] *= distrec;
        if(dElectric){
            dir[0] += float(SSRandomFloatBetween(0.0f, dStepSize)) - (dStepSize * 0.5f);
            dir[1] += float(SSRandomFloatBetween(0.0f, dStepSize)) - (dStepSize * 0.5f);
            dir[2] += float(SSRandomFloatBetween(0.0f, dStepSize)) - (dStepSize * 0.5f);
        }
        lastxyz[0] = xyz[0];
        lastxyz[1] = xyz[1];
        lastxyz[2] = xyz[2];
        xyz[0] += dir[0];
        xyz[1] += dir[1];
        xyz[2] += dir[2];
        if(!dConstwidth){
            glLineWidth((xyz[2] + 300.0f) * 0.000333f * dWidth);
            glBegin(GL_LINE_STRIP);
        }
        glColor3f(lastr, lastg, lastb);
        glVertex3fv(lastxyz);
        if(i != 10000){
            if(i == (dMaxSteps - 1))
                glColor3f(0.0f, 0.0f, 0.0f);
            else
                glColor3f(r, g, b);
            glVertex3fv(xyz);
            if(i == (dMaxSteps - 1))
                glEnd();
        }
    }
    if(i == 10001){
        glColor3f(r, g, b);
        glVertex3fv(end);
        glEnd();
    }
}

- (void) setup_all
{
    srandom(int(time(NULL)));
    
    if( ions ) {
        delete[] ions;
        ions = 0;
    }
    ionsNo = dIons;
    if (minCharge >= maxCharge)	// previous versions of this screen saver had a zero divide bug that occurred if minCharge and maxCharge were equal; don't allow that to happen
		maxCharge++;
    ions = new ion[dIons](dSpeed, minCharge, maxCharge);

    targetTime = time(0) + updateDelay;
}

@end

ion::ion(int dSpeed, int min, int max){

    charge = (int)random() % 2;
    if(charge == 0.0f)
        charge = -1.0f;
    int foo = min + ((int)random() % (max-min));
    charge *= foo;
    xyz[0] = float(random()) / RAND_MAX * 2.0f * float(wide) - float(wide);
    xyz[1] = float(random()) / RAND_MAX * 2.0f * float(high) - float(high);
    xyz[2] = float(random()) / RAND_MAX * 2.0f * float(wide) - float(wide);
    vel[0] = float(random()) / RAND_MAX * float(dSpeed) * 0.1f - (float(dSpeed) * 0.05f);
    vel[1] = float(random()) / RAND_MAX * float(dSpeed) * 0.1f - (float(dSpeed) * 0.05f);
    vel[2] = float(random()) / RAND_MAX * float(dSpeed) * 0.1f - (float(dSpeed) * 0.05f);
    angle = 0.0f;
    anglevel = 0.0005f * float(dSpeed) + 0.0005f * float(random()) / RAND_MAX * float(dSpeed);
}

ion::~ion(){
}

void ion::update(int dSpeed){
    xyz[0] += vel[0];
    xyz[1] += vel[1];
    xyz[2] += vel[2];
    if(xyz[0] > wide)
        vel[0] -= 0.001f * float(dSpeed);
    if(xyz[0] < -wide)
        vel[0] += 0.001f * float(dSpeed);
    if(xyz[1] > high)
        vel[1] -= 0.001f * float(dSpeed);
    if(xyz[1] < -high)
        vel[1] += 0.001f * float(dSpeed);
    if(xyz[2] > wide)
        vel[2] -= 0.001f * float(dSpeed);
    if(xyz[2] < -wide)
        vel[2] += 0.001f * float(dSpeed);

    angle += anglevel;
    if(angle > PIx2)
        angle -= PIx2;
}
