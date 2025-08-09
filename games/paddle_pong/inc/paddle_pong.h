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
#include "utils.h"

// Configurações do jogo
#define PADDLE_WIDTH 20
#define PADDLE_HEIGHT 3
#define BALL_SIZE 2
#define GAME_SPEED 50  // ms entre frames

// Estrutura da bola
typedef struct {
    float x, y;      // Posição
    float vx, vy;    // Velocidade
    int size;        // Tamanho
} ball_t;

// Estrutura da raquete
typedef struct {
    int x, y;        // Posição
    int width, height;
} paddle_t;

// Estrutura do estado do jogo
typedef struct {
    ball_t ball;
    paddle_t paddle;
    int score;
    int lives;
    bool game_over;
    bool paused;
} game_state_t;

extern TaskHandle_t paddle_pong_game_task_handle;

// Função para inicializar o jogo
void game_init();

// Função para resetar a bola
void reset_ball();

// Função para atualizar a posição da raquete baseada no MPU6050
void update_paddle();

// Função para verificar colisão entre bola e raquete
bool check_paddle_collision();

// Função para atualizar a física da bola
void update_ball();

// Função para desenhar o jogo
void draw_game();

// Função para verificar se o jogador quer reiniciar
bool check_restart();

// Task principal do jogo
void game_task(void *pvParameters);

// Função principal
void start_paddle_pong_game();

#endif
