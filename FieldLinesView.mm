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

@implementation FieldLinesView

- (id)initWithFrame:(NSRect)frameRect isPreview:(BOOL) preview
{
    ScreenSaverDefaults *defaults = [ScreenSaverDefaults defaultsForModuleWithName:@"fieldlines"];

#ifdef LOG_DEBUG
    NSLog( @"initWithFrame" );
#endif
	
	self = [super initWithFrame:frameRect isPreview:preview];
	if (self)
	{
		if (preview)
			mainMonitor = YES;
		else
			mainMonitor = frameRect.origin.x == 0.0 && frameRect.origin.y == 0;
		mainMonitorOnly = [defaults boolForKey:@"mainMonitorOnly"];
		if (mainMonitor || !mainMonitorOnly)
		{
			NSOpenGLPixelFormatAttribute attribs[] =
			{
				NSOpenGLPFAAccelerated,
				NSOpenGLPFAColorSize, (NSOpenGLPixelFormatAttribute)32,
				NSOpenGLPFADoubleBuffer,
				NSOpenGLPFAMinimumPolicy,
				NSOpenGLPFADepthSize, (NSOpenGLPixelFormatAttribute)16,
				NSOpenGLPFAAllowOfflineRenderers,
				(NSOpenGLPixelFormatAttribute)0
			};
			
			NSOpenGLPixelFormat *format = [[NSOpenGLPixelFormat alloc] initWithAttributes:attribs];
			
			_view = [[NSOpenGLView alloc] initWithFrame:NSZeroRect pixelFormat:format];
			_view.wantsBestResolutionOpenGLSurface = YES;
			[self addSubview:_view];
			
			[_view.openGLContext makeCurrentContext];
			setDefaults(&settings);
			if ([defaults objectForKey:@"dIons"])
			{
				settings.dIons = int([defaults integerForKey:@"dIons"]);
				settings.dStepSize = int([defaults integerForKey:@"dStepSize"]);
				settings.dMaxSteps = int([defaults integerForKey:@"dMaxSteps"]);
				settings.dWidth = int([defaults integerForKey:@"dWidth"]);
				settings.dSpeed = int([defaults integerForKey:@"dSpeed"]);
				settings.dConstwidth = [defaults boolForKey:@"dConstwidth"];
				settings.dElectric = [defaults boolForKey:@"dElectric"];
				settings.minCharge = int([defaults integerForKey:@"minCharge"]);
				settings.maxCharge = int([defaults integerForKey:@"maxCharge"]);
				if (settings.minCharge < 1) settings.minCharge = 1;
				if (settings.maxCharge < settings.minCharge) settings.maxCharge = settings.minCharge+1;
				updateDelay = int([defaults integerForKey:@"updateDelay"]);
			}
			
			NSRect newBounds = [_view convertRectToBacking:_view.bounds];
			
			settings.viewWidth = newBounds.size.width;
			settings.viewHeight = newBounds.size.height;
			self.animationTimeInterval = 1.0/60.0;
		}
    }
    return self;
}


- (void)setFrameSize:(NSSize)newSize
{
	[super setFrameSize:newSize];
	if (_view)
	{
		[_view setFrameSize:newSize];
		if (_view.wantsBestResolutionOpenGLSurface)	// on Lion & later, if we're using a best resolution surface, then call glViewport() with the appropriate width and height for the backing
		{
			NSRect newBounds = [_view convertRectToBacking:_view.bounds];
			
			settings.viewHeight = newBounds.size.height;
			settings.viewWidth = newBounds.size.width;
		}
		else
		{
			settings.viewHeight = _view.bounds.size.height;
			settings.viewWidth = _view.bounds.size.width;
		}
		if (settings.readyToDraw)
		{
			settings.aspectRatio = float(settings.viewWidth)/float(settings.viewHeight);
			glViewport(0, 0, settings.viewWidth, settings.viewHeight);
		}
	}
}


- (void)drawRect:(NSRect)rect
{
	[[NSColor blackColor] set];
    NSRectFill(rect);
    
    if (!_view)
    {
		// In the past, we'd tell the user here here that their computer does not meet the minimum specification for this screen saver. But I wonder how often that happens these days...
	}
}


- (void)animateOneFrame
{
	if (!_configuring && _view)
    {
        if (mainMonitor || !mainMonitorOnly)
        {
			settings.frameTime = timer.tick();
			if (settings.readyToDraw)
			{
				if (updateDelay && (targetTime < time(0)))
				{
					cleanUp(&settings);
					initSaver(&settings);
					targetTime = time(0) + updateDelay;
				}
				[[_view openGLContext] makeCurrentContext];
				draw(&settings);
				[[_view openGLContext] flushBuffer];
			}
        }
    }
}

- (void)startAnimation
{
#ifdef LOG_DEBUG
    NSLog( @"startAnimation" );
#endif
    
    [super startAnimation];
	if (!_configuring && _view)
    {
        if (mainMonitor || !mainMonitorOnly)
        {
			GLint interval = 1;
            
            [self lockFocus];
            [[_view openGLContext] makeCurrentContext];
            
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            glFlush();
            CGLSetParameter(CGLGetCurrentContext(), kCGLCPSwapInterval, &interval);	// don't allow screen tearing
            [[_view openGLContext] flushBuffer];
            
            initSaver(&settings);
			gettimeofday(&timer.prev_tv, NULL);	// reset the timer
			[self unlockFocus];
			
			targetTime = time(0) + updateDelay;
        }
    }
}

