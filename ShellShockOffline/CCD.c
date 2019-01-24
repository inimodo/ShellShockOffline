#define _CRT_SECURE_NO_WARNINGS
#define _CCD_FILE_WRITEMODE	"w"
#define _CCD_FILE_NAME_RGB	"export.ppm"
#define _CCD_FILE_NAME_GRA	"export.pgm"
#define _CCD_FILE_PGM		"P2\n"
#define _CCD_FILE_PPM		"P3\n"
#define _CCD_FILE_S			"255\n"
#define _CCD_FILE_WH		"%hu  %hu\n"
#define _CCD_FILE_COLOR		"%hu  %hu  %hu    "
#define _CCD_FILE_GRAY		"%hu  "
#define _CCD_FILE_BRK		"\n"
#define _CCD_WINDOW_CLASS	"CCD"
#define _CCD_WINDOW_CONSOLE "CON", "w", stdout
#define _CCD_WINDOW_DEFAULT "> Initialized window and console...\n\n"
#define _CCD_WINDOW_WITITLE	""
#define _CCD_WINDOW_EHWND	"window handle damaged"
#define _CCD_WINDOW_EREG	"windows registration failed"

#include "CCD.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <windows.h>

#ifdef _CCD_WINDOW
_CCD_DATA_WNDC wc;
_CCD_DATA_HWND hwnd;
_CCD_DATA_MSG Msg;
_CCD_DATA_ULINT ui_frame = 0;
_CCD_DATA_HDC hdc_frame;
#endif

