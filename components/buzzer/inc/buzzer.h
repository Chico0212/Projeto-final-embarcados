#include "driver/ledc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define BUZZER_GPIO          GPIO_NUM_18

#define NOTE_C4  262
#define NOTE_D4  294
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_G4  392
#define NOTE_A4  440
#define NOTE_A5  880
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_D5  587
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_G5  784

#define QUARTER_NOTE  300
#define EIGHTH_NOTE   150
#define SIXTEENTH_NOTE 75

void buzzer_pwm_init();

void buzzer_play_note(uint32_t frequency, uint32_t duration_ms);

void buzzer_victory_melody();

void buzzer_defeat_melody();

void buzzer_on();

void buzzer_off();

void game_win();

void game_lose();

void catch_sound();

void buzzer_mario_theme();
