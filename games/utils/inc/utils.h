#ifndef UTILS_H
#define UTILS_H 

#include "sd_card.h"
#include "buzzer.h"
#include "ssd1306.h"

// grafico
void show_game_over(const char* file_path, int score);

// memória
int update_score(const char *file_path, int new_score);

int read_score_from_file(const char* file_path);

// som
void game_win();

void game_lose();

void catch_sound();

void buzzer_mario_theme();

void hit_ball();

#endif