//
//  FieldLines.h
//  FieldLines
//
//  Created by Nick Zitzmann on 5/25/20.
//

#ifndef FieldLines_h
#define FieldLines_h

#include <sys/types.h>
#include <vector>

class ion;
class rsText;

typedef struct FieldLinesSaverSettings
{
#ifdef WIN32
	LPCTSTR registryPath = ("Software\\Really Slick\\Field Lines");
	HDC hdc;
	HGLRC hglrc;
#endif
	int32_t viewWidth;
	int32_t viewHeight;
	float wide;
	float high;
	float deep;
	
	bool doingPreview;
	float totalTime;
	bool kStatistics;
	
	int readyToDraw = 0;
	ion *ions;
	float aspectRatio;
	float frameTime = 0.0f;
	// text output
	rsText* textwriter;
	// Parameters edited in the dialog box
	int dIons;
	int dStepSize;
	int dMaxSteps;
	int dWidth;
	int dSpeed;
	bool dConstwidth;
	bool dElectric;
	
	int minCharge;
	int maxCharge;
} _fieldLinesSaverSettings;

__private_extern__ void setDefaults(FieldLinesSaverSettings *inSettings);
__private_extern__ void draw(FieldLinesSaverSettings *inSettings);
__private_extern__ void initSaver(FieldLinesSaverSettings *inSettings);
__private_extern__ void cleanUp(FieldLinesSaverSettings *inSettings);

#endif /* FieldLines_h */
