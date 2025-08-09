#include "paddle_pong.h"

static const char *TAG = "PADDLE_PONG";

// Variáveis globais do jogo
static game_state_t game;

const char* PADDLE_PONG_SCORE_FILE = "/files/paddle_pong.txt";

TaskHandle_t paddle_pong_game_task_handle;

// Função para inicializar o jogo
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

// Função para resetar a bola
void reset_ball()
{
    game.ball.x = SSD1306_WIDTH / 2;
    game.ball.y = SSD1306_HEIGHT / 2;

    // Velocidade aleatória
    game.ball.vx = (rand() % 2 == 0) ? 3.f : -3.f;
    game.ball.vy = 2;
}

// Função para atualizar a posição da raquete baseada no MPU6050
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

// Função para verificar colisão entre bola e raquete
bool check_paddle_collision()
{
    return (game.ball.x + game.ball.size >= game.paddle.x &&
            game.ball.x <= game.paddle.x + game.paddle.width &&
            game.ball.y + game.ball.size >= game.paddle.y &&
            game.ball.y <= game.paddle.y + game.paddle.height);
}

// Função para atualizar a física da bola
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

    // Colisão com raquete
    if (check_paddle_collision() && game.ball.vy > 0)
    {
        catch_sound();
        game.ball.vy = -game.ball.vy;
        game.score += 1;

        // Adicionar efeito baseado na posição de colisão na raquete
        float hit_pos = (game.ball.x - game.paddle.x) / (float)game.paddle.width;
        game.ball.vx += (hit_pos - 0.5f) * 2.0f; // Efeito de "spin"

        // Limitar velocidade máxima
        if (game.ball.vx > 12.f)
            game.ball.vx = 12.0f;
        if (game.ball.vx < -12.0f)
            game.ball.vx = -12.0f;
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

// Função para desenhar o jogo
void draw_game()
{
    // Limpar buffer
    ssd1306_clear_buffer();

    if (game.game_over)
    {
        // Tela de game over
        ssd1306_draw_string(10, 10, "GAME OVER");
        char score_str[32];
        snprintf(score_str, sizeof(score_str), "Score: %d", game.score);
        ssd1306_draw_string(5, 25, score_str);
    }
    else
    {
        // Desenhar raquete
                                                                                                                                                                                                                                                                                      
        ssd1306_draw_rect(game.paddle.x, game.paddle.y,
                          game.paddle.width, game.paddle.height, true);

        // Desenhar bola
        ssd1306_draw_circle((int)game.ball.x + game.ball.size / 2,
                            (int)game.ball.y + game.ball.size / 2,
                            game.ball.size, true);

        // Desenhar HUD (Score e Vidas)
        char hud_str[32];
        snprintf(hud_str, sizeof(hud_str), "pt: %d", game.score);
        ssd1306_draw_string(0, 1, hud_str);

        snprintf(hud_str, sizeof(hud_str), "vida: %d", game.lives);
        ssd1306_draw_string(50, 0, hud_str);

        ssd1306_draw_line(0, 10, SSD1306_WIDTH - 1, 10);
    }

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

// Task principal do jogo
void game_task(void *pvParameters)
{
    ESP_LOGI(TAG, "Iniciando task do jogo...");

    while (1)
    {
        if (game.game_over)
        {
            break;
        }

        update_paddle();
        update_ball();

        draw_game();

        // Controle de FPS
        vTaskDelay(pdMS_TO_TICKS(75));
    }
    
    if (update_score(PADDLE_PONG_SCORE_FILE, game.score)) {
        game_win();
    } else {
        game_lose();
    }
    vTaskDelete(paddle_pong_game_task_handle);
}

// Função principal
void start_paddle_pong_game()
{
    game_init();

    xTaskCreate(game_task, "game_task", 4096, NULL, 5, &paddle_pong_game_task_handle);
}
