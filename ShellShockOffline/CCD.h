//	     "C powered Colored Drawing"
//		  www.ucpsystems.com 2018
//		Windows only graphic library
//		    CPU based rendering
//				  V2.2.0

#pragma once

//_CCD_FULLRGB
//IFDEF:	You can use 8 bit RGB 
//IFNDEF:	Only 8 bit grayscale will be rendered.
#define _CCD_FULLRGB	


//_CCD_WINDOW
//IFDEF:	The win32 api will create a window and you can use the following settings. You need to use the CCD entrypoints (Below).
//IFNDEF:	No window will appear and you need to use a "main" function for entry.
#define _CCD_WINDOW				

	//_CCD_NOBUFFER
	//IFDEF:	The canvas will not be used. Only screen pixel will be written.
	//IFNDEF:	Pixels wil be writte to screen and buffer.
	//INFO:		Very slow.
	#define _CCD_NOBUFFER_

	//_CCD_ONLYBUFFER
	//IFDEF:	Pixels are only written to the canvas and not the screen. Use _CCD_DRAWBUFFER to draw the canvas
	//IFNDEF:	Pixels are written normaly to screen and buffer.
	//INFO:		_CCD_NOBUFFER must be undefinded. This is the fastest way of drawing.
	#define _CCD_ONLYBUFFER

	//_CCD_SINGLEFRAME
	//IFDEF:	The update function will only be called once.
	//IFNDEF:	The update function will be called every form update.
	#define _CCD_SINGLEFRAME_	

	//_CCD_SHOWCONSOL
	//IFDEF:	The console will appear.
	//IFNDEF:	The console will not show up.
	//INFO:		scanf does not work or even gets called.
	#define _CCD_SHOWCONSOL		

//INFO:		This is how many pixels are used to draw lines. Lower is better, but slower.
#define _CCD_QUALITY 0.001f

//INFO:		This is the grayscale value which will be used for initializing.
#define _CCD_BACKCOLOR 0

#define _CCD_WINDOW_WINAME	"ShellShockOffline"
#define _CCD_WINDOW_WIDTH	16*63
#define _CCD_WINDOW_HEIGHT	9*63

#define _CCD_NULL			(void*)0x0

#define _CCD_DATA_STRING	char*
#define _CCD_DATA_DEF		unsigned short int
#define _CCD_DATA_ULINT		unsigned long int
#define _CCD_DATA_RAT		short int
#define _CCD_DATA_UINT		unsigned int
#define _CCD_DATA_FLOAT		float 

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <windows.h>

#define _CCD_DATA_HINTS		HINSTANCE
#define _CCD_DATA_LPSTR		LPSTR
#define _CCD_DATA_WPARAM	WPARAM
#define _CCD_DATA_LPARAM	LPARAM
#define _CCD_DATA_HWND		HWND
#define _CCD_DATA_MSG		MSG
#define _CCD_DATA_WNDC		WNDCLASSEX
#define _CCD_DATA_HBRUSH	HBRUSH
#define _CCD_DATA_HDC		HDC
#define _CCD_DATA_COLR		COLORREF
#define _CCD_DATA_HBITM		HBITMAP
#define _CDD_DATA_TIME		time_t

#define _CCD_ALIASWINDOW	CCD_WINDOW
typedef struct _CCD_ALIASWINDOW _CCD_ALIASWINDOW;
struct _CCD_ALIASWINDOW {
	_CCD_DATA_DEF	USI_WIDTH, USI_HEIGHT, USI_BACKCOLOR;
	_CCD_DATA_STRING C_WINDOWNAME;
};

#define _CCD_MAIN_F			cdd_main
#define _CCD_STARTUP_F		cdd_startup
#define _CCD_END_F			cdd_end
#define _CCD_UPDATE_F		cdd_update

//INFO:		These are the function will be called when _CCD_WINDOW is defined. No "int main(void)" is needed.
//USAGE:	_CCD_UPDATE{ [CODE] } 

//INFO:		This function will be called every frame the window updates.
#define _CCD_UPDATE			_CCD_DATA_DEF		_CCD_UPDATE_F	(_CCD_DATA_ULINT ui_frame,_CCD_DATA_ULINT ui_time,_CCD_DATA_MSG msg_info)
//INFO:		_CCD_STARTUP will be called before window initializing.
#define _CCD_STARTUP		_CCD_ALIASWINDOW*	_CCD_STARTUP_F	(_CCD_DATA_HINTS hInstance, _CCD_DATA_HINTS hPrevInstance, _CCD_DATA_LPSTR lpCmdLine, int nCmdShow)
//INFO:		_CCD_MAIN will be called after window initializing.
#define _CCD_MAIN		    _CCD_DATA_DEF		_CCD_MAIN_F		(_CCD_DATA_HWND hwnd_handle)
//INFO:		After application closing _CCD_END will be called.
#define _CCD_END			void				_CCD_END_F		(_CCD_DATA_WPARAM wpr_wparam,_CCD_DATA_ULINT ui_frame)

//INFO:		These number describe the way triangles are getting drawn on the screen.
#define _CCD_DM_FILLED		0x1FF
#define _CCD_DM_OUTLINE		0x0FF

#define _CCD_DATA_DRAWMODE CCD_BOOL
typedef _CCD_DATA_DEF _CCD_DATA_DRAWMODE;

#ifdef _CCD_FULLRGB
	#define _CCD_ALIASCOLOR CCD_COLOR	
#else
	#define _CCD_ALIASCOLOR _CCD_DATA_DEF
#endif

