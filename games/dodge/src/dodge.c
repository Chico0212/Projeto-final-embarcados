// #include <stdio.h>
// #include <string.h>
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "freertos/queue.h"
// #include "driver/gpio.h"
// #include "esp_log.h"
// #include "ssd1306.h"
// #include "mpu6050.h"
// #include "dodge.h"
// #include "buzzer.h"

#include "dodge.h"

const char* SCORE_FILE = "/files/dodge.txt";

static int player_x = DISPLAY_WIDTH / 2;
static int score = 0;
static bool game_over = false;

static Block blocks[MAX_BLOCKS];

void draw_player()
{
    ssd1306_draw_rect(player_x, PLAYER_Y, PLAYER_WIDTH, PLAYER_HEIGHT, true);
}

void draw_block(Block *b)
{
    if (b->active)
        ssd1306_draw_rect(b->x, b->y, BLOCK_WIDTH, BLOCK_HEIGHT, true);
}

void reset_game()
{
    player_x = DISPLAY_WIDTH / 2;
    score = 0;
    game_over = false;
    for (int i = 0; i < MAX_BLOCKS; i++)
    {
        blocks[i].x = rand() % (DISPLAY_WIDTH - BLOCK_WIDTH);
        blocks[i].y = -(rand() % 40);
        blocks[i].speed = BLOCK_SPEED_INITIAL + (rand() % 30) / 10.0f;
        blocks[i].active = true;
    }
}

bool check_collision(Block *b)
{
    return (b->x < player_x + PLAYER_WIDTH && b->x + BLOCK_WIDTH > player_x &&
            b->y < PLAYER_Y + PLAYER_HEIGHT && b->y + BLOCK_HEIGHT > PLAYER_Y);
}

void start_dodge_blocks_game(void)
{
    reset_game();
    mpu6050_data_t sensor_data;
    float accel[3];
    float unused_gyro[3];
    float unused_temp;

    while (1)
    {
        if (game_over)
        {
            ssd1306_clear_buffer();
            ssd1306_draw_string(20, 20, "game over");
            char score_text[20];
            snprintf(score_text, sizeof(score_text), "score: %d", score);
            ssd1306_draw_string(20, 35, score_text);
            //   ssd1306_draw_string(0, 55, "press b0 to back");
            ssd1306_update_display();
            if (update_score(SCORE_FILE, score)) {
                game_win();
                return;
            }
            buzzer_defeat_melody();
            return;
        }

        esp_err_t err = mpu6050_read_all(&sensor_data);
        if (err == ESP_OK)
        {
            mpu6050_convert_data(&sensor_data, accel, unused_gyro, &unused_temp);
            player_x += (int)(accel[1] * 6);
        }
        else
        {
            ESP_LOGW("DODGE", "Falha ao ler sensor: %s", esp_err_to_name(err));
        }

        // Impede que o jogador saia pelas bordas laterais
        if (player_x < 0)
        {
            player_x = 0; // direita
        }

        if (player_x > SSD1306_WIDTH - PLAYER_WIDTH)
        {
            player_x = SSD1306_WIDTH - PLAYER_WIDTH; // esquerda
        }

        for (int i = 0; i < MAX_BLOCKS; i++)
        {
            if (!blocks[i].active)
                continue;
            blocks[i].y += (int)blocks[i].speed;
            if (blocks[i].y > DISPLAY_HEIGHT)
            {
                blocks[i].y = -(rand() % 30);
                blocks[i].x = rand() % (DISPLAY_WIDTH - BLOCK_WIDTH);
                blocks[i].speed += 0.1f;
                score++;
            }
            if (check_collision(&blocks[i]))
                game_over = true;
        }

        ssd1306_clear_buffer();
        draw_player();
        for (int i = 0; i < MAX_BLOCKS; i++)
            draw_block(&blocks[i]);
        char score_text[16];
        
        snprintf(score_text, sizeof(score_text), "%d", score);
        ssd1306_draw_string(DISPLAY_WIDTH - 30, 0, score_text);
        ssd1306_update_display();
        vTaskDelay(pdMS_TO_TICKS(GAME_DELAY_MS));
    }
}
