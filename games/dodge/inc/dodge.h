#ifndef DODGE_H
#define DODGE_H 

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "ssd1306.h"
#include "mpu6050.h"

// GPIO definitions for buttons
#define BUTTON_UP_GPIO       GPIO_NUM_4   // Button for navigating up/previous
#define BUTTON_SELECT_GPIO   GPIO_NUM_5   // Button for selecting/confirming

// Display dimensions (assuming standard SSD1306)
#define DISPLAY_WIDTH        128
#define DISPLAY_HEIGHT       64

// Menu configuration
#define MAX_MENU_ITEMS       4
#define MENU_ITEM_HEIGHT     10
#define MENU_START_Y         16
#define MENU_ITEM_SPACING    2
#define TITLE_HEIGHT         12
#define INSTRUCTION_HEIGHT   8

// Menu layout definitions
#define SELECTION_ARROW_X    2
#define MENU_TEXT_X          12 
#define SELECTION_BOX_MARGIN 1
#define TITLE_CENTER_X       32
#define INSTRUCTIONS_Y       (DISPLAY_HEIGHT - INSTRUCTION_HEIGHT)

// Button debounce time in milliseconds
#define BUTTON_DEBOUNCE_MS   50

// === INÍCIO DO JOGO "DODGE THE BLOCKS" ===
#define PLAYER_WIDTH  8
#define PLAYER_HEIGHT 8
#define PLAYER_Y      (DISPLAY_HEIGHT - PLAYER_HEIGHT - 2)
#define BLOCK_WIDTH   6
#define BLOCK_HEIGHT  6
#define MAX_BLOCKS    5
#define BLOCK_SPEED_INITIAL 1.0f
#define GAME_DELAY_MS 50

typedef struct {
    int x, y;
    float speed;
    bool active;
} Block;

void draw_player();

void draw_block(Block* b);

void reset_game();

bool check_collision(Block* b);

void start_dodge_blocks_game(void);

#endif