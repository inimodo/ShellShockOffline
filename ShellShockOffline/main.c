#include"CCD.h"
#include<math.h>

_CCD_ALIASCOLOR		c_under = { 200, 255, 255 };
_CCD_ALIASCOLOR		c_debug = { 1, 255, 155 };

#define DEBUGGER
//PHYSIK
#define _P_TIMESCALE	10
#define _P_POWERSCALE	0.5f

//SHOOTING
#define _P_SHELLSIZE	3
#define _P_MAXTIME		100
#define _P_HITREGMAX	(_CCD_DATA_FLOAT)(25)
#define _P_HITSIZE		(_CCD_DATA_FLOAT)(3)
#define _P_HITPOWER		(_CCD_DATA_FLOAT)(50)
#define _P_HITCRATOR	(_CCD_DATA_FLOAT)(5.f)

#define _P_ROUGHNESS	50
#define _P_POWERDIV		10
#define _P_TANK_W		8
#define _P_TANK_H		5
#define _P_DEF_STRETCH	50
#define _P_MIN_HEIGTH	300
#define _P_SHELLOFFSET	20

#define _P_DEF_SCATTER	(_CCD_DATA_DEF)(_CCD_WINDOW_WIDTH/_P_DEF_STRETCH)
#define _P_GRAVITY		(_CCD_DATA_FLOAT)(9.81f)
#define _P_RADDEG		(_CCD_DATA_FLOAT)(3.1415f/180.0f)
#define _P_DEGRAD		(_CCD_DATA_FLOAT)(180.0f/3.1415f)

_CCD_ALIASCOLOR		c_backcolor = { 230, 185, 155 };
_CCD_ALIASCOLOR		c_shellcolor = { 255, 185, 0 };
_CCD_ALIASCOLOR		c_powerline = { 255 / 2, 255 / 2, 255 / 2 };
_CCD_ALIASCOLOR		c_line = { 0, 0, 255 };
_CCD_ALIASVECTOR	v_zeropos = { 20, 20 };

_CCD_DATA_DEF		ui_currentlines = _P_DEF_STRETCH;
_CCD_DATA_DEF		ui_i = 0;

_CCD_DATA_DEF		ui_currentplayer = 0;
_CCD_DATA_DEF		ui_currentshooting = 0;
_CCD_DATA_DEF		ui_shooting_time = 0;

_CCD_ALIASVECTOR	v_lines[_P_DEF_STRETCH];

_CCD_SPRITE			s_shell;

_CCD_ALIAS			c_canvas;

_CCD_ALIASCOLOR		c_tank_color[2] = { { 255, 0, 0 },{ 0, 255, 0 } };
_CCD_SPRITE			c_tank[2];
_CCD_ALIASVECTOR	v_tank_pos[2] = { { 4, 50 },{ _P_DEF_STRETCH - 4, 50 } };
_CCD_ALIASVECTOR	v_tank_aim[2] = { { 0, 50 },{ 0, 50 } };

void switchrole() { if (ui_currentplayer)ui_currentplayer = 0; else ui_currentplayer = 1; }

