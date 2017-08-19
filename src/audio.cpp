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

#ifdef __cplusplus
extern "C" {
#endif

#include "soloud.h"
#include "soloud_wav.h"

SoLoud::Soloud gSoloud;
SoLoud::Wav gWave;

void audio_init() {
	gSoloud.init();
}

void play_pong_paddle() {
	gWave.load("app0:/pong_paddle.ogg");
	gSoloud.play(gWave);
}

void play_pong_score() {
	gWave.load("app0:/pong_score.ogg");
	gSoloud.play(gWave);
}

void play_pong_wall() {
	gWave.load("app0:/pong_wall.ogg");
	gSoloud.play(gWave);
}

#ifdef __cplusplus
}
#endif
