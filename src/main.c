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

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <psp2/ctrl.h>
#include <psp2/display.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/touch.h>
#include <vita2d.h>

#include "splash.h"
#include "Audio.h"

AudioHandler bgm_audio;
AudioHandler pong_audio;

#define lerp(value, from_max, to_max) ((((value*10) * (to_max*10))/(from_max*10))/10)

#define BLACK   RGBA8(0, 0, 0, 255)
#define WHITE	RGBA8(255, 255, 255, 255)

extern unsigned char pixeled[];
extern unsigned int pixeled_size;

// ball struct
struct {
	int x;
	int y;
	struct {
		int x;
		int y;
	} speed;
	enum {
		LEFT_UP,
		LEFT_DOWN,
		RIGHT_UP,
		RIGHT_DOWN
	} direction;
} ball;

// left paddle struct
struct {
	int pos;
	int speed;
	struct {
		int top; // (paddle.pos + 20) = paddle.face.top
		int bottom; // (paddle.face.top + 100) = paddle.face.bottom
	} face;
} left_paddle;

// right paddle struct
struct {
	int pos;
	int speed;
	struct {
		int top; // paddle.pos = paddle.face.top
		int bottom; // (paddle.face.top + 100) = paddle.face.bottom
	} face;
} right_paddle;

struct {
	int left;
	int right;
} score;

void draw_court(void) {
	// top block
	vita2d_draw_rectangle(10, 10, 960-20, 20, WHITE);
	// bottom block
	vita2d_draw_rectangle(10, 544-30, 960-20, 20, WHITE);
	// center dotted line
	vita2d_draw_rectangle(480-20, 30+10, 20, 20, WHITE);
	vita2d_draw_rectangle(480-20, 70+10, 20, 20, WHITE);
	vita2d_draw_rectangle(480-20, 110+10, 20, 20, WHITE);
	vita2d_draw_rectangle(480-20, 150+10, 20, 20, WHITE);
	vita2d_draw_rectangle(480-20, 190+10, 20, 20, WHITE);
	vita2d_draw_rectangle(480-20, 230+10, 20, 20, WHITE);
	vita2d_draw_rectangle(480-20, 270+10, 20, 20, WHITE);
	vita2d_draw_rectangle(480-20, 310+10, 20, 20, WHITE);
	vita2d_draw_rectangle(480-20, 350+10, 20, 20, WHITE);
	vita2d_draw_rectangle(480-20, 390+10, 20, 20, WHITE);
	vita2d_draw_rectangle(480-20, 430+10, 20, 20, WHITE);
	vita2d_draw_rectangle(480-20, 470+10, 20, 20, WHITE);
	return;
}

void draw_score(vita2d_font *font, char *left_score, char *right_score) {
	vita2d_font_draw_textf(font, 500, 110, WHITE, 60, left_score);
	vita2d_font_draw_textf(font, 390, 110, WHITE, 60, right_score);
	return;
}

void check_score(vita2d_font *font, int left_score, int right_score) {
	if (left_score > 9) {
		vita2d_font_draw_textf(font, 530, 300, WHITE, 60, "Winner");
		vita2d_end_drawing();
		vita2d_swap_buffers();
		sceKernelDelayThread(5 * 1000 * 1000);
		main_menu(font);
	}
	if (right_score > 9) {
		vita2d_font_draw_textf(font, 70, 300, WHITE, 60, "Winner");
		vita2d_end_drawing();
		vita2d_swap_buffers();
		sceKernelDelayThread(5 * 1000 * 1000);
		main_menu(font);
	}
}

void reverse_ball_direction(void) {
	if (ball.direction == LEFT_UP) {
		ball.direction = LEFT_DOWN;
	} else if (ball.direction == LEFT_DOWN) {
		ball.direction = LEFT_UP;
	} else if (ball.direction == RIGHT_UP) {
		ball.direction = RIGHT_DOWN;
	} else if (ball.direction == RIGHT_DOWN) {
		ball.direction = RIGHT_UP;
	}
	return;
}

void reverse_ball_direction_from_paddle(void) {
	if (ball.direction == LEFT_UP) {
		ball.direction = RIGHT_UP;
	} else if (ball.direction == LEFT_DOWN) {
		ball.direction = RIGHT_DOWN;
	} else if (ball.direction == RIGHT_UP) {
		ball.direction = LEFT_UP;
	} else if (ball.direction == RIGHT_DOWN) {
		ball.direction = LEFT_DOWN;
	}
	return;
}

void calculate_ball_direction(void) {
	if (ball.direction == LEFT_UP) {
		ball.x -= ball.speed.x;
		ball.y -= ball.speed.y;
	} else if (ball.direction == LEFT_DOWN) {
		ball.x -= ball.speed.x;
		ball.y += ball.speed.y;
	} else if (ball.direction == RIGHT_UP) {
		ball.x += ball.speed.x;
		ball.y -= ball.speed.y;
	} else if (ball.direction == RIGHT_DOWN) {
		ball.x += ball.speed.x;
		ball.y += ball.speed.y;
	}
	return;
}

void set_ball(void) {
	// setup ball starting position
	ball.x = 460;
	ball.y = 262;
	ball.speed.x = 5;
	ball.speed.y = 5;

	// generate random number 0-3
	srand(time(NULL));
	int random = (rand() % 4);

	// set direction based on random number
	if (random == 0) {
		ball.direction = LEFT_UP;
	} else if (random == 1) {
		ball.direction = LEFT_DOWN;
	} else if (random == 2) {
		ball.direction = RIGHT_UP;
	} else if (random == 3) {
		ball.direction = RIGHT_DOWN;
	}

	return;
}