void render_lines()
{
	for (ui_i = 0; ui_i < _P_DEF_STRETCH; ui_i++)
	{
		if (ui_i != _P_DEF_STRETCH - 1)
			_CCD_DRAWLINE(&c_canvas, v_lines[ui_i], v_lines[ui_i + 1], c_line);
		else
			_CCD_DRAWLINE(&c_canvas, v_lines[ui_i], (_CCD_ALIASVECTOR) { _CCD_WINDOW_WIDTH, v_lines[0].Y }, c_line);
#ifdef DEBUGGER
		//if (ui_i != _P_DEF_STRETCH - 1)
		//_CCD_DRAWRECT(&c_canvas, v_lines[ui_i], v_lines[ui_i + 1], c_debug, _CCD_DM_OUTLINE);
		//else
		//_CCD_DRAWRECT(&c_canvas, v_lines[ui_i], (_CCD_ALIASVECTOR){ _CCD_WINDOW_WIDTH, v_lines[0].Y }, c_debug, _CCD_DM_OUTLINE);

#endif
	}
}
void interpolate(_CCD_ALIASVECTOR * v_pos, _CCD_DATA_DEF d_percentage, _CCD_DATA_DEF d_index)
{
	_CCD_DATA_FLOAT ui_delta_x = 0, ui_delta_y = 0;

	if (d_index != _P_DEF_STRETCH - 1) {
		ui_delta_x = (_CCD_DATA_FLOAT)v_lines[d_index].X - (_CCD_DATA_FLOAT)v_lines[d_index + 1].X;
		ui_delta_y = (_CCD_DATA_FLOAT)v_lines[d_index].Y - (_CCD_DATA_FLOAT)v_lines[d_index + 1].Y;
	}
	else {
		ui_delta_x = (_CCD_DATA_FLOAT)v_lines[d_index].X - (_CCD_DATA_FLOAT)_CCD_WINDOW_WIDTH;
		ui_delta_y = (_CCD_DATA_FLOAT)v_lines[d_index].Y - (_CCD_DATA_FLOAT)v_lines[0].Y;
	}
	ui_delta_x *= (_CCD_DATA_FLOAT)d_percentage / 100.f;
	ui_delta_y *= (_CCD_DATA_FLOAT)d_percentage / 100.f;
	if (d_index != _P_DEF_STRETCH - 1) {
		v_pos->X = (_CCD_DATA_DEF)ui_delta_x + v_lines[d_index + 1].X;
		v_pos->Y = (_CCD_DATA_DEF)ui_delta_y + v_lines[d_index + 1].Y;
	}
	else {
		v_pos->X = (_CCD_DATA_DEF)ui_delta_x + (_CCD_DATA_FLOAT)_CCD_WINDOW_WIDTH;
		v_pos->Y = (_CCD_DATA_DEF)ui_delta_y + (_CCD_DATA_FLOAT)v_lines[0].Y;
	}

}
_CCD_DATA_FLOAT getdistance(_CCD_ALIASVECTOR v_one, _CCD_ALIASVECTOR v_two)
{
	return sqrtf((v_one.Y - v_two.Y)*(v_one.Y - v_two.Y) + (v_one.X - v_two.X)*(v_one.X - v_two.X));
}

void explode_on(_CCD_DATA_DEF d_index)
{
	_CCD_DATA_DEF i_startlevel = 0;
	i_startlevel += v_lines[d_index - (_CCD_DATA_DEF)_P_HITSIZE].Y;
	i_startlevel += v_lines[d_index + (_CCD_DATA_DEF)_P_HITSIZE].Y;
	i_startlevel /= 2;

	for (_CCD_DATA_DEF i = 0; i < _P_HITSIZE * 2; i++)
	{
		_CCD_DATA_DEF i_index = (i - (_CCD_DATA_DEF)_P_HITSIZE) + d_index;
		if (i_index < 0 || i_index >_P_DEF_STRETCH)continue;
		_CCD_DATA_DEF d_yaxis = (_CCD_DATA_DEF)((_CCD_DATA_FLOAT)(i - _P_HITSIZE)*(_CCD_DATA_FLOAT)(i - _P_HITSIZE)*_P_HITCRATOR);
		v_lines[i_index].Y = (_CCD_DATA_DEF)(_P_HITSIZE*_P_HITSIZE*_P_HITCRATOR) - d_yaxis + i_startlevel;
	}

}

