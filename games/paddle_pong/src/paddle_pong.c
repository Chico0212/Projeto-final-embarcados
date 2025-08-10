#include "paddle_pong.h"

static const char *TAG = "PADDLE_PONG";

static game_state_t game;

const char *PADDLE_PONG_SCORE_FILE = "/files/pong.txt";

TaskHandle_t paddle_pong_game_task_handle;

void game_init()
{
    ESP_LOGI(TAG, "Inicializando jogo...");

    // Inicializar raquete
    game.paddle.width = PADDLE_WIDTH;
    game.paddle.height = PADDLE_HEIGHT;
    game.paddle.x = (SSD1306_WIDTH - PADDLE_WIDTH) / 2;
    game.paddle.y = SSD1306_HEIGHT - 10;

    // Inicializar bola
    game.ball.x = SSD1306_WIDTH / 2;
    game.ball.y = SSD1306_HEIGHT / 2;
    game.ball.vx = 1.5f;
    game.ball.vy = 1.0f;
    game.ball.size = BALL_SIZE;

    // Inicializar estado do jogo
    game.score = 0;
    game.lives = 3;
    game.game_over = false;
    game.paused = false;

    ESP_LOGI(TAG, "Jogo inicializado!");
}

void reset_ball()
{
    game.ball.x = SSD1306_WIDTH / 2;
    game.ball.y = SSD1306_HEIGHT / 2;

    // Velocidade aleatória
    game.ball.vx = (rand() % 2 == 0) ? 5.f : -5.f;
    game.ball.vy = 5;
}

void update_paddle()
{
    float ax, ay, az;

    // Ler aceleração do MPU6050
    if (mpu6050_read_acceleration(&ax, &ay, &az) == ESP_OK)
    {
        // Usar aceleração X para controlar a raquete
        // Mapear de -1g a +1g para largura da tela
        int new_x = game.paddle.x + (int)(ay * 30); // Sensibilidade ajustável

        // Limitar dentro da tela
        if (new_x < 0)
            new_x = 0;
        if (new_x > SSD1306_WIDTH - PADDLE_WIDTH)
            new_x = SSD1306_WIDTH - PADDLE_WIDTH;

        game.paddle.x = new_x;
    }
}

bool check_paddle_collision()
{
    return (game.ball.x + game.ball.size >= game.paddle.x &&
            game.ball.x <= game.paddle.x + game.paddle.width &&
            game.ball.y + game.ball.size >= game.paddle.y &&
            game.ball.y <= game.paddle.y + game.paddle.height);
}

void update_ball()
{
    // Atualizar posição
    float speed_factor = 1.2f;
    game.ball.x += game.ball.vx * speed_factor;
    game.ball.y += game.ball.vy * speed_factor;

    // Colisão com paredes laterais
    if (game.ball.x <= 0 || game.ball.x >= SSD1306_WIDTH - game.ball.size)
    {
        game.ball.vx = -game.ball.vx;
        if (game.ball.x <= 0)
            game.ball.x = 0;
        if (game.ball.x >= SSD1306_WIDTH - game.ball.size)
            game.ball.x = SSD1306_WIDTH - game.ball.size;
    }

    // Colisão com parede superior
    if (game.ball.y <= 0)
    {
        game.ball.vy = -game.ball.vy;
        game.ball.y = 0;
    }

    if (check_paddle_collision() && game.ball.vy > 0)
    {
        hit_ball();
        game.ball.vy = -game.ball.vy;
        game.score += 1;
        float hit_pos = (game.ball.x - game.paddle.x) / (float)game.paddle.width;
        game.ball.vx += (hit_pos - 0.5f) * 2.0f;

        // Limitar velocidade máxima
        if (game.ball.vx > 25.f)
            game.ball.vx = 25.0f;
        if (game.ball.vx < -25.0f)
            game.ball.vx = -25.0f;
    }

    // Bola saiu pela parte inferior (perdeu uma vida)
    if (game.ball.y >= SSD1306_HEIGHT)
    {
        game.lives--;
        if (game.lives <= 0)
        {
            game.game_over = true;
        }
        else
        {
            reset_ball();
        }
    }
}

void draw_game()
{
    ssd1306_clear_buffer();

    ssd1306_draw_rect(game.paddle.x, game.paddle.y,
                      game.paddle.width, game.paddle.height, true);

    ssd1306_draw_circle((int)game.ball.x + game.ball.size / 2,
                        (int)game.ball.y + game.ball.size / 2,
                        game.ball.size, true);

    // Desenhar Score e Vidas
    char hud_str[32];
    snprintf(hud_str, sizeof(hud_str), "Pts: %d", game.score);
    ssd1306_draw_string(0, 1, hud_str);

    snprintf(hud_str, sizeof(hud_str), "Vida: %d", game.lives);

    int placement = SSD1306_WIDTH - strlen(hud_str) * 8;
    ssd1306_draw_string(placement, 0, hud_str);

    ssd1306_draw_line(0, 10, SSD1306_WIDTH - 1, 10);

    ssd1306_update_display();
    // game_lose();
}

bool check_restart()
{
    float ax, ay, az;

    if (mpu6050_read_acceleration(&ax, &ay, &az) == ESP_OK)
    {
        // Se inclinar muito para um lado, reinicia
        return (fabs(ax) > 0.8f);
    }
    return false;
}

void game_task(void *pvParameters)
{
    ESP_LOGI(TAG, "Iniciando task do jogo...");
    uint32_t notif;

    while (1)
    {
        if (xTaskNotifyWait(0, NOTIF_STOP, &notif, 0) == pdTRUE)
        {
            break;
        }
        if (game.game_over)
        {
            update_score(PADDLE_PONG_SCORE_FILE, game.score) ? game_win() : game_lose();
            show_game_over(PADDLE_PONG_SCORE_FILE, game.score);
            break;
        }

        update_paddle();
        update_ball();

        draw_game();

        // Controle de FPS
        vTaskDelay(pdMS_TO_TICKS(75));
    }

    vTaskDelete(paddle_pong_game_task_handle);
}

void start_paddle_pong_game()
{
    game_init();

    xTaskCreate(game_task, "game_task", 4096, NULL, 5, &paddle_pong_game_task_handle);
}
