#ifndef TILT_MAZE_H
#define TILT_MAZE_H

#include "ssd1306.h"
#include "mpu6050.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "buzzer.h"


// Maze definitions
#define MAZE_WIDTH 16
#define MAZE_HEIGHT 8
#define TILT_MAZE_TILE_SIZE 8
typedef struct {
    int x, y;
} player_pos_t;

typedef struct {
    int x, y;
} goal_pos_t;

extern TaskHandle_t tilt_maze_task_handle;

void draw_maze(void);

int update_player_from_tilt(void);

void start_tilt_maze_game(void);

#endif
