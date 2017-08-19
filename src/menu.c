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

#include <psp2/display.h>
#include <psp2/touch.h>
#include <vita2d.h>

#include "audio.h"
#include "credits.h"

#define lerp(value, from_max, to_max) ((((value*10) * (to_max*10))/(from_max*10))/10)

#define BLACK   RGBA8(0, 0, 0, 255)
#define WHITE	RGBA8(255, 255, 255, 255)

void main_menu(vita2d_font *font) {
	SceTouchData touch;
	int touchx;
	int touchy;

	sceTouchSetSamplingState(SCE_TOUCH_PORT_FRONT, 1);

	while (1) {
		sceTouchPeek(SCE_TOUCH_PORT_FRONT, &touch, 1);

		if (touch.reportNum > 0) {
			touchx = lerp(touch.report[0].x, 1919, 960);
			touchy = lerp(touch.report[0].y, 1087, 544);

			if ((touchx > 370) && (touchy > 210) && (touchx < 570) && (touchy < 260)) {
				play_pong_score();
				break;
			} else if ((touchx > 360) && (touchy > 310) && (touchx < 590) && (touchy < 360)) {
				play_pong_score();
				credits(font);
			}
		}

		vita2d_start_drawing();
		vita2d_clear_screen();

		// box around the start button
		vita2d_draw_rectangle(370, 210, 200, 50, WHITE);
		vita2d_draw_rectangle(370+2, 210+2, 200-4, 50-4, BLACK);

		// box around the credits button
		vita2d_draw_rectangle(360, 310, 230, 50, WHITE);
		vita2d_draw_rectangle(360+2, 310+2, 230-4, 50-4, BLACK);

		vita2d_font_draw_textf(font, 365, 130, WHITE, 60, "PONG");
		vita2d_font_draw_textf(font, 395, 250+3, WHITE, 30, "START");
		vita2d_font_draw_textf(font, 380, 350+3, WHITE, 30, "CREDITS");

		vita2d_end_drawing();
		vita2d_swap_buffers();
		sceDisplayWaitVblankStart();
	}
	return;
}
