#include "snake.h"

static Point snake[MAX_SNAKE_LEN];
static int snake_len = 3;
static int dx = TILE_SIZE;
static int dy = 0;
static Point fruit;
static bool game_over = false;
static int score = 0;
const char *SCORE_FILE_SNAKE = "/files/snake.txt";

TaskHandle_t snake_game_task_handle;

void draw_tile(int x, int y, char symbol)
{
  for (int i = 0; i < TILE_SIZE; i++)
  {
    for (int j = 0; j < TILE_SIZE; j++)
    {
      ssd1306_set_pixel(x + j, y + i, true);
    }
  }
}

void place_fruit()
{
  fruit.x = (rand() % (DISPLAY_WIDTH / TILE_SIZE)) * TILE_SIZE;
  fruit.y = (rand() % (DISPLAY_HEIGHT / TILE_SIZE)) * TILE_SIZE;
}

void reset_snake_game()
{
  snake_len = 3;
  snake[0] = (Point){.x = 40, .y = 32};
  snake[1] = (Point){.x = 36, .y = 32};
  snake[2] = (Point){.x = 32, .y = 32};
  dx = TILE_SIZE;
  dy = 0;
  game_over = false;
  score = 0;
  place_fruit();
}

bool collision(Point head)
{
  if (head.x < 0 || head.x >= DISPLAY_WIDTH || head.y < 0 || head.y >= DISPLAY_HEIGHT)
    return true;
  for (int i = 1; i < snake_len; i++)
  {
    if (head.x == snake[i].x && head.y == snake[i].y)
      return true;
  }
  return false;
}

void snake_game_task(void *)
{
  ESP_LOGI("SNAKE", "Iniciando Snake...");
  reset_snake_game();
  mpu6050_data_t sensor_data;
  float accel[3];
  float unused_gyro[3];
  float unused_temp;
  char score_text[10];

  uint32_t notif;
  while (1)
  {
    if (xTaskNotifyWait(0, NOTIF_STOP, &notif, 0) == pdTRUE)
    {
      break;
    }
    if (game_over)
    {
      update_score(SCORE_FILE_SNAKE, score) ? game_win() : game_lose();
      show_game_over(SCORE_FILE_SNAKE, score);
      break;
    }

    if (mpu6050_read_all(&sensor_data) == ESP_OK)
    {
      mpu6050_convert_data(&sensor_data, accel, unused_gyro, &unused_temp);
      if (accel[1] < 0.5f && dx == 0)
      {
        dx = -TILE_SIZE;
        dy = 0; // Ir para a esquerda, se não estiver indo na horizontal
      }
      else if (accel[1] > -0.5f && dx == 0)
      {
        dx = TILE_SIZE;
        dy = 0; // Ir para a direita, se não estiver indo na horizontal
      }

      // Movimentos no eixo vertical (cima/baixo) — AGORA baseados no eixo accel[0]
      else if (accel[0] > 0.5f && dy == 0)
      {
        dx = 0;
        dy = TILE_SIZE; // Ir para baixo, se não estiver indo na vertical
      }
      else if (accel[0] < -0.5f && dy == 0)
      {
        dx = 0;
        dy = -TILE_SIZE; // Ir para cima, se não estiver indo na vertical
      }

      ESP_LOGI("SNAKE", "X: %d, Y: %d", dx, dy);
    }

    Point new_head = {snake[0].x + dx, snake[0].y + dy};

    if (collision(new_head))
    {
      game_over = true;
      continue;
    }

    for (int i = snake_len; i > 0; i--)
    {
      snake[i] = snake[i - 1];
    }
    snake[0] = new_head;

    if (new_head.x == fruit.x && new_head.y == fruit.y)
    {
      catch_sound();
      if (snake_len < MAX_SNAKE_LEN - 1)
        snake_len++;
      score++;
      place_fruit();
    }
    else
    {
      snake_len = (snake_len > 2) ? snake_len : 3;
    }

    ssd1306_clear_buffer();
    snprintf(score_text, sizeof(score_text), "Score: %d", score);
    ssd1306_draw_string(0, 0, score_text);

    for (int i = 0; i < snake_len; i++)
    {
      draw_tile(snake[i].x, snake[i].y, SNAKE_SYMBOL);
    }
    draw_tile(fruit.x, fruit.y, FRUIT_SYMBOL);
    ssd1306_update_display();
    vTaskDelay(pdMS_TO_TICKS(100));
  }

  vTaskDelete(snake_game_task_handle);
}

void start_snake_game(void)
{
  ESP_LOGI("SNAKE", "Iniciando Snake...");
  reset_snake_game();

  xTaskCreate(
      snake_game_task,
      "snake_game_task",
      4096,
      NULL,
      1,
      &snake_game_task_handle);
}