_CCD_ALIAS _CCD_CREATE(_CCD_DATA_DEF USI_WIDTH, _CCD_DATA_DEF USI_HEIGTH) 
{
	_CCD_ALIAS _CPP_CREATE;
	_CPP_CREATE.USI_HEIGHT = USI_HEIGTH;
	_CPP_CREATE.USI_WIDTH  = USI_WIDTH;
	_CPP_CREATE.C_COLOR = (_CCD_ALIASCOLOR**)malloc(USI_HEIGTH *sizeof(_CCD_ALIASCOLOR*));
	for (_CCD_DATA_DEF usi_row = 0; usi_row < USI_HEIGTH; usi_row++)	
		_CPP_CREATE.C_COLOR[usi_row] = (_CCD_ALIASCOLOR*)malloc(USI_WIDTH * sizeof(_CCD_ALIASCOLOR));
	return _CPP_CREATE;
}
_CCD_SPRITE _CCD_SPR_CREATE(_CCD_DATA_DEF USI_WIDTH, _CCD_DATA_DEF USI_HEIGHT, _CCD_ALIASVECTOR* V_POSITION)
{
	_CCD_SPRITE _CCD_SPR_CREATE;
	_CCD_SPR_CREATE.USI_HEIGHT = USI_HEIGHT;
	_CCD_SPR_CREATE.USI_WIDTH = USI_WIDTH;
	_CCD_SPR_CREATE.C_COLOR = (_CCD_ALIASCOLOR**)malloc(USI_HEIGHT * sizeof(_CCD_ALIASCOLOR*));
	for (_CCD_DATA_DEF usi_row = 0; usi_row < USI_HEIGHT; usi_row++)
		_CCD_SPR_CREATE.C_COLOR[usi_row] = (_CCD_ALIASCOLOR*)malloc(USI_WIDTH * sizeof(_CCD_ALIASCOLOR));
	if (V_POSITION != _CCD_NULL) {
		_CCD_SPR_CREATE.V_POSITION = *V_POSITION;
	}
	return _CCD_SPR_CREATE;
}
void _CCD_CLEAR(_CCD_ALIAS* CCD_CANVAS) 
{
	for (_CCD_DATA_DEF X = 0; X <CCD_CANVAS->USI_HEIGHT; X++)
	{
		for (_CCD_DATA_DEF Y = 0; Y <  CCD_CANVAS->USI_WIDTH; Y++)
		{
		#ifdef _CCD_FULLRGB
			CCD_CANVAS->C_COLOR[X][Y].R = _CCD_BACKCOLOR;
			CCD_CANVAS->C_COLOR[X][Y].G = _CCD_BACKCOLOR;
			CCD_CANVAS->C_COLOR[X][Y].B = _CCD_BACKCOLOR;
		#else	
			CCD_CANVAS->C_COLOR[X][Y] = _CCD_BACKCOLOR;
		#endif 
		}
	}
}
void _CCD_DELETE(_CCD_ALIAS* CCD_CANVAS) 
{	
	for (_CCD_DATA_DEF usi_row = 0; usi_row < CCD_CANVAS->USI_HEIGHT; usi_row++)
		free(CCD_CANVAS->C_COLOR[usi_row]);
	free(CCD_CANVAS->C_COLOR);
}
void _CCD_DRAWLINE(_CCD_ALIAS* 	CCD_CANVAS,_CCD_ALIASVECTOR	V_LEFT,_CCD_ALIASVECTOR	V_RIGHT, _CCD_ALIASCOLOR C_COLOR)
{
	_CCD_DATA_RAT usi_deltax = V_LEFT.X - V_RIGHT.X;
	_CCD_DATA_RAT usi_deltay = V_LEFT.Y - V_RIGHT.Y;
	for (float f_render = 1; f_render > 0; f_render-= _CCD_QUALITY)
	{
		_CCD_ALIASVECTOR v_deltavec = { usi_deltax*f_render + V_RIGHT.X,usi_deltay*f_render + V_RIGHT.Y };
		_CCD_SETPIXEL(CCD_CANVAS, v_deltavec, C_COLOR);
	} 
}
void _CCD_DRAWRECT(_CCD_ALIAS* CCD_CANVAS, _CCD_ALIASVECTOR	V_LEFTUPPER, _CCD_ALIASVECTOR	V_RIGHTBOT, _CCD_ALIASCOLOR		C_COLOR, _CCD_DATA_DRAWMODE DM_MODE)
{
	if (DM_MODE == _CCD_DM_FILLED)
	{
		for (_CCD_DATA_DEF i = 0; i < V_RIGHTBOT .X-V_LEFTUPPER.X; i++)
		{
			_CCD_DRAWLINE(CCD_CANVAS, (_CCD_ALIASVECTOR) { V_LEFTUPPER.X+i, V_LEFTUPPER.Y }, (_CCD_ALIASVECTOR) { V_LEFTUPPER.X + i, V_RIGHTBOT.Y }, C_COLOR);
		}
	}
	else if (DM_MODE == _CCD_DM_OUTLINE)
	{
		_CCD_DRAWLINE(CCD_CANVAS, V_LEFTUPPER, (_CCD_ALIASVECTOR){ V_LEFTUPPER.X,V_RIGHTBOT.Y }, C_COLOR);
		_CCD_DRAWLINE(CCD_CANVAS, V_LEFTUPPER, (_CCD_ALIASVECTOR){ V_RIGHTBOT.X, V_LEFTUPPER.Y }, C_COLOR);
		_CCD_DRAWLINE(CCD_CANVAS, V_RIGHTBOT, (_CCD_ALIASVECTOR){ V_LEFTUPPER.X, V_RIGHTBOT.Y }, C_COLOR);
		_CCD_DRAWLINE(CCD_CANVAS, V_RIGHTBOT, (_CCD_ALIASVECTOR) { V_RIGHTBOT.X, V_LEFTUPPER.Y }, C_COLOR);
	}
}
void _CCD_DRAWTRI(_CCD_ALIAS* CCD_CANVAS, _CCD_ALIASVECTOR V_ONE, _CCD_ALIASVECTOR V_TWO, _CCD_ALIASVECTOR V_THREE, _CCD_ALIASCOLOR	C_COLOR, _CCD_DATA_DRAWMODE DM_MODE) 
{
	if (DM_MODE == _CCD_DM_FILLED) 
	{
		_CCD_DATA_RAT usi_deltax = V_ONE.X - V_TWO.X;
		_CCD_DATA_RAT usi_deltay = V_ONE.Y - V_TWO.Y;
		for (float f_render = 1; f_render > 0; f_render -= _CCD_QUALITY)
		{
			_CCD_ALIASVECTOR v_deltavec = { usi_deltax*f_render + V_TWO.X,usi_deltay*f_render + V_TWO.Y };
			_CCD_DRAWLINE(CCD_CANVAS, V_THREE, v_deltavec, C_COLOR);
		}
	}
	else if (DM_MODE == _CCD_DM_OUTLINE)
	{
		_CCD_DRAWLINE(CCD_CANVAS, V_ONE, V_TWO,C_COLOR);
		_CCD_DRAWLINE(CCD_CANVAS, V_TWO, V_THREE,C_COLOR);
		_CCD_DRAWLINE(CCD_CANVAS, V_THREE, V_ONE,C_COLOR);
	}
}
int _CCD_SAVE(_CCD_ALIAS* CCD_CANVAS)
{
	#ifdef _CCD_FULLRGB
		FILE * F_FILE = fopen(_CCD_FILE_NAME_RGB, _CCD_FILE_WRITEMODE);
	#else
		FILE * F_FILE = fopen(_CCD_FILE_NAME_GRA, _CCD_FILE_WRITEMODE);
	#endif
		if (!F_FILE)return 1;
	#ifdef _CCD_FULLRGB
		fprintf(F_FILE, _CCD_FILE_PPM);
		fprintf(F_FILE, _CCD_FILE_WH, CCD_CANVAS->USI_HEIGHT, CCD_CANVAS->USI_WIDTH);
		fprintf(F_FILE, _CCD_FILE_S);
		for (_CCD_DATA_DEF Y = 0; Y < CCD_CANVAS->USI_WIDTH; Y++){
			for (_CCD_DATA_DEF X = 0; X <CCD_CANVAS->USI_HEIGHT; X++)
				fprintf(F_FILE, _CCD_FILE_COLOR, CCD_CANVAS->C_COLOR[X][Y].R, CCD_CANVAS->C_COLOR[X][Y].G, CCD_CANVAS->C_COLOR[X][Y].B);			
			fprintf(F_FILE, _CCD_FILE_BRK);
		}
	#else	
		fprintf(F_FILE, _CCD_FILE_PGM);
		fprintf(F_FILE, _CCD_FILE_WH,  CCD_CANVAS->USI_HEIGHT, CCD_CANVAS->USI_WIDTH);
		fprintf(F_FILE, _CCD_FILE_S);
		for (_CCD_DATA_DEF Y = 0; Y < CCD_CANVAS->USI_WIDTH; Y++)
		{	
			for (_CCD_DATA_DEF X = 0; X <CCD_CANVAS->USI_HEIGHT; X++)
			{
				fprintf(F_FILE, _CCD_FILE_GRAY, CCD_CANVAS->C_COLOR[X][Y]);
			}
			fprintf(F_FILE, _CCD_FILE_BRK);
		}
	#endif
	fclose(F_FILE);
	return 0;
}
_CCD_ALIASCOLOR	_CCD_GETPIXEL(_CCD_ALIAS* CCD_CANVAS,_CCD_ALIASVECTOR V_POINT){
	return CCD_CANVAS->C_COLOR[V_POINT.Y][V_POINT.X];
}
int _CCD_COLORCMP(_CCD_ALIASCOLOR C_ONE, _CCD_ALIASCOLOR C_TWO)
{
	#ifdef _CCD_FULLRGB
		if (C_ONE.R == C_TWO.R && 
			C_ONE.G == C_TWO.G && 
			C_ONE.B == C_TWO.B)return 1;
	#else
		if (C_ONE == C_TWO)return 1;
	#endif
	return 0;
}
void _CCD_SETPIXEL(_CCD_ALIAS* CCD_CANVAS, _CCD_ALIASVECTOR V_POINT, _CCD_ALIASCOLOR	C_COLOR) {
	#ifdef _CCD_WINDOW
			#ifndef _CCD_NOBUFFER
				if (V_POINT.X >= 0 && V_POINT.Y >= 0 && V_POINT.X < CCD_CANVAS->USI_WIDTH && V_POINT.Y < CCD_CANVAS->USI_HEIGHT) {		
						CCD_CANVAS->C_COLOR[V_POINT.Y][V_POINT.X] = C_COLOR;		
					#ifndef _CCD_ONLYBUFFER
						#ifdef _CCD_FULLRGB
							SetPixel(hdc_frame, V_POINT.X, V_POINT.Y, RGB(C_COLOR.R, C_COLOR.G, C_COLOR.B));
						#else
							SetPixel(hdc_frame, V_POINT.X, V_POINT.Y, RGB(C_COLOR, C_COLOR, C_COLOR));
						#endif	
					#endif
				}
			#else 
				if (V_POINT.X >= 0 && V_POINT.Y >= 0) {
					#ifdef _CCD_FULLRGB
							SetPixel(hdc_frame, V_POINT.X, V_POINT.Y, RGB(C_COLOR.R, C_COLOR.G, C_COLOR.B));
					#else
							SetPixel(hdc_frame, V_POINT.X, V_POINT.Y, RGB(C_COLOR, C_COLOR, C_COLOR));
					#endif	
				}
			#endif
	#else 
	if (V_POINT.X >= 0 && V_POINT.Y >= 0 && V_POINT.X < CCD_CANVAS->USI_WIDTH && V_POINT.Y < CCD_CANVAS->USI_HEIGHT) 
		CCD_CANVAS->C_COLOR[V_POINT.Y][V_POINT.X] = C_COLOR;
	#endif
}
#ifdef _CCD_WINDOW
	#ifndef _CCD_FULLAC
		_CCD_STARTUP{
			_CCD_ALIASWINDOW win = { 1000, 500, 2, _CCD_WINDOW_WINAME };
			return &win;
		}
	#endif
