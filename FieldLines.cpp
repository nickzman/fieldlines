/*
 * Copyright (C) 1999-2010  Terence M. Welsh
 *
 * This file is part of Field Lines.
 *
 * Field Lines is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
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


#ifdef WIN32
#include <windows.h>
#include <rsWin32Saver/rsWin32Saver.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <regstr.h>
#include <commctrl.h>
#include <resource.h>
#endif
#include "FieldLines.h"
#include <stdio.h>
#include "rsText/rsText.h"
#include <math.h>
#include <time.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>


#define PIx2 6.28318530718f
/*float wide;
float high;
float deep;


class ion;


LPCTSTR registryPath = ("Software\\Really Slick\\Field Lines");
HDC hdc;
HGLRC hglrc;
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
BOOL dConstwidth;
BOOL dElectric;*/


// Useful random number macros
// Don't forget to initialize with srand()
inline int rsRandi(int x){
	return rand() % x;
}
inline float rsRandf(float x){
	return x * (float(rand()) / float(RAND_MAX));
}


class ion{
public:
	float charge;
	float xyz[3];
	float vel[3];
	float angle;
	float anglevel;

	ion() {}
	ion(FieldLinesSaverSettings *inSettings);
	~ion(){};
	void update(FieldLinesSaverSettings *inSettings);
};

ion::ion(FieldLinesSaverSettings *inSettings){
	if(rsRandi(2))
		charge = -1.0f;
	else
		charge = 1.0f;
	int foo = inSettings->minCharge + (rand() % (inSettings->maxCharge-inSettings->minCharge));	// change from the original code to set a minimum and maximum charge
    charge *= foo;
	xyz[0] = rsRandf(2.0f * inSettings->wide) - inSettings->wide;
	xyz[1] = rsRandf(2.0f * inSettings->high) - inSettings->high;
	xyz[2] = rsRandf(2.0f * inSettings->deep) - inSettings->deep;
	vel[0] = rsRandf(float(inSettings->dSpeed) * 4.0f) - (float(inSettings->dSpeed) * 2.0f);
	vel[1] = rsRandf(float(inSettings->dSpeed) * 4.0f) - (float(inSettings->dSpeed) * 2.0f);
	vel[2] = rsRandf(float(inSettings->dSpeed) * 4.0f) - (float(inSettings->dSpeed) * 2.0f);
	angle = 0.0f;
	anglevel = 0.0005f * float(inSettings->dSpeed) + 0.0005f * rsRandf(float(inSettings->dSpeed));
}

void ion::update(FieldLinesSaverSettings *inSettings){
	xyz[0] += vel[0] * inSettings->frameTime;
	xyz[1] += vel[1] * inSettings->frameTime;
	xyz[2] += vel[2] * inSettings->frameTime;
	if(xyz[0] > inSettings->wide)
		vel[0] -= 0.1f * float(inSettings->dSpeed);
	if(xyz[0] < -(inSettings->wide))
		vel[0] += 0.1f * float(inSettings->dSpeed);
	if(xyz[1] > inSettings->high)
		vel[1] -= 0.1f * float(inSettings->dSpeed);
	if(xyz[1] < -(inSettings->high))
		vel[1] += 0.1f * float(inSettings->dSpeed);
	if(xyz[2] > inSettings->deep)
		vel[2] -= 0.1f * float(inSettings->dSpeed);
	if(xyz[2] < -(inSettings->deep))
		vel[2] += 0.1f * float(inSettings->dSpeed);

	angle += anglevel;
	if(angle > PIx2)
		angle -= PIx2;
}


