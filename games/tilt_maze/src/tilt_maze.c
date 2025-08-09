#include "tilt_maze.h"

TaskHandle_t tilt_maze_task_handle;

static const uint8_t maze_map[MAZE_HEIGHT][MAZE_WIDTH] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 2},
    {1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1},
    {1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1},
    {1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
  };

static player_pos_t player = {1, 1};

void draw_maze(void)
{
  ssd1306_clear_buffer();

  for (int y = 0; y < MAZE_HEIGHT; y++)
  {
    for (int x = 0; x < MAZE_WIDTH; x++)
    {
      if (maze_map[y][x] == 1)
      {
        ssd1306_draw_rect(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, true);
      }
    }
  }

  ssd1306_draw_circle(player.x * TILE_SIZE + TILE_SIZE / 2,
                      player.y * TILE_SIZE + TILE_SIZE / 2,
                      2, true);

  ssd1306_update_display();
}

int update_player_from_tilt(void)
{
  float ax, ay, az;
  if (mpu6050_read_acceleration(&ax, &ay, &az) != ESP_OK)
    return 0;

  static TickType_t last_move = 0;
  TickType_t now = xTaskGetTickCount();
  if (now - last_move < pdMS_TO_TICKS(200))
    return 0; // Reduzido para mais responsivo

  int dx = 0, dy = 0;
  float threshold = 0.15f; // Limiar reduzido para mais sensibilidade

  if (ay > threshold)
    dx = 1;
  else if (ay < -threshold)
    dx = -1;

  if (ax > threshold)
    dy = 1; // Inclinar para frente -> move para baixo
  else if (ax < -threshold)
    dy = -1; // Inclinar para trás -> move para cima

  // Apenas move se detectou movimento
  if (dx != 0 || dy != 0)
  {
    int new_x = player.x + dx;
    int new_y = player.y + dy;

    if (new_x >= 0 && new_x < MAZE_WIDTH &&
        new_y >= 0 && new_y < MAZE_HEIGHT &&
        (maze_map[new_y][new_x] == 0 || maze_map[new_y][new_x] == 2))
    {
      player.x = new_x;
      player.y = new_y;
      draw_maze();
      if (maze_map[new_y][new_x] == 2)
      {
        return 1;
      }

      last_move = now;

      // Debug: mostrar movimento
      printf("Movimento: dx=%d, dy=%d, pos=(%d,%d)\n", dx, dy, player.x, player.y);
    }
    // return maze_map[new_y][new_x] == 2;
  }

  return 0;
}

void finish()
{
  ssd1306_clear_buffer();
  ssd1306_draw_string(20, 20, "You won!");
  ssd1306_update_display();
}

void tilt_maze_task(void*);

void start_tilt_maze_game(void)
{
  ESP_LOGI("TILT_MAZE", "Game started");
  player.x = 1;
  player.y = 1;

  draw_maze();

  xTaskCreate(
    tilt_maze_task,
    "tilt_maze_task",
    4096,
    NULL,
    1,
    &tilt_maze_task_handle
  );
}

void tilt_maze_task(void*)
{
  ESP_LOGI("TILT_MAZE", "Game started");
  player.x = 1;
  player.y = 1;

  draw_maze();

  while (1)
  {
    if (update_player_from_tilt())
    {
      finish();
      game_win();
      vTaskDelete(tilt_maze_task_handle);
    }

    vTaskDelay(pdMS_TO_TICKS(100));
  }
}