#ifdef _CCD_FULLRGB
	typedef struct _CCD_ALIASCOLOR _CCD_ALIASCOLOR;
	struct _CCD_ALIASCOLOR {
		_CCD_DATA_DEF		R, G, B;
	};
#endif

#define _CCD_ALIAS			CCD_CANVAS
typedef struct _CCD_ALIAS _CCD_ALIAS;
struct _CCD_ALIAS {
	_CCD_ALIASCOLOR**		C_COLOR;
	_CCD_DATA_DEF			USI_WIDTH;
	_CCD_DATA_DEF			USI_HEIGHT;
};


#define _CCD_ALIASVECTOR	CCD_VECTOR
typedef struct _CCD_ALIASVECTOR _CCD_ALIASVECTOR;
struct _CCD_ALIASVECTOR {
	_CCD_DATA_DEF			X, Y;
};
//INFO:		Parameter for DM_MODE. Sprite blendmodes are describing the write process of the sprite onto the canvas.
//			Only used with _CCD_DRAWSPRITE as DM_MODE.

//FUNCTION:	Pixels are overwritten when using _CCD_SBM_OVERRIDE.
//INFO:		_CCD_SBM_OVERRIDE does not use DM_VALUE. Please fill in 0. 
#define _CCD_SBM_OVERRIDE	0x0CC 
//FUNCTION:	Pixels are getting averaged.
//INFO:		_CCD_SBM_COMPINE does not use DM_VALUE. Please fill in 0. 
#define _CCD_SBM_COMPINE	0xCCC
//FUNCTION:	Specific color channels are overwritten. 
//INFO:		_CCD_SBM_BLEND does specifi wich color channel is getting rewritten. 1 = red, 2 = green, 3 = blue, 12 = red and green,13 = red and blue, 23 blue and green.
#define _CCD_SBM_BLEND		0xBCC
	#define _CCD_C_RED		0x01	
	#define _CCD_C_GREEN	0x02	
	#define _CCD_C_BLUE		0x03	
	#define _CCD_C_REDGREEN	0x12	
	#define _CCD_C_REDBLUE	0x13	
	#define _CCD_C_BLUEGRN	0x23	
//FUNCTION:	Pixel Values are getting added to one another.
//INFO:		_CCD_SBM_ADD does not use DM_VALUE. Please fill in 0. 
#define _CCD_SBM_ADD		0xACC

#define _CCD_SPRITE			CCD_SPRITE
typedef struct _CCD_SPRITE _CCD_SPRITE;
struct _CCD_SPRITE {
	_CCD_ALIASCOLOR**		C_COLOR;
	_CCD_DATA_DEF			USI_WIDTH;
	_CCD_DATA_DEF			USI_HEIGHT;
	_CCD_ALIASVECTOR		V_POSITION;
};

//INFO:		Usable functions for drawing.
//			_CCD_SPRITE can be used as a _CCD_ALIAS!
_CCD_ALIAS			_CCD_CREATE		(_CCD_DATA_DEF		USI_WIDTH, 
									_CCD_DATA_DEF		USI_HEIGHT	);

_CCD_SPRITE			_CCD_SPR_CREATE	(_CCD_DATA_DEF		USI_WIDTH,
									_CCD_DATA_DEF		USI_HEIGHT,	
									_CCD_ALIASVECTOR*	V_POSITION);

void				_CCD_CLEAR		(_CCD_ALIAS*		CCD_CANVAS	);

void				_CCD_DELETE		(_CCD_ALIAS*		CCD_CANVAS	);

void				_CCD_DRAWLINE	(_CCD_ALIAS*		CCD_CANVAS,
									_CCD_ALIASVECTOR	V_LEFT, 
									_CCD_ALIASVECTOR	V_RIGH,
									_CCD_ALIASCOLOR		C_COLOR);

void				_CCD_DRAWBUFFER (_CCD_ALIAS*		CCD_CANVAS);

void				_CCD_CPUSHADER	(_CCD_ALIAS*		CCD_CANVAS
									/* IN PROGRESS*/			);

void				_CCD_DRAWSPRITE (_CCD_ALIAS*		CCD_CANVAS,
									_CCD_SPRITE			SPR_SPRITE,
									_CCD_DATA_DRAWMODE	DM_MODE,
									_CCD_DATA_DRAWMODE	DM_VALUE);

void				_CCD_DRAWRECT	(_CCD_ALIAS*		CCD_CANVAS,
									_CCD_ALIASVECTOR	V_LEFTUPPER, 
									_CCD_ALIASVECTOR	V_RIGHTBOT,	
									_CCD_ALIASCOLOR		C_COLOR,
									_CCD_DATA_DRAWMODE	DM_MODE);

void				_CCD_DRAWTRI	(_CCD_ALIAS*		CCD_CANVAS,
									_CCD_ALIASVECTOR	V_ONE, 
									_CCD_ALIASVECTOR	V_TWO,	
									_CCD_ALIASVECTOR	V_THREE,	
									_CCD_ALIASCOLOR		C_COLOR,
									_CCD_DATA_DRAWMODE	DM_MODE);

void				_CCD_SETPIXEL	(_CCD_ALIAS*		CCD_CANVAS,
									_CCD_ALIASVECTOR	V_POINT,		
									_CCD_ALIASCOLOR		C_COLOR);
_CCD_ALIASCOLOR		_CCD_GETPIXEL	(_CCD_ALIAS*		CCD_CANVAS,
									_CCD_ALIASVECTOR	V_POINT);	
int					_CCD_COLORCMP	(_CCD_ALIASCOLOR	C_ONE,
									_CCD_ALIASCOLOR		C_TWO);
int					_CCD_SAVE		(_CCD_ALIAS*		CCD_CANVAS);