- (void)stopAnimation
{
    [super stopAnimation];
	
	if (!_configuring && _view)
    {
        if (mainMonitor || !mainMonitorOnly)
		{
			[[_view openGLContext] makeCurrentContext];
			cleanUp(&settings);
            settings.frameTime=0;
		}
	}
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
	if (!configureSheet)
		[[NSBundle bundleForClass:self.class] loadNibNamed:@"FieldLines" owner:self topLevelObjects:NULL];
    
    [IBversionNumberField setStringValue:kVersion];

    [IBmainMonitorOnly setState:(mainMonitorOnly ? NSOnState : NSOffState)];
    
	[IBdIons setIntValue:settings.dIons];
    [IBdIonsTxt setStringValue:[NSString stringWithFormat:
								[thisBundle localizedStringForKey:@"Ions number (%d)" value:@"" table:@""], settings.dIons]];

    [IBdSpeed setIntValue:settings.dSpeed];
    [IBdSpeedTxt setStringValue:[NSString stringWithFormat:
        [thisBundle localizedStringForKey:@"Speed (%d)" value:@"" table:@""], settings.dSpeed]];
    
    [IBdConstwidth setState:(settings.dConstwidth ? NSOnState : NSOffState)];
    
    [IBdWidth setIntValue:settings.dWidth];
    [IBdWidthTxt setStringValue:[NSString stringWithFormat:
        [thisBundle localizedStringForKey:@"Line Width (%d)" value:@"" table:@""], settings.dWidth]];

    [IBdStepSize setIntValue:settings.dStepSize];
    [IBdStepSizeTxt setStringValue:[NSString stringWithFormat:
        [thisBundle localizedStringForKey:@"Step size (%d)" value:@"" table:@""], settings.dStepSize]];

    [IBdMaxSteps setIntValue:settings.dMaxSteps];
    [IBdMaxStepsTxt setStringValue:[NSString stringWithFormat:
        [thisBundle localizedStringForKey:@"Max Steps (%d)" value:@"" table:@""], settings.dMaxSteps]];

    [IBdElectric setState:(settings.dElectric ? NSOnState : NSOffState)];

    [IBminCharge setIntValue:settings.minCharge];
    [IBmaxCharge setIntValue:settings.maxCharge];
    [IBChargeTxt setStringValue:[NSString stringWithFormat:
        [thisBundle localizedStringForKey:@"Charge value (%d-%d)" value:@"" table:@""], settings.minCharge, settings.maxCharge]];

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
	
    [IBmainMonitorOnly setTitle:[thisBundle localizedStringForKey:@"Main monitor only" value:@"" table:@""]];
    [IBCancel setTitle:[thisBundle localizedStringForKey:@"Cancel" value:@"" table:@""]];
    [IBSave setTitle:[thisBundle localizedStringForKey:@"Save" value:@"" table:@""]];
    
	_configuring = YES;
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
                [thisBundle localizedStringForKey:@"Refresh after %d min." value:@"" table:@""], fooInt]];
        }
        else {
            [IBupdateDelayTxt setStringValue:
                [thisBundle localizedStringForKey:@"Refresh: never" value:@"" table:@""]];
        }
    }
}

- (IBAction) closeSheet_save:(id) sender {
    ScreenSaverDefaults *defaults = [ScreenSaverDefaults defaultsForModuleWithName:@"fieldlines"];
    
#ifdef LOG_DEBUG
    NSLog( @"closeSheet_save" );
#endif
    
    mainMonitorOnly = ( [IBmainMonitorOnly state] == NSOnState ) ? true : false;

    settings.dIons = [IBdIons intValue];
    settings.dSpeed = [IBdSpeed intValue];
    settings.dConstwidth = ( [IBdConstwidth state] == NSOnState ) ? true : false;
    settings.dWidth = [IBdWidth intValue];
    settings.dStepSize = [IBdStepSize intValue];
    settings.dMaxSteps = [IBdMaxSteps intValue];
    settings.dElectric = ( [IBdElectric state] == NSOnState ) ? true : false;
    settings.minCharge = [IBminCharge intValue];
    settings.maxCharge = [IBmaxCharge intValue];
    updateDelay = [IBupdateDelay intValue]*60;
        
    [defaults setBool: mainMonitorOnly forKey: @"mainMonitorOnly"];
    [defaults setInteger: settings.dIons forKey: @"dIons"];
    [defaults setInteger: settings.dSpeed forKey: @"dSpeed"];
    [defaults setBool: settings.dConstwidth forKey: @"dConstwidth"];
    [defaults setInteger: settings.dWidth forKey: @"dWidth"];
    [defaults setInteger: settings.dStepSize forKey: @"dStepSize"];
    [defaults setInteger: settings.dMaxSteps forKey: @"dMaxSteps"];
    [defaults setBool: settings.dElectric forKey: @"dElectric"];
    [defaults setInteger: settings.minCharge forKey: @"minCharge"];
    [defaults setInteger: settings.maxCharge forKey: @"maxCharge"];
    [defaults setInteger: updateDelay forKey: @"updateDelay"];
    
    [defaults synchronize];

#ifdef LOG_DEBUG
    NSLog(@"Canged params" );
#endif

    _configuring = NO;
	if ([self isAnimating])
	{
		[self stopAnimation];
		[self startAnimation];
	}
	[NSApp endSheet:configureSheet];
	[configureSheet close];
}

- (IBAction) closeSheet_cancel:(id) sender {

#ifdef LOG_DEBUG
    NSLog( @"closeSheet_cancel" );
#endif
	_configuring = NO;
    [NSApp endSheet:configureSheet];
	[configureSheet close];
}

@end
