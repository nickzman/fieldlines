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


#include <windows.h>
#include <stdio.h>
#include "../Savergl/savergl.h"
//#include <process.h>
#include <math.h>
#include <time.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <regstr.h>
#include <commctrl.h>
#include "resource.h"


#define PIx2 6.28318530718f
#define wide 200
#define high 150


class ion;


// Globals
LPCTSTR registryPath = ("Software\\Really Slick\\Field Lines");
HDC hdc;
HGLRC hglrc;
int readyToDraw = 0;
ion *ions;
// Parameters edited in the dialog box
int dIons;
int dStepSize;
int dMaxSteps;
int dWidth;
int dSpeed;
int dPriority;
BOOL dConstwidth;
BOOL dElectric;


// Useful random number macros
// Don't forget to initialize with srand()
inline int myRandi(int x){
	return((rand() * x) / RAND_MAX);
}
inline float myRandf(float x){
	return(float(rand() * x) / float(RAND_MAX));
}


class ion{
public:
	float charge;
	float xyz[3];
	float vel[3];
	float angle;
	float anglevel;

	ion();
	~ion();
	void update();
};

ion::ion(){
	charge = float(myRandi(2));
	if(charge == 0.0f)
		charge = -1.0f;
	xyz[0] = myRandf(2.0f * float(wide)) - float(wide);
	xyz[1] = myRandf(2.0f * float(high)) - float(high);
	xyz[2] = myRandf(2.0f * float(wide)) - float(wide);
	vel[0] = myRandf(float(dSpeed) * 0.1f) - (float(dSpeed) * 0.05f);
	vel[1] = myRandf(float(dSpeed) * 0.1f) - (float(dSpeed) * 0.05f);
	vel[2] = myRandf(float(dSpeed) * 0.1f) - (float(dSpeed) * 0.05f);
	angle = 0.0f;
	anglevel = 0.0005f * float(dSpeed) + 0.0005f * myRandf(float(dSpeed));
}

ion::~ion(){
}