void check_collision() {
	/*
		top block:
			start - 10x10
			width x height - 940x20
		bottom block:
			start - 10x514
			width x height - 940x20
	*/

	// check for top & bottom block collision
	if (ball.y < 30) {
		play_pong_snd();
		reverse_ball_direction(); // bounce off top block
	}
	if (ball.y > 514-20) {
		play_pong_snd();
		reverse_ball_direction(); // bounce off bottom block
	}

	/*
		paddle size:
			width x height - 20x100
		left_paddle.pos starts at 10px from left side of screen
		right_paddle.pos starts at 930px from left side of screen
	*/

	// check for left paddle collision
	left_paddle.face.top = (left_paddle.pos + 20);
	left_paddle.face.bottom = (left_paddle.face.top + 100);

	if (ball.x < 30 && (ball.y + 20) > left_paddle.face.top && ball.y < left_paddle.face.bottom) {
		play_pong_snd();
		reverse_ball_direction_from_paddle();
	}

	// check for right paddle collision
	right_paddle.face.top = right_paddle.pos;
	right_paddle.face.bottom = (right_paddle.face.top + 100);

	if ((ball.x + 20) > 930 && (ball.y + 20) > right_paddle.face.top && ball.y < right_paddle.face.bottom) {
		play_pong_snd();
		reverse_ball_direction_from_paddle();
	}

	// check if ball made it past the paddle
	if (ball.x < (30 - 1 - ball.speed.x)) {
		score.left++;
		set_ball();
	}
	if (ball.x > (930 + 1 + ball.speed.x)) {
		score.right++;
		set_ball();
	}

	return;
}

void play_pong_snd() {
	InitializeAudio(&pong_audio);

	LoadOgg(&pong_audio, "app0:/pong.ogg", AUDIO_OUT_MAIN,0);

	PlayAudio(&pong_audio);
}

void play_bgm() {
	InitializeAudio(&bgm_audio);

	LoadOgg(&bgm_audio, "app0:/bgm.ogg", AUDIO_OUT_BGM,0);

	PlayAudio(&bgm_audio);
}

void credits(vita2d_font *font) {
	int y_pos = 544;

	while (1) {
		y_pos--;

		vita2d_start_drawing();
		vita2d_clear_screen();

		vita2d_font_draw_textf(font, 100, y_pos, WHITE, 30, "Main Developer: NamelessGhoul0");
		vita2d_font_draw_textf(font, 125, y_pos+100, WHITE, 30, "LiveArea Design: ACViperPro");
		vita2d_font_draw_textf(font, 180, y_pos+200, WHITE, 30, "Music: Pong by Eisenfunk");
		vita2d_font_draw_textf(font, 300, y_pos+300, WHITE, 30, "RIP Ralph Baer");

		vita2d_end_drawing();
		vita2d_swap_buffers();
		sceDisplayWaitVblankStart();

		if ((y_pos+300) < 0) {
			break;
		}
	}
	return;
}

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
				play_pong_snd();
				break;
			} else if ((touchx > 360) && (touchy > 310) && (touchx < 590) && (touchy < 360)) {
				play_pong_snd();
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

int main(void) {
	SceCtrlData ctrl;
	char left_score[1];
	char right_score[1];

	// setup paddle starting position
	left_paddle.pos = 272-50;
	right_paddle.pos = 272-50;
	left_paddle.speed = 4;
	right_paddle.speed = 4;

	score.left = 0;
	score.right = 0;

	set_ball();

	// init
	vita2d_init();
	vita2d_set_clear_color(BLACK);
	vita2d_font *font = vita2d_load_font_mem(pixeled, pixeled_size);

	sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG);

	show_splash();

	//play_bgm();

	main_menu(font);

	while (1) {
		sceCtrlPeekBufferPositive(0, &ctrl, 1);

		// start render
		vita2d_start_drawing();
		vita2d_clear_screen();

		// control left paddle
		if (ctrl.buttons & SCE_CTRL_UP) {
			left_paddle.pos -= left_paddle.speed;
		}
		if (ctrl.buttons & SCE_CTRL_DOWN) {
			left_paddle.pos += left_paddle.speed;
		}

		// control right paddle
		if (ctrl.buttons & SCE_CTRL_TRIANGLE) {
			right_paddle.pos -= right_paddle.speed;
		}
		if (ctrl.buttons & SCE_CTRL_CROSS) {
			right_paddle.pos += right_paddle.speed;
		}

		// make sure paddles dont go out of bounds
		if (left_paddle.pos < 30) {
			left_paddle.pos += left_paddle.speed;
		}
		if (left_paddle.pos > 414) {
			left_paddle.pos -= left_paddle.speed;
		}
		if (right_paddle.pos < 30) {
			right_paddle.pos += right_paddle.speed;
		}
		if (right_paddle.pos > 414) {
			right_paddle.pos -= right_paddle.speed;
		}

		calculate_ball_direction();
		check_collision();

		draw_court();

		// check to see if game has been won
		check_score(font, score.left, score.right);

		// display the current score
		itoa(score.left, left_score, 10);
		itoa(score.right, right_score, 10);
		draw_score(font, left_score, right_score);

		// draw ball
		vita2d_draw_rectangle(ball.x, ball.y, 20, 20, WHITE);

		// draw left paddle
		vita2d_draw_rectangle(10, left_paddle.pos, 20, 100, WHITE);

		// draw right paddle
		vita2d_draw_rectangle(960-30, right_paddle.pos, 20, 100, WHITE);

		// end render
		vita2d_end_drawing();
		vita2d_swap_buffers();
		sceDisplayWaitVblankStart();
	}

	vita2d_fini();
	sceKernelExitProcess(0);

    return 0;
}
