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

#include <psp2/kernel/processmgr.h>
#include <psp2/display.h>
#include <vita2d.h>

extern unsigned char gekihen[];
extern unsigned int gekihen_size;

void show_splash(void) {
	int alpha = 1;
	int done = 0;

	vita2d_texture *splash = vita2d_load_PNG_buffer(gekihen);

	while(1) {
		if (alpha < 255 && !done) {
			alpha += 2;
		} else {
			if (!done) {
				sceKernelDelayThread(1 * 1000 * 1000);
			}
			done = 1;
			alpha -= 2;
		}

		if (alpha <= 0)
			break;

		vita2d_start_drawing();
		vita2d_clear_screen();
		vita2d_draw_texture_tint(splash, 0, 0, RGBA8(255, 255, 255, alpha));
		vita2d_end_drawing();
		vita2d_swap_buffers();
		sceDisplayWaitVblankStart();
	}
	return;
}