void drawfieldline(int source, float x, float y, float z, FieldLinesSaverSettings *inSettings){
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

	charge = inSettings->ions[source].charge;
	lastxyz[0] = inSettings->ions[source].xyz[0];
	lastxyz[1] = inSettings->ions[source].xyz[1];
	lastxyz[2] = inSettings->ions[source].xyz[2];
	dir[0] = x;
	dir[1] = y;
	dir[2] = z;

	// Do the first segment
	r = float(fabs(dir[2])) * brightness;
	g = float(fabs(dir[0])) * brightness;
	b = float(fabs(dir[1])) * brightness;
	if(r > 1.0f)
		r = 1.0f;
	if(g > 1.0f)
		g = 1.0f;
	if(b > 1.0f)
		b = 1.0f;
	lastr = r;
	lastg = g;
	lastb = b;
	glColor3f(r, g, b);
	xyz[0] = lastxyz[0] + dir[0];
	xyz[1] = lastxyz[1] + dir[1];
	xyz[2] = lastxyz[2] + dir[2];
	if(inSettings->dElectric){
		xyz[0] += rsRandf(float(inSettings->dStepSize) * 0.2f) - (float(inSettings->dStepSize) * 0.1f);
		xyz[1] += rsRandf(float(inSettings->dStepSize) * 0.2f) - (float(inSettings->dStepSize) * 0.1f);
		xyz[2] += rsRandf(float(inSettings->dStepSize) * 0.2f) - (float(inSettings->dStepSize) * 0.1f);
	}
	if(!inSettings->dConstwidth)
		glLineWidth((xyz[2] + 300.0f) * 0.000333f * float(inSettings->dWidth));
	glBegin(GL_LINE_STRIP);
		glColor3f(lastr, lastg, lastb);
		glVertex3fv(lastxyz);
		glColor3f(r, g, b);
		glVertex3fv(xyz);
	if(!inSettings->dConstwidth)
		glEnd();

	for(i=0; i<int(inSettings->dMaxSteps); i++){
		dir[0] = 0.0f;
		dir[1] = 0.0f;
		dir[2] = 0.0f;
		for(j=0; j<int(inSettings->dIons); j++){
			repulsion = charge * inSettings->ions[j].charge;
			tempvec[0] = xyz[0] - inSettings->ions[j].xyz[0];
			tempvec[1] = xyz[1] - inSettings->ions[j].xyz[1];
			tempvec[2] = xyz[2] - inSettings->ions[j].xyz[2];
			distsquared = tempvec[0] * tempvec[0] + tempvec[1] * tempvec[1] + tempvec[2] * tempvec[2];
			dist = float(sqrt(distsquared));
			if(dist < float(inSettings->dStepSize) && i > 2){
				end[0] = inSettings->ions[j].xyz[0];
				end[1] = inSettings->ions[j].xyz[1];
				end[2] = inSettings->ions[j].xyz[2];
				i = 10000;
			}
			tempvec[0] /= dist;
			tempvec[1] /= dist;
			tempvec[2] /= dist;
			if(distsquared < 1.0f)
				distsquared = 1.0f;
			dir[0] += tempvec[0] * repulsion / distsquared;
			dir[1] += tempvec[1] * repulsion / distsquared;
			dir[2] += tempvec[2] * repulsion / distsquared;
		}
		lastr = r;
		lastg = g;
		lastb = b;
		r = float(fabs(dir[2])) * brightness;
		g = float(fabs(dir[0])) * brightness;
		b = float(fabs(dir[1])) * brightness;
		if(inSettings->dElectric){
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
		distrec = float(inSettings->dStepSize) / float(sqrt(distsquared));
		dir[0] *= distrec;
		dir[1] *= distrec;
		dir[2] *= distrec;
		if(inSettings->dElectric){
			dir[0] += rsRandf(float(inSettings->dStepSize)) - (float(inSettings->dStepSize) * 0.5f);
			dir[1] += rsRandf(float(inSettings->dStepSize)) - (float(inSettings->dStepSize) * 0.5f);
			dir[2] += rsRandf(float(inSettings->dStepSize)) - (float(inSettings->dStepSize) * 0.5f);
		}
		lastxyz[0] = xyz[0];
		lastxyz[1] = xyz[1];
		lastxyz[2] = xyz[2];
		xyz[0] += dir[0];
		xyz[1] += dir[1];
		xyz[2] += dir[2];
		if(!inSettings->dConstwidth){
			glLineWidth((xyz[2] + 300.0f) * 0.000333f * float(inSettings->dWidth));
			glBegin(GL_LINE_STRIP);
		}
			glColor3f(lastr, lastg, lastb);
			glVertex3fv(lastxyz);
			if(i != 10000){
				if(i == (int(inSettings->dMaxSteps) - 1))
					glColor3f(0.0f, 0.0f, 0.0f);
				else
					glColor3f(r, g, b);
				glVertex3fv(xyz);
				if(i == (int(inSettings->dMaxSteps) - 1))
					glEnd();
			}
	}
	if(i == 10001){
			glColor3f(r, g, b);
			glVertex3fv(end);
		glEnd();
	}
}


void draw(FieldLinesSaverSettings *inSettings){
	int i;

	static float s = float(sqrt(float(inSettings->dStepSize) * float(inSettings->dStepSize) * 0.333f));

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for(i=0; i<inSettings->dIons; i++)
		inSettings->ions[i].update(inSettings);

	for(i=0; i<inSettings->dIons; i++){
		drawfieldline(i, s, s, s, inSettings);
		drawfieldline(i, s, s, -s, inSettings);
		drawfieldline(i, s, -s, s, inSettings);
		drawfieldline(i, s, -s, -s, inSettings);
		drawfieldline(i, -s, s, s, inSettings);
		drawfieldline(i, -s, s, -s, inSettings);
		drawfieldline(i, -s, -s, s, inSettings);
		drawfieldline(i, -s, -s, -s, inSettings);
	}

	// print text
	//static float totalTime = 0.0f;
	inSettings->totalTime += inSettings->frameTime;
	static std::string str;
	static int frames = 0;
	++frames;
	if(frames == 20){
		str = "FPS = " + to_string(20.0f / inSettings->totalTime);
		inSettings->totalTime = 0.0f;
		frames = 0;
	}
	if(inSettings->kStatistics){
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(0.0f, 50.0f * inSettings->aspectRatio, 0.0f, 50.0f, -1.0f, 1.0f);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		glTranslatef(1.0f, 48.0f, 0.0f);

		glColor3f(1.0f, 0.6f, 0.0f);
		inSettings->textwriter->draw(str);

		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
	}

    //wglSwapLayerBuffers(hdc, WGL_SWAP_MAIN_PLANE);
}


/*void idleProc(){
	// update time
	static rsTimer timer;
	frameTime = timer.tick();

	if(readyToDraw && !isSuspended && !checkingPassword)
		draw();
}*/


#ifdef WIN32
void initSaver(HWND hwnd){
	RECT rect;

	srand((unsigned)time(NULL));

	// Window initialization
	hdc = GetDC(hwnd);
	setBestPixelFormat(hdc);
	hglrc = wglCreateContext(hdc);
	GetClientRect(hwnd, &rect);
	wglMakeCurrent(hdc, hglrc);
	glViewport(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);

	// calculate boundaries
	if((rect.right - rect.left) > (rect.bottom - rect.top)){
		high = deep = 160.0f;
		wide = high * (rect.right - rect.left) / (rect.bottom - rect.top);
	}
	else{
		wide = deep = 160.0f;
		high = wide * (rect.bottom - rect.top) / (rect.right - rect.left);
	}
	aspectRatio = float(rect.right) / float(rect.bottom);
#else
void initSaver(FieldLinesSaverSettings *inSettings){
	srand(unsigned(time(NULL)));
	glViewport(0, 0, inSettings->viewWidth, inSettings->viewHeight);
	
	// calculate boundaries
	if (inSettings->viewWidth > inSettings->viewHeight)
	{
		inSettings->high = inSettings->deep = 160.0f;
		inSettings->wide = inSettings->high * (inSettings->viewWidth) / (inSettings->viewHeight);
	}
	else{
		inSettings->wide = inSettings->deep = 160.0f;
		inSettings->high = inSettings->wide * (inSettings->viewHeight) / (inSettings->viewWidth);
	}
	inSettings->aspectRatio = float(inSettings->viewWidth)/float(inSettings->viewHeight);
#endif

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LINE_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	gluPerspective(60.0, inSettings->aspectRatio, 1.0, inSettings->deep * 10);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -2.0f * inSettings->deep);

	if(inSettings->dConstwidth)
		glLineWidth(float(inSettings->dWidth) * 0.1f);

	if(inSettings->dIons < 1)
		inSettings->dIons = 1;
	if(inSettings->dIons > 50)
		inSettings->dIons = 50;
	inSettings->ions = new ion[inSettings->dIons];
	for (size_t i = 0 ; i < inSettings->dIons ; ++i)
	{
		new (&(inSettings->ions[i])) ion(inSettings);
	}

	// Initialize text
	inSettings->textwriter = new rsText;
	inSettings->readyToDraw = 1;
}

#if WIN32
void cleanUp(HWND hwnd){
	// Free memory
	delete[] ions;

	// Kill device context
	ReleaseDC(hwnd, hdc);
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(hglrc);
}
#else
void cleanUp(FieldLinesSaverSettings *inSettings)
{
	delete[] inSettings->ions;
	delete inSettings->textwriter;
	inSettings->readyToDraw = 0;
}
#endif


void setDefaults(FieldLinesSaverSettings *inSettings){
	inSettings->dIons = 6;
	inSettings->dStepSize = 10;
	inSettings->dMaxSteps = 300;
	inSettings->dWidth = 30;
	inSettings->dSpeed = 10;
	inSettings->dConstwidth = false;
	inSettings->dElectric = false;
	
	inSettings->minCharge = 1;
	inSettings->maxCharge = 2;
}


#ifdef WIN32
// Initialize all user-defined stuff
void readRegistry(){
	LONG result;
	HKEY skey;
	DWORD valtype, valsize, val;

	setDefaults();

	result = RegOpenKeyEx(HKEY_CURRENT_USER, registryPath, 0, KEY_READ, &skey);
	if(result != ERROR_SUCCESS)
		return;

	valsize=sizeof(val);

	result = RegQueryValueEx(skey, "Ions", 0, &valtype, (LPBYTE)&val, &valsize);
	if(result == ERROR_SUCCESS)
		dIons = val;
	result = RegQueryValueEx(skey, "StepSize", 0, &valtype, (LPBYTE)&val, &valsize);
	if(result == ERROR_SUCCESS)
		dStepSize = val;
	result = RegQueryValueEx(skey, "MaxSteps", 0, &valtype, (LPBYTE)&val, &valsize);
	if(result == ERROR_SUCCESS)
		dMaxSteps = val;
	result = RegQueryValueEx(skey, "Width", 0, &valtype, (LPBYTE)&val, &valsize);
	if(result == ERROR_SUCCESS)
		dWidth = val;
	result = RegQueryValueEx(skey, "Speed", 0, &valtype, (LPBYTE)&val, &valsize);
	if(result == ERROR_SUCCESS)
		dSpeed = val;
	result = RegQueryValueEx(skey, "Constwidth", 0, &valtype, (LPBYTE)&val, &valsize);
	if(result == ERROR_SUCCESS)
		dConstwidth = val;
	result = RegQueryValueEx(skey, "Electric", 0, &valtype, (LPBYTE)&val, &valsize);
	if(result == ERROR_SUCCESS)
		dElectric = val;
	result = RegQueryValueEx(skey, "FrameRateLimit", 0, &valtype, (LPBYTE)&val, &valsize);
	if(result == ERROR_SUCCESS)
		dFrameRateLimit = val;

	RegCloseKey(skey);
}


// Save all user-defined stuff
void writeRegistry(){
    LONG result;
	HKEY skey;
	DWORD val, disp;

	result = RegCreateKeyEx(HKEY_CURRENT_USER, registryPath, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &skey, &disp);
	if(result != ERROR_SUCCESS)
		return;

	val = dIons;
	RegSetValueEx(skey, "Ions", 0, REG_DWORD, (CONST BYTE*)&val, sizeof(val));
	val = dStepSize;
	RegSetValueEx(skey, "StepSize", 0, REG_DWORD, (CONST BYTE*)&val, sizeof(val));
	val = dMaxSteps;
	RegSetValueEx(skey, "MaxSteps", 0, REG_DWORD, (CONST BYTE*)&val, sizeof(val));
	val = dWidth;
	RegSetValueEx(skey, "Width", 0, REG_DWORD, (CONST BYTE*)&val, sizeof(val));
	val = dSpeed;
	RegSetValueEx(skey, "Speed", 0, REG_DWORD, (CONST BYTE*)&val, sizeof(val));
	val = dConstwidth;
	RegSetValueEx(skey, "Constwidth", 0, REG_DWORD, (CONST BYTE*)&val, sizeof(val));
	val = dElectric;
	RegSetValueEx(skey, "Electric", 0, REG_DWORD, (CONST BYTE*)&val, sizeof(val));
	val = dFrameRateLimit;
	RegSetValueEx(skey, "FrameRateLimit", 0, REG_DWORD, (CONST BYTE*)&val, sizeof(val));

	RegCloseKey(skey);
}


BOOL CALLBACK aboutProc(HWND hdlg, UINT msg, WPARAM wpm, LPARAM lpm){
	switch(msg){
	case WM_CTLCOLORSTATIC:
		if(HWND(lpm) == GetDlgItem(hdlg, WEBPAGE)){
			SetTextColor(HDC(wpm), RGB(0,0,255));
			SetBkColor(HDC(wpm), COLORREF(GetSysColor(COLOR_3DFACE)));
			return(int(GetSysColorBrush(COLOR_3DFACE)));
		}
		break;
    case WM_COMMAND:
		switch(LOWORD(wpm)){
		case IDOK:
		case IDCANCEL:
			EndDialog(hdlg, LOWORD(wpm));
			break;
		case WEBPAGE:
			ShellExecute(NULL, "open", "http://www.reallyslick.com", NULL, NULL, SW_SHOWNORMAL);
		}
	}
	return FALSE;
}


void initControls(HWND hdlg){
	char cval[16];

	SendDlgItemMessage(hdlg, IONS, UDM_SETRANGE, 0, LPARAM(MAKELONG(DWORD(50), DWORD(1))));
	SendDlgItemMessage(hdlg, IONS, UDM_SETPOS, 0, LPARAM(dIons));

	SendDlgItemMessage(hdlg, STEPSIZE, UDM_SETRANGE, 0, LPARAM(MAKELONG(DWORD(100), DWORD(1))));
	SendDlgItemMessage(hdlg, STEPSIZE, UDM_SETPOS, 0, LPARAM(dStepSize));

	SendDlgItemMessage(hdlg, MAXSTEPS, UDM_SETRANGE, 0, LPARAM(MAKELONG(DWORD(1000), DWORD(1))));
	SendDlgItemMessage(hdlg, MAXSTEPS, UDM_SETPOS, 0, LPARAM(dMaxSteps));

	SendDlgItemMessage(hdlg, WIDTH, TBM_SETRANGE, 0, LPARAM(MAKELONG(DWORD(1), DWORD(100))));
	SendDlgItemMessage(hdlg, WIDTH, TBM_SETPOS, 1, LPARAM(dWidth));
	SendDlgItemMessage(hdlg, WIDTH, TBM_SETLINESIZE, 0, LPARAM(1));
	SendDlgItemMessage(hdlg, WIDTH, TBM_SETPAGESIZE, 0, LPARAM(10));
	sprintf(cval, "%d", dWidth);
	SendDlgItemMessage(hdlg, WIDTHTEXT, WM_SETTEXT, 0, LPARAM(cval));

	SendDlgItemMessage(hdlg, SPEED, TBM_SETRANGE, 0, LPARAM(MAKELONG(DWORD(1), DWORD(100))));
	SendDlgItemMessage(hdlg, SPEED, TBM_SETPOS, 1, LPARAM(dSpeed));
	SendDlgItemMessage(hdlg, SPEED, TBM_SETLINESIZE, 0, LPARAM(1));
	SendDlgItemMessage(hdlg, SPEED, TBM_SETPAGESIZE, 0, LPARAM(10));
	sprintf(cval, "%d", dSpeed);
	SendDlgItemMessage(hdlg, SPEEDTEXT, WM_SETTEXT, 0, LPARAM(cval));

	CheckDlgButton(hdlg, CONSTWIDTH, dConstwidth);

	CheckDlgButton(hdlg, ELECTRIC, dElectric);

	initFrameRateLimitSlider(hdlg, FRAMERATELIMIT, FRAMERATELIMITTEXT);
}


BOOL screenSaverConfigureDialog(HWND hdlg, UINT msg, WPARAM wpm, LPARAM lpm){
	int ival;
	char cval[16];

    switch(msg){
    case WM_INITDIALOG:
        InitCommonControls();
        readRegistry();
		initControls(hdlg);
        return TRUE;
    case WM_COMMAND:
        switch(LOWORD(wpm)){
        case IDOK:
			dIons = SendDlgItemMessage(hdlg, IONS, UDM_GETPOS, 0, 0);
			dStepSize = SendDlgItemMessage(hdlg, STEPSIZE, UDM_GETPOS, 0, 0);
			dMaxSteps = SendDlgItemMessage(hdlg, MAXSTEPS, UDM_GETPOS, 0, 0);
			dWidth = SendDlgItemMessage(hdlg, WIDTH, TBM_GETPOS, 0, 0);
			dSpeed = SendDlgItemMessage(hdlg, SPEED, TBM_GETPOS, 0, 0);
			dConstwidth = (IsDlgButtonChecked(hdlg, CONSTWIDTH) == BST_CHECKED);
			dElectric = (IsDlgButtonChecked(hdlg, ELECTRIC) == BST_CHECKED);
			dFrameRateLimit = SendDlgItemMessage(hdlg, FRAMERATELIMIT, TBM_GETPOS, 0, 0);
			writeRegistry();
            // Fall through
        case IDCANCEL:
            EndDialog(hdlg, LOWORD(wpm));
            break;
		case DEFAULTS:
			setDefaults();
			initControls(hdlg);
			break;
        case ABOUT:
			DialogBox(mainInstance, MAKEINTRESOURCE(DLG_ABOUT), hdlg, DLGPROC(aboutProc));
		}
        return TRUE;
	case WM_HSCROLL:
		if(HWND(lpm) == GetDlgItem(hdlg, WIDTH)){
			ival = SendDlgItemMessage(hdlg, WIDTH, TBM_GETPOS, 0, 0);
			sprintf(cval, "%d", ival);
			SendDlgItemMessage(hdlg, WIDTHTEXT, WM_SETTEXT, 0, LPARAM(cval));
		}
		if(HWND(lpm) == GetDlgItem(hdlg, SPEED)){
			ival = SendDlgItemMessage(hdlg, SPEED, TBM_GETPOS, 0, 0);
			sprintf(cval, "%d", ival);
			SendDlgItemMessage(hdlg, SPEEDTEXT, WM_SETTEXT, 0, LPARAM(cval));
		}
		if(HWND(lpm) == GetDlgItem(hdlg, FRAMERATELIMIT))
			updateFrameRateLimitSlider(hdlg, FRAMERATELIMIT, FRAMERATELIMITTEXT);
		return TRUE;
    }
    return FALSE;
}


LONG screenSaverProc(HWND hwnd, UINT msg, WPARAM wpm, LPARAM lpm){
	static unsigned long threadID;

	switch(msg){
	case WM_CREATE:
		readRegistry();
		initSaver(hwnd);
		readyToDraw = 1;
		break;
	case WM_DESTROY:
		readyToDraw = 0;
		cleanUp(hwnd);
		break;
	}
	return defScreenSaverProc(hwnd, msg, wpm, lpm);
}
#endif