_CCD_DATA_DEF on_line(_CCD_ALIASVECTOR v_pos)
{
	_CCD_DATA_FLOAT ui_delta_x = 0, ui_delta_y = 0, ui_angle = 0, ui_y_delta = 0, ui_x_delta = 0, f_distance = 0;

	_CCD_DATA_DEF d_index = (_CCD_DATA_DEF)((float)v_pos.X / (float)_P_DEF_SCATTER);

	if (d_index != _P_DEF_STRETCH - 1) {
		ui_delta_x = (_CCD_DATA_FLOAT)v_lines[d_index + 1].X - (_CCD_DATA_FLOAT)v_lines[d_index].X;
		ui_delta_y = (_CCD_DATA_FLOAT)v_lines[d_index + 1].Y - (_CCD_DATA_FLOAT)v_lines[d_index].Y;
	}
	else {
		ui_delta_x = (_CCD_DATA_FLOAT)_CCD_WINDOW_WIDTH - (_CCD_DATA_FLOAT)v_lines[d_index].X;
		ui_delta_y = (_CCD_DATA_FLOAT)v_lines[0].Y - (_CCD_DATA_FLOAT)v_lines[d_index].Y;
	}
	ui_angle = _P_DEGRAD * atan(ui_delta_y / ui_delta_x);
	ui_x_delta = v_pos.X - (_CCD_DATA_FLOAT)v_lines[d_index].X;
	ui_y_delta = tan(ui_angle*_P_RADDEG)*ui_x_delta;

	_CCD_ALIASVECTOR v_out = { v_lines[d_index].X + ui_x_delta, v_lines[d_index].Y + ui_y_delta };

	f_distance = getdistance(v_out, v_pos);

	if (f_distance < _P_HITREGMAX)return 1;

#ifdef DEBUGGER

	_CCD_DRAWLINE(&c_canvas, v_lines[d_index], v_lines[d_index + 1], c_under);

	_CCD_DRAWLINE(&c_canvas, v_lines[d_index], v_out, c_debug);
	_CCD_DRAWLINE(&c_canvas, v_pos, v_out, c_debug);

	_CCD_DRAWLINE(&c_canvas, v_lines[d_index], v_pos, c_under);

	_CCD_SETPIXEL(&c_canvas, v_out, c_shellcolor);


#endif

	return 0;
}
void movetanks()
{
	if (GetAsyncKeyState(VK_NUMPAD4) != 0)v_tank_pos[ui_currentplayer].Y++;
	if (GetAsyncKeyState(VK_NUMPAD6) != 0)v_tank_pos[ui_currentplayer].Y--;

	if (GetAsyncKeyState(VK_RIGHT) != 0)v_tank_aim[ui_currentplayer].X++;
	if (GetAsyncKeyState(VK_LEFT) != 0)v_tank_aim[ui_currentplayer].X--;
	if (v_tank_aim[ui_currentplayer].X <= 0)v_tank_aim[ui_currentplayer].X = 359;
	v_tank_aim[ui_currentplayer].X %= 360;

	if (GetAsyncKeyState(VK_UP) != 0 && v_tank_aim[ui_currentplayer].Y < 99)v_tank_aim[ui_currentplayer].Y++;
	if (GetAsyncKeyState(VK_DOWN) != 0 && v_tank_aim[ui_currentplayer].Y > 1)v_tank_aim[ui_currentplayer].Y--;

	if (v_tank_pos[ui_currentplayer].Y <= 1) {
		v_tank_pos[ui_currentplayer].Y = 98;
		if (v_tank_pos[ui_currentplayer].X != ui_currentlines - 1)v_tank_pos[ui_currentplayer].X++;
	}
	if (v_tank_pos[ui_currentplayer].Y >= 99) {
		v_tank_pos[ui_currentplayer].Y = 1;
		if (v_tank_pos[ui_currentplayer].X != 0)v_tank_pos[ui_currentplayer].X--;
	}
}
void initshooting()
{
	s_shell.V_POSITION.X = 10;
	s_shell.V_POSITION.Y = 10;
	ui_currentshooting = 1;
	ui_shooting_time = 0;
}
void shoot() {

	if (on_line(s_shell.V_POSITION))
	{
		explode_on((int)((float)s_shell.V_POSITION.X / (float)_P_DEF_SCATTER));
		ui_currentshooting = 0;
		switchrole();
		return;
	}

	ui_shooting_time++;
	if (s_shell.V_POSITION.X > _CCD_WINDOW_WIDTH || s_shell.V_POSITION.X <0 || s_shell.V_POSITION.Y > _CCD_WINDOW_HEIGHT) {
		ui_currentshooting = 0;
		switchrole();
		return;
	}
	_CCD_DATA_FLOAT f_time = (float)((float)ui_shooting_time / (float)_P_TIMESCALE);
	_CCD_DATA_FLOAT f_power = (float)((float)v_tank_aim[ui_currentplayer].Y / (float)_P_POWERSCALE);
	_CCD_DATA_FLOAT f_angle = (float)(360 - v_tank_aim[ui_currentplayer].X);
	s_shell.V_POSITION.X = ((int)c_tank[ui_currentplayer].V_POSITION.X + (int)((f_power * cos(f_angle*_P_RADDEG))*f_time));
	s_shell.V_POSITION.Y = ((int)c_tank[ui_currentplayer].V_POSITION.Y - (int)((f_power * sin(f_angle*_P_RADDEG) - (_P_GRAVITY*f_time)) *f_time)) - _P_SHELLOFFSET;
}
void render_tanks() {
	for (ui_i = 0; ui_i < 2; ui_i++) {
		interpolate(&c_tank[ui_i].V_POSITION, v_tank_pos[ui_i].Y, v_tank_pos[ui_i].X);
		c_tank[ui_i].V_POSITION.Y -= _P_TANK_H / 2;
		c_tank[ui_i].V_POSITION.X -= _P_TANK_W / 2;
		_CCD_DRAWSPRITE(&c_canvas, c_tank[ui_i], _CCD_SBM_OVERRIDE, _CCD_NULL);
		_CCD_ALIASVECTOR v_line = {
			c_tank[ui_i].V_POSITION.X + cos(v_tank_aim[ui_i].X*_P_RADDEG)*v_tank_aim[ui_i].Y,
			c_tank[ui_i].V_POSITION.Y + sin(v_tank_aim[ui_i].X*_P_RADDEG)*v_tank_aim[ui_i].Y
		};
		_CCD_ALIASVECTOR v_tank = {
			c_tank[ui_i].V_POSITION.X + _P_TANK_H / 2,
			c_tank[ui_i].V_POSITION.Y + _P_TANK_H / 2
		};
		if (ui_i == ui_currentplayer)
			_CCD_DRAWLINE(&c_canvas, v_line, v_tank, c_powerline);
		if (ui_currentshooting)
			_CCD_DRAWSPRITE(&c_canvas, s_shell, _CCD_SBM_OVERRIDE, _CCD_NULL);
	}
}
_CCD_MAIN{
	c_canvas = _CCD_CREATE(_CCD_WINDOW_WIDTH, _CCD_WINDOW_HEIGHT);
s_shell = _CCD_SPR_CREATE(_P_SHELLSIZE * 2, _P_SHELLSIZE * 2, &v_zeropos);
_CCD_CLEAR(&s_shell);
for (ui_i = 0; ui_i < 2; ui_i++)
{
	c_tank[ui_i] = _CCD_SPR_CREATE(_P_TANK_W, _P_TANK_H, &v_zeropos);
	_CCD_DRAWRECT(&c_tank[ui_i], (_CCD_ALIASVECTOR) { 0, 0 }, (_CCD_ALIASVECTOR) { _P_TANK_W, _P_TANK_H }, c_tank_color[ui_i], _CCD_DM_FILLED);
}

for (ui_i = 0; ui_i < 360; ui_i += 5) {

	_CCD_DRAWLINE(&s_shell, (_CCD_ALIASVECTOR) { _P_SHELLSIZE, _P_SHELLSIZE }, (_CCD_ALIASVECTOR) {
		_P_SHELLSIZE + cos(_P_RADDEG*ui_i)*_P_SHELLSIZE, _P_SHELLSIZE + sin(_P_RADDEG*ui_i)*_P_SHELLSIZE
	}, c_shellcolor);
}
srand(time(0));
for (_CCD_DATA_DEF i = 0; i < _P_DEF_STRETCH; i++)
{
	v_lines[i].Y = _P_MIN_HEIGTH + (rand() % _P_ROUGHNESS);
	v_lines[i].X = i * _P_DEF_SCATTER;
}
return 0;
}
_CCD_DATA_DEF ui_fps = 0, ui_pastframes = 0, ui_lastfps = 0;

