#include "tilt_maze.h"

#define MAX_LEVELS 5

TaskHandle_t tilt_maze_task_handle;
void tilt_maze_task(void *pvParameters);

int current_level = 0;
static player_pos_t player = {1, 1};
static goal_pos_t goal = {14, 1};

// Fase 1 - Tutorial básico
static const uint8_t maze_level_1[MAZE_HEIGHT][MAZE_WIDTH] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1},
    {1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

// Fase 2 - Labirinto simples
static const uint8_t maze_level_2[MAZE_HEIGHT][MAZE_WIDTH] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 2, 1},
    {1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1},
    {1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1},
    {1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

// Fase 3 - Labirinto com caminho em zigue-zague
static const uint8_t maze_level_3[MAZE_HEIGHT][MAZE_WIDTH] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 2, 1},
    {1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

// Fase 4 - Labirinto complexo
static const uint8_t maze_level_4[MAZE_HEIGHT][MAZE_WIDTH] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 2, 1},
    {1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1},
    {1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1},
    {1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

// Fase 5 - Desafio final
static const uint8_t maze_level_5[MAZE_HEIGHT][MAZE_WIDTH] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 2, 1},
    {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1},
    {1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 1},
    {1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1},
    {1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

// Array de ponteiros para os níveis
static const uint8_t *maze_levels[MAX_LEVELS] = {
    (const uint8_t *)maze_level_1,
    (const uint8_t *)maze_level_2,
    (const uint8_t *)maze_level_3,
    (const uint8_t *)maze_level_4,
    (const uint8_t *)maze_level_5};

// Posições iniciais do jogador para cada fase
static const player_pos_t start_positions[MAX_LEVELS] = {
    {1, 1}, {1, 1}, {1, 1}, {1, 1}, {1, 1}};

// Posições da bolinha (objetivo) para cada fase
static const goal_pos_t goal_positions[MAX_LEVELS] = {
    {14, 1}, {14, 1}, {14, 1}, {14, 1}, {14, 1}};

// Função para obter o mapa atual
const uint8_t *get_current_maze(void)
{
  return maze_levels[current_level];
}

// Função para obter valor da célula do labirinto atual
uint8_t get_maze_cell(int x, int y)
{
  const uint8_t *current_maze = get_current_maze();
  return current_maze[y * MAZE_WIDTH + x];
}

void draw_maze(void)
{
  ssd1306_clear_buffer();

  // Desenhar título da fase
  char level_text[20];
  snprintf(level_text, sizeof(level_text), "Level %d/%d", current_level + 1, MAX_LEVELS);
  ssd1306_draw_string(0, 0, level_text);

  // Desenhar labirinto (deslocado para baixo para dar espaço ao título)
  for (int y = 0; y < MAZE_HEIGHT; y++)
  {
    for (int x = 0; x < MAZE_WIDTH; x++)
    {
      uint8_t cell = get_maze_cell(x, y);
      int draw_y = (y * TILT_MAZE_TILE_SIZE) + 8; // +8 para dar espaço ao título

      if (cell == 1) // Parede
      {
        ssd1306_draw_rect(x * TILT_MAZE_TILE_SIZE, draw_y, TILT_MAZE_TILE_SIZE, TILT_MAZE_TILE_SIZE, true);
      }
      else if (cell == 2) // Objetivo (bolinha)
      {
        // Desenhar bolinha piscando
        static int blink_counter = 0;
        blink_counter++;
        if ((blink_counter / 2) % 2 == 0) // Pisca a cada 10 frames
        {
          ssd1306_draw_circle(x * TILT_MAZE_TILE_SIZE + TILT_MAZE_TILE_SIZE / 2,
                              draw_y + TILT_MAZE_TILE_SIZE / 2,
                              3, true);
        }
      }
    }
  }

  // Desenhar jogador
  int player_draw_y = (player.y * TILT_MAZE_TILE_SIZE) + 8;
  ssd1306_draw_circle(player.x * TILT_MAZE_TILE_SIZE + TILT_MAZE_TILE_SIZE / 2,
                      player_draw_y + TILT_MAZE_TILE_SIZE / 2,
                      2, false); // Círculo vazio para distinguir da bolinha

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
    return 0;

  int dx = 0, dy = 0;
  float threshold = 0.15f;

  if (ay > threshold)
    dx = 1;
  else if (ay < -threshold)
    dx = -1;

  if (ax > threshold)
    dy = 1;
  else if (ax < -threshold)
    dy = -1;

  if (dx != 0 || dy != 0)
  {
    int new_x = player.x + dx;
    int new_y = player.y + dy;

    if (new_x >= 0 && new_x < MAZE_WIDTH &&
        new_y >= 0 && new_y < MAZE_HEIGHT)
    {
      uint8_t cell = get_maze_cell(new_x, new_y);

      if (cell == 0 || cell == 2) // Caminho livre ou objetivo
      {
        player.x = new_x;
        player.y = new_y;

        if (cell == 2) // Alcançou o objetivo
        {
          catch_sound();
          return 1;
        }

        last_move = now;
        printf("Movimento: dx=%d, dy=%d, pos=(%d,%d)\n", dx, dy, player.x, player.y);
      }
    }
  }

  return 0;
}

void next_level(void)
{
  current_level++;
  ESP_LOGI("TILT_MAZE", "Nível atual: %d", current_level);
  if (current_level >= MAX_LEVELS)
  {
    // Jogo completado
    ssd1306_clear_buffer();
    ssd1306_draw_string(0, 20, "PARABENS!");
    ssd1306_draw_string(0, 35, "Voce completou");
    ssd1306_draw_string(0, 50, "todas as fases!");
    ssd1306_update_display();
    return;
  }

  // Configurar nova fase
  player.x = start_positions[current_level].x;
  player.y = start_positions[current_level].y;
  goal.x = goal_positions[current_level].x;
  goal.y = goal_positions[current_level].y;

  // Mostrar mensagem da nova fase
  ssd1306_clear_buffer();
  char level_msg[30];
  snprintf(level_msg, sizeof(level_msg), "FASE %d", current_level + 1);
  ssd1306_draw_string(0, 20, level_msg);
  ssd1306_draw_string(0, 35, "Chegue ao alvo!");
  ssd1306_update_display();

  vTaskDelay(pdMS_TO_TICKS(2000)); // Mostra mensagem por 2 segundos
  draw_maze();
}

void level_complete(void)
{
  // Efeito de vitória da fase
  ssd1306_clear_buffer();
  ssd1306_draw_string(0, 20, "FASE COMPLETA!");
  ssd1306_draw_string(0, 35, "Proximo nivel...");
  ssd1306_update_display();

  // Buzzer de vitória (se disponível)
  // buzzer_beep();

  vTaskDelay(pdMS_TO_TICKS(2000));
  next_level();
}

void start_tilt_maze_game(void)
{
  ESP_LOGI("TILT_MAZE", "Iniciando jogo multi-niveis");

  // Resetar jogo
  current_level = 0;
  player.x = start_positions[current_level].x;
  player.y = start_positions[current_level].y;
  goal.x = goal_positions[current_level].x;
  goal.y = goal_positions[current_level].y;

  xTaskCreate(
      tilt_maze_task,
      "tilt_maze_task",
      4096,
      NULL,
      1,
      &tilt_maze_task_handle);
}

void tilt_maze_task(void *pvParameters)
{
  ESP_LOGI("TILT_MAZE", "Task do jogo iniciada");

  draw_maze();
  uint32_t notif = 1;

  while (1)
  {
    if (xTaskNotifyWait(0, NOTIF_STOP, &notif, 0) == pdTRUE)
    {
      break;
    }

    if (update_player_from_tilt())
    {
      ESP_LOGI("TILT_MAZE", "Fase: %d", current_level);
      if (current_level >= MAX_LEVELS - 1)
      {
        // Último nível completado
        ssd1306_clear_buffer();
        ssd1306_draw_string(0, 15, "JOGO COMPLETO!");
        ssd1306_draw_string(0, 35, "Voce e um mestre");
        ssd1306_draw_string(0, 50, "do labirinto!");
        ssd1306_update_display();

        // buzzer_beep();
        vTaskDelete(tilt_maze_task_handle);

        break;
      }
      else
      {
        level_complete();
      }
    }

    // Redesenhar a cada frame para manter a bolinha piscando
    draw_maze();
    vTaskDelay(pdMS_TO_TICKS(100));
  }

  vTaskDelete(tilt_maze_task_handle);
}