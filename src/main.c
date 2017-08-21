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

#include "audio.h"
#include "credits.h"
#include "menu.h"
#include "splash.h"

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
	if (left_score > 11) {
		vita2d_font_draw_textf(font, 530, 300, WHITE, 60, "Winner");
		vita2d_end_drawing();
		vita2d_swap_buffers();
		sceKernelDelayThread(5 * 1000 * 1000);
		score.left = 0;
		score.right = 0;
		main_menu(font);
	}
	if (right_score > 11) {
		vita2d_font_draw_textf(font, 70, 300, WHITE, 60, "Winner");
		vita2d_end_drawing();
		vita2d_swap_buffers();
		sceKernelDelayThread(5 * 1000 * 1000);
		score.left = 0;
		score.right = 0;
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
		play_pong_wall();
		reverse_ball_direction(); // bounce off top block
	}
	if (ball.y > 514-20) {
		play_pong_wall();
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
		ball.x = 30; // move ball in front of the paddle
		play_pong_paddle();
		reverse_ball_direction_from_paddle();
	}

	// check for right paddle collision
	right_paddle.face.top = right_paddle.pos;
	right_paddle.face.bottom = (right_paddle.face.top + 100);

	if ((ball.x + 20) > 930 && (ball.y + 20) > right_paddle.face.top && ball.y < right_paddle.face.bottom) {
		ball.x = (930 - 20); // move ball in front of the paddle
		play_pong_paddle();
		reverse_ball_direction_from_paddle();
	}

	// check if ball made it past the paddle
	if (ball.x < (30 - 1 - ball.speed.x)) {
		play_pong_score();
		score.left++;
		set_ball();
	}
	if (ball.x > (930 + 1 + ball.speed.x)) {
		play_pong_score();
		score.right++;
		set_ball();
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
	audio_init();

	sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG);

	//show_splash();

	main_menu(font);

	while (1) {
		sceCtrlPeekBufferPositive(0, &ctrl, 1);

		// start render
		vita2d_start_drawing();
		vita2d_clear_screen();

		// control left paddle
		if (ctrl.ly < 100) {
			left_paddle.pos -= left_paddle.speed; // move up
		}
		if (ctrl.ly > 140) {
			left_paddle.pos += left_paddle.speed; // move down
		}

		// control right paddle
		if (ctrl.ry < 100) {
			right_paddle.pos -= right_paddle.speed; // move up
		}
		if (ctrl.ry > 140) {
			right_paddle.pos += right_paddle.speed; // move down
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
