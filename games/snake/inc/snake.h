#ifndef SNAKE_H
#define SNAKE_H

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "ssd1306.h"
#include "mpu6050.h"
#include <stdlib.h>
#include <buzzer.h>
#include <sd_card.h>

#define DISPLAY_WIDTH  128
#define DISPLAY_HEIGHT 64
#define SNAKE_SIZE     4
#define MAX_SNAKE_LEN  64
#define GAME_SPEED_MS  150
#define FRUIT_SYMBOL   '*'
#define SNAKE_SYMBOL   '#'
#define TILE_SIZE      4

typedef struct {
    int x, y;
} Point;

extern TaskHandle_t snake_game_task_handle;

void draw_tile(int x, int y, char symbol);

void place_fruit();

void reset_snake_game();

bool collision(Point head);

void start_snake_game(void);

#endif