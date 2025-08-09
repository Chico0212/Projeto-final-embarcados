#ifndef PADDLE_PONG_H
#define PADDLE_PONG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_system.h"
#include "mpu6050.h"
#include "ssd1306.h"
#include "buzzer.h"
#include "sd_card.h"

#define PADDLE_WIDTH 20
#define PADDLE_HEIGHT 3
#define BALL_SIZE 2
#define GAME_SPEED 50  // ms entre frames

typedef struct {
    float x, y;      // Posição
    float vx, vy;    // Velocidade
    int size;        // Tamanho
} ball_t;

typedef struct {
    int x, y;        // Posição
    int width, height;
} paddle_t;

typedef struct {
    ball_t ball;
    paddle_t paddle;
    int score;
    int lives;
    bool game_over;
    bool paused;
} game_state_t;

extern TaskHandle_t paddle_pong_game_task_handle;

void game_init();

void reset_ball();

void update_paddle();

bool check_paddle_collision();

void update_ball();

void draw_game();

bool check_restart();

void game_task(void *pvParameters);

void start_paddle_pong_game();

#endif