_CCD_UPDATE
{
	//system("cls");
	_CCD_DATA_DEF ui_times = clock();

_CCD_CLEAR(&c_canvas);

if (GetAsyncKeyState(VK_ADD) != 0)
{
	switchrole();
	Sleep(1000);
}
while (GetAsyncKeyState(VK_RETURN) != 0)
{
	if (GetAsyncKeyState(VK_RETURN) != 0 && !ui_currentshooting)
		initshooting();
}

render_tanks();
render_lines();
if (!ui_currentshooting)movetanks();
else shoot();


_CCD_DRAWBUFFER(&c_canvas);



ui_fps += clock() - ui_times;
ui_pastframes++;
if (ui_fps >= 1000)
{
	ui_lastfps = ui_pastframes;
	ui_pastframes = 0;
	ui_fps = 0;
}



//printf("TIME:\t\t%d ms\nSHOOTTIME:\t%d \nFRAMES:\t\t%d fps\nSHOOTING:\t%d\nPLAYER:\t\t%d\n", clock() - ui_times, ui_shooting_time, ui_lastfps, ui_currentshooting, ui_currentplayer);
//printf("X: \t\t%d \nY: \t\t%d\n", s_shell.V_POSITION.X, s_shell.V_POSITION.Y);
//printf("X: \t\t%d \nY: \t\t%d", c_tank[ui_currentplayer].V_POSITION.X, c_tank[ui_currentplayer].V_POSITION.Y);


return 0;
}
_CCD_END
{
	_CCD_DELETE(&c_canvas);
}
