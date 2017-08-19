/*
	VitaPong
	Copyright (C) 2016-2017, NamelessGhoul0

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <psp2/ctrl.h>
#include <psp2/display.h>
#include <psp2/touch.h>
#include <vita2d.h>

#define BLACK   RGBA8(0, 0, 0, 255)
#define WHITE	RGBA8(255, 255, 255, 255)

#define SCE_CTRL_ANY (SCE_CTRL_UP | SCE_CTRL_DOWN | SCE_CTRL_LEFT | SCE_CTRL_RIGHT | SCE_CTRL_TRIANGLE | SCE_CTRL_CROSS | SCE_CTRL_SQUARE | SCE_CTRL_CIRCLE | SCE_CTRL_LTRIGGER | SCE_CTRL_RTRIGGER | SCE_CTRL_SELECT | SCE_CTRL_START)

void credits(vita2d_font *font) {
	SceCtrlData ctrl;
	int y_pos = 544;

	while (1) {
		sceCtrlPeekBufferPositive(0, &ctrl, 1);

		if (ctrl.buttons & SCE_CTRL_ANY) {
			break;
		}

		y_pos--;

		vita2d_start_drawing();
		vita2d_clear_screen();

		vita2d_font_draw_textf(font, 100, y_pos, WHITE, 30, "Main Developer: NamelessGhoul0");
		vita2d_font_draw_textf(font, 125, y_pos+100, WHITE, 30, "LiveArea Design: ACViperPro");
		vita2d_font_draw_textf(font, 300, y_pos+200, WHITE, 30, "RIP Ralph Baer");

		vita2d_end_drawing();
		vita2d_swap_buffers();
		sceDisplayWaitVblankStart();

		if ((y_pos+200) < 0) {
			break;
		}
	}
	return;
}