void ion::update(){
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


void drawfieldline(int source, float x, float y, float z){
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

	charge = ions[source].charge;
	lastxyz[0] = ions[source].xyz[0];
	lastxyz[1] = ions[source].xyz[1];
	lastxyz[2] = ions[source].xyz[2];
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
	if(dElectric){
		xyz[0] += myRandf(float(dStepSize) * 0.3f) - (float(dStepSize) * 0.15f);
		xyz[1] += myRandf(float(dStepSize) * 0.3f) - (float(dStepSize) * 0.15f);
		xyz[2] += myRandf(float(dStepSize) * 0.3f) - (float(dStepSize) * 0.15f);
	}
	if(!dConstwidth)
		glLineWidth((xyz[2] + 300.0f) * 0.000333f * float(dWidth));
	glBegin(GL_LINE_STRIP);
		glColor3f(lastr, lastg, lastb);
		glVertex3fv(lastxyz);
		glColor3f(r, g, b);
		glVertex3fv(xyz);
	if(!dConstwidth)
		glEnd();

	for(i=0; i<int(dMaxSteps); i++){
		dir[0] = 0.0f;
		dir[1] = 0.0f;
		dir[2] = 0.0f;
		for(j=0; j<int(dIons); j++){
			repulsion = charge * ions[j].charge;
			tempvec[0] = xyz[0] - ions[j].xyz[0];
			tempvec[1] = xyz[1] - ions[j].xyz[1];
			tempvec[2] = xyz[2] - ions[j].xyz[2];
			distsquared = tempvec[0] * tempvec[0] + tempvec[1] * tempvec[1] + tempvec[2] * tempvec[2];
			dist = float(sqrt(distsquared));
			if(dist < float(dStepSize) && i > 2){
				end[0] = ions[j].xyz[0];
				end[1] = ions[j].xyz[1];
				end[2] = ions[j].xyz[2];
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
		distrec = float(dStepSize) / float(sqrt(distsquared));
		dir[0] *= distrec;
		dir[1] *= distrec;
		dir[2] *= distrec;
		if(dElectric){
			dir[0] += myRandf(float(dStepSize)) - (float(dStepSize) * 0.5f);
			dir[1] += myRandf(float(dStepSize)) - (float(dStepSize) * 0.5f);
			dir[2] += myRandf(float(dStepSize)) - (float(dStepSize) * 0.5f);
		}
		lastxyz[0] = xyz[0];
		lastxyz[1] = xyz[1];
		lastxyz[2] = xyz[2];
		xyz[0] += dir[0];
		xyz[1] += dir[1];
		xyz[2] += dir[2];
		if(!dConstwidth){
			glLineWidth((xyz[2] + 300.0f) * 0.000333f * float(dWidth));
			glBegin(GL_LINE_STRIP);
		}
			glColor3f(lastr, lastg, lastb);
			glVertex3fv(lastxyz);
			if(i != 10000){
				if(i == (int(dMaxSteps) - 1))
					glColor3f(0.0f, 0.0f, 0.0f);
				else
					glColor3f(r, g, b);
				glVertex3fv(xyz);
				if(i == (int(dMaxSteps) - 1))
					glEnd();
			}
	}
	if(i == 10001){
			glColor3f(r, g, b);
			glVertex3fv(end);
		glEnd();
	}
}


void draw(){
	int i;
	static float s = float(sqrt(float(dStepSize) * float(dStepSize) * 0.333f));

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for(i=0; i<dIons; i++)
		ions[i].update();

	for(i=0; i<dIons; i++){
		drawfieldline(i, s, s, s);
		drawfieldline(i, s, s, -s);
		drawfieldline(i, s, -s, s);
		drawfieldline(i, s, -s, -s);
		drawfieldline(i, -s, s, s);
		drawfieldline(i, -s, s, -s);
		drawfieldline(i, -s, -s, s);
		drawfieldline(i, -s, -s, -s);
	}

    wglSwapLayerBuffers(hdc, WGL_SWAP_MAIN_PLANE);
}


void IdleProc(){
	if(readyToDraw && !checkingPassword)
		draw();
}


void initSaver(HWND hwnd){
	RECT rect;

	srand((unsigned)time(NULL));
	rand(); rand(); rand(); rand(); rand();

	// Window initialization
	hdc = GetDC(hwnd);
	SetBestPixelFormat(hdc);
	hglrc = wglCreateContext(hdc);
	GetClientRect(hwnd, &rect);
	wglMakeCurrent(hdc, hglrc);
	glViewport(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LINE_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(80.0, float(rect.right) / float(rect.bottom), 50, 3000);
	glTranslatef(0.0, 0.0, -(wide * 2));
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if(dConstwidth)
		glLineWidth(float(dWidth) * 0.1f);

	ions = new ion[dIons];
}


void cleanUp(HWND hwnd){
	// Free memory
	delete[] ions;

	// Kill device context
	ReleaseDC(hwnd, hdc);
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(hglrc);
}


void setDefaults(){
	dIons = 4;
	dStepSize = 15;
	dMaxSteps = 100;
	dWidth = 30;
	dSpeed = 10;
	dPriority = 0;
	dConstwidth = FALSE;
	dElectric = FALSE;
}


// Initialize all user-defined stuff
void readRegistry(){
	LONG result;
	HKEY skey;
	DWORD valtype, valsize, val;

	setDefaults();
	theVideoMode = 10000;

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
	result = RegQueryValueEx(skey, "Priority", 0, &valtype, (LPBYTE)&val, &valsize);
	if(result == ERROR_SUCCESS)
		dPriority = val;
	result = RegQueryValueEx(skey, "Constwidth", 0, &valtype, (LPBYTE)&val, &valsize);
	if(result == ERROR_SUCCESS)
		dConstwidth = val;
	result = RegQueryValueEx(skey, "Electric", 0, &valtype, (LPBYTE)&val, &valsize);
	if(result == ERROR_SUCCESS)
		dElectric = val;
	result = RegQueryValueEx(skey, "theVideoMode", 0, &valtype, (LPBYTE)&val, &valsize);
	if(result == ERROR_SUCCESS)
		theVideoMode = val;

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
	val = dPriority;
	RegSetValueEx(skey, "Priority", 0, REG_DWORD, (CONST BYTE*)&val, sizeof(val));
	val = dConstwidth;
	RegSetValueEx(skey, "Constwidth", 0, REG_DWORD, (CONST BYTE*)&val, sizeof(val));
	val = dElectric;
	RegSetValueEx(skey, "Electric", 0, REG_DWORD, (CONST BYTE*)&val, sizeof(val));
	val = theVideoMode;
	RegSetValueEx(skey, "theVideoMode", 0, REG_DWORD, (CONST BYTE*)&val, sizeof(val));

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
			ShellExecute(NULL, "open", "http://business.fortunecity.com/knight/352/", NULL, NULL, SW_SHOWNORMAL);
		}
	}
	return FALSE;
}


void initControls(HWND hdlg){
	char cval[16];

	SendDlgItemMessage(hdlg, IONS, UDM_SETRANGE, 0, LPARAM(MAKELONG(DWORD(100), DWORD(1))));
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
	SendDlgItemMessage(hdlg, PRIORITY, TBM_SETRANGE, 0, LPARAM(MAKELONG(DWORD(0), DWORD(2))));
	SendDlgItemMessage(hdlg, PRIORITY, TBM_SETPOS, 1, LPARAM(dPriority));
	SendDlgItemMessage(hdlg, PRIORITY, TBM_SETLINESIZE, 0, LPARAM(1));
	SendDlgItemMessage(hdlg, PRIORITY, TBM_SETPAGESIZE, 0, LPARAM(1));
	sprintf(cval, "%d", dPriority);
	SendDlgItemMessage(hdlg, PRIORITYTEXT, WM_SETTEXT, 0, LPARAM(cval));
	CheckDlgButton(hdlg, CONSTWIDTH, dConstwidth);
	CheckDlgButton(hdlg, ELECTRIC, dElectric);

	InitVideoModeComboBox(hdlg, VIDEOMODE);
}


BOOL ScreenSaverConfigureDialog(HWND hdlg, UINT msg, WPARAM wpm, LPARAM lpm){
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
			dPriority = SendDlgItemMessage(hdlg, PRIORITY, TBM_GETPOS, 0, 0);
			dConstwidth = (IsDlgButtonChecked(hdlg, CONSTWIDTH) == BST_CHECKED);
			dElectric = (IsDlgButtonChecked(hdlg, ELECTRIC) == BST_CHECKED);
			theVideoMode = SendDlgItemMessage(hdlg, VIDEOMODE, CB_GETCURSEL, 0, 0);
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
		if(HWND(lpm) == GetDlgItem(hdlg, PRIORITY)){
			ival = SendDlgItemMessage(hdlg, PRIORITY, TBM_GETPOS, 0, 0);
			sprintf(cval, "%d", ival);
			SendDlgItemMessage(hdlg, PRIORITYTEXT, WM_SETTEXT, 0, LPARAM(cval));
		}
		return TRUE;
    }
    return FALSE;
}


LONG ScreenSaverProc(HWND hwnd, UINT msg, WPARAM wpm, LPARAM lpm){
	static unsigned long threadID;

	switch(msg){
	case WM_CREATE:
		readRegistry();
		switch(dPriority){
		case 0:
			SetThreadPriority((void *)threadID, THREAD_PRIORITY_LOWEST);
			break;
		case 1:
			SetThreadPriority((void *)threadID, THREAD_PRIORITY_BELOW_NORMAL);
			break;
		case 2:
			SetThreadPriority((void *)threadID, THREAD_PRIORITY_NORMAL);
		}
		initSaver(hwnd);
		readyToDraw = 1;
		break;
	case WM_DESTROY:
		readyToDraw = 0;
		cleanUp(hwnd);
		break;
	}
	return DefScreenSaverProc(hwnd, msg, wpm, lpm);
}