long __stdcall WndProc(_CCD_DATA_HWND hwnd, _CCD_DATA_UINT msg, _CCD_DATA_WPARAM wParam, _CCD_DATA_LPARAM lParam){
	if(msg == WM_CLOSE)DestroyWindow(hwnd);
	else if(msg == WM_DESTROY)PostQuitMessage(0);
	else return DefWindowProc(hwnd, msg, wParam, lParam);
	return 0;
}
int __stdcall WinMain(_CCD_DATA_HINTS hInstance, _CCD_DATA_HINTS hPrevInstance, _CCD_DATA_LPSTR lpCmdLine, int nCmdShow){
	#ifdef _CCD_SHOWCONSOL
		AllocConsole();
		freopen(_CCD_WINDOW_CONSOLE);
	#endif
	printf(_CCD_WINDOW_DEFAULT);


	_CCD_ALIASWINDOW* win_window = _CCD_STARTUP_F(hInstance, hPrevInstance, lpCmdLine, nCmdShow);

	wc.hbrBackground = (_CCD_DATA_HBRUSH)(win_window->USI_BACKCOLOR); wc.cbSize = sizeof(_CCD_DATA_WNDC);
	wc.lpfnWndProc = WndProc; wc.hInstance = hInstance; 
	wc.lpszClassName = _CCD_WINDOW_CLASS; wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wc))return 0;

	hwnd = CreateWindowEx(WS_EX_CLIENTEDGE,_CCD_WINDOW_CLASS,_CCD_WINDOW_WINAME,WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, _CCD_WINDOW_WIDTH, _CCD_WINDOW_HEIGHT,NULL, NULL, hInstance, NULL);

	if (hwnd == NULL)return 0;
	
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	hdc_frame = GetDC(hwnd);
	if (_CCD_MAIN_F(hwnd) != 0)return 0;	
	_CCD_DATA_RAT us_switch = 1;
	_CDD_DATA_TIME t_pasttime= clock();
	while (GetMessage(&Msg, NULL, 0, 0) > 0){

		#ifdef _CCD_SINGLEFRAME
		if (ui_frame == 0)
		#endif 
		if (_CCD_UPDATE_F(ui_frame, clock() - t_pasttime, Msg) != 0)return 0;
		t_pasttime = clock();
		if (us_switch == 1) {
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}
		ui_frame++;
		if (GetAsyncKeyState(VK_NUMPAD0) != 0) {
			us_switch *= -1;
			Sleep(200);
		}
	}
	_CCD_END_F(Msg.wParam, ui_frame);
	ReleaseDC(NULL,hdc_frame);
	return Msg.wParam;
}
void _CCD_DRAWBUFFER(_CCD_ALIAS* CCD_CANVAS) {
	_CCD_DATA_COLR *col_buffer = (_CCD_DATA_COLR*)malloc(CCD_CANVAS->USI_HEIGHT * CCD_CANVAS->USI_WIDTH* sizeof(_CCD_DATA_COLR));
	for (_CCD_DATA_UINT x = 0, c = 0; x < CCD_CANVAS->USI_HEIGHT; x++)
		for (_CCD_DATA_UINT y = 0; y < CCD_CANVAS->USI_WIDTH; y++){
		#ifdef _CCD_FULLRGB
			col_buffer[c] = RGB(CCD_CANVAS->C_COLOR[x][y].B, CCD_CANVAS->C_COLOR[x][y].G, CCD_CANVAS->C_COLOR[x][y].R);
		#else
			col_buffer[c] = RGB(CCD_CANVAS->C_COLOR[x][y], CCD_CANVAS->C_COLOR[x][y], CCD_CANVAS->C_COLOR[x][y]);
		#endif	
			c++;
		}
	_CCD_DATA_HBITM bmp_map = CreateBitmap(CCD_CANVAS->USI_WIDTH,CCD_CANVAS->USI_HEIGHT,1, 8 * 4, (void*)col_buffer);
	_CCD_DATA_HDC hdc_src = CreateCompatibleDC(hdc_frame);
	SelectObject(hdc_src, bmp_map);
	BitBlt(hdc_frame,0, 0, CCD_CANVAS->USI_WIDTH,CCD_CANVAS->USI_HEIGHT,hdc_src,0,0,SRCCOPY); 
	free(col_buffer);
	DeleteObject(bmp_map);
	DeleteDC(hdc_src);	
}
void _CCD_DRAWSPRITE(_CCD_ALIAS * CCD_CANVAS, _CCD_SPRITE SPR_SPRITE, _CCD_DATA_DRAWMODE DM_MODE, _CCD_DATA_DRAWMODE DM_VALUE)
{
	_CCD_ALIASCOLOR	c_color_over,	c_color_avr;
	for (_CCD_DATA_UINT x = 0, c = 0; x < SPR_SPRITE.USI_HEIGHT; x++)
		for (_CCD_DATA_UINT y = 0; y < SPR_SPRITE.USI_WIDTH; y++) {

			switch (DM_MODE)
			{
			case _CCD_SBM_OVERRIDE:
				_CCD_SETPIXEL(CCD_CANVAS, (_CCD_ALIASVECTOR) { y + SPR_SPRITE.V_POSITION.X,x + SPR_SPRITE.V_POSITION.Y }, SPR_SPRITE.C_COLOR[x][y]);
				break;
			case _CCD_SBM_COMPINE:
				c_color_over  = _CCD_GETPIXEL(CCD_CANVAS, (_CCD_ALIASVECTOR) { y + SPR_SPRITE.V_POSITION.X, x + SPR_SPRITE.V_POSITION.Y });
				#ifdef _CCD_FULLRGB
					_CCD_ALIASCOLOR	c_color_avr_over;
					c_color_avr_over.R = (SPR_SPRITE.C_COLOR[x][y].R + c_color_over.R) / 2;
					c_color_avr_over.G = (SPR_SPRITE.C_COLOR[x][y].G + c_color_over.G) / 2;
					c_color_avr_over.B = (SPR_SPRITE.C_COLOR[x][y].B + c_color_over.B) / 2;
					_CCD_SETPIXEL(CCD_CANVAS, (_CCD_ALIASVECTOR) { y + SPR_SPRITE.V_POSITION.X, x + SPR_SPRITE.V_POSITION.Y }, c_color_avr_over);
				#else
					if (c_color_over == 0)c_color_over = 1;
					_CCD_SETPIXEL(CCD_CANVAS, (_CCD_ALIASVECTOR) { y + SPR_SPRITE.V_POSITION.X, x + SPR_SPRITE.V_POSITION.Y }, (SPR_SPRITE.C_COLOR[x][y]+ c_color_over)/2);
				#endif
				break;
			case _CCD_SBM_BLEND:
				c_color_over = _CCD_GETPIXEL(CCD_CANVAS, (_CCD_ALIASVECTOR) { y + SPR_SPRITE.V_POSITION.X, x + SPR_SPRITE.V_POSITION.Y });
				#ifdef _CCD_FULLRGB
					c_color_avr.R = c_color_over.R;
					c_color_avr.G = c_color_over.G ;
					c_color_avr.B = c_color_over.B ;
					switch (DM_VALUE)
					{
					case _CCD_C_RED:
						c_color_avr.R = SPR_SPRITE.C_COLOR[x][y].R;
						break;
					case _CCD_C_GREEN:
						c_color_avr.G = SPR_SPRITE.C_COLOR[x][y].G;
						break;
					case _CCD_C_BLUE:
						c_color_avr.B = SPR_SPRITE.C_COLOR[x][y].B;
						break;
					case _CCD_C_REDGREEN:
						c_color_avr.R = SPR_SPRITE.C_COLOR[x][y].R;
						c_color_avr.G = SPR_SPRITE.C_COLOR[x][y].G;
						break;
					case _CCD_C_REDBLUE:
						c_color_avr.R = SPR_SPRITE.C_COLOR[x][y].R;
						c_color_avr.B = SPR_SPRITE.C_COLOR[x][y].B;
						break;
					case _CCD_C_BLUEGRN:
						c_color_avr.B = SPR_SPRITE.C_COLOR[x][y].B;
						c_color_avr.G = SPR_SPRITE.C_COLOR[x][y].G;
						break;
					}
					_CCD_SETPIXEL(CCD_CANVAS, (_CCD_ALIASVECTOR) { y + SPR_SPRITE.V_POSITION.X, x + SPR_SPRITE.V_POSITION.Y }, c_color_avr);
				#else 
					return;
				#endif
				break;
			case _CCD_SBM_ADD:
				c_color_over = _CCD_GETPIXEL(CCD_CANVAS, (_CCD_ALIASVECTOR) { y + SPR_SPRITE.V_POSITION.X, x + SPR_SPRITE.V_POSITION.Y });
				#ifdef _CCD_FULLRGB
					_CCD_ALIASCOLOR	c_color_avr;
					c_color_avr.R = (SPR_SPRITE.C_COLOR[x][y].R + c_color_over.R) %255;
					c_color_avr.G = (SPR_SPRITE.C_COLOR[x][y].G + c_color_over.G) %255;
					c_color_avr.B = (SPR_SPRITE.C_COLOR[x][y].B + c_color_over.B) %255;
					_CCD_SETPIXEL(CCD_CANVAS, (_CCD_ALIASVECTOR) { y + SPR_SPRITE.V_POSITION.X, x + SPR_SPRITE.V_POSITION.Y }, c_color_avr);
				#else
					if (c_color == 0)c_color = 1;
					_CCD_SETPIXEL(CCD_CANVAS, (_CCD_ALIASVECTOR) { y + SPR_SPRITE.V_POSITION.X, x + SPR_SPRITE.V_POSITION.Y }, (SPR_SPRITE.C_COLOR[x][y] + c_color)%255);
				#endif
				break;
			}

		}
}
#endif 
