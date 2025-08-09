# 🕹️ Paddle Pong – Jogo de Pong com Controle por Inclinação (MPU6050)

Esta biblioteca implementa um minigame estilo **Pong** controlado por inclinação utilizando o sensor **MPU6050**, com saída gráfica em um display OLED (SSD1306) e feedback sonoro via buzzer. É uma ótima demonstração de interação física com sistemas embarcados.

---

## 🎯 Objetivo

Controlar uma raquete (paddle) na parte inferior da tela para rebater a bolinha, mantendo-a em jogo o maior tempo possível. O controle é feito pela inclinação do dispositivo (acelerômetro MPU6050).

---

## 🎮 Recursos do Jogo

- Raquete controlada por inclinação (eixo X do acelerômetro)
- Bolinha com movimento contínuo (posição e velocidade em float)
- Colisões com parede e paddle
- Pontuação e contagem de vidas
- Sons via buzzer (acertos, falhas)
- Sistema de pausa e reinício
- Registro de pontuação via cartão SD

---

## 📐 Definições Visuais

| Elemento  | Dimensões                |
|-----------|--------------------------|
| Raquete   | 20x3 pixels              |
| Bolinha   | 2x2 pixels               |
| Tela      | 128x64 pixels (SSD1306) |

---

## ⏱️ Velocidade de Jogo

```c
#define GAME_SPEED 50  // delay entre frames em milissegundos
```

---

## 🧩 Estrutura do Estado

```c
typedef struct {
    float x, y;
    float vx, vy;
    int size;
} ball_t;

typedef struct {
    int x, y;
    int width, height;
} paddle_t;

typedef struct {
    ball_t ball;
    paddle_t paddle;
    int score;
    int lives;
    bool game_over;
    bool paused;
} game_state_t;
```

---

## 🚀 Como Jogar

### 1. Inicialize o jogo

```c
start_paddle_pong_game();
```

Internamente isso cria uma task `game_task()` no FreeRTOS.

---

### 2. Funções principais

| Função                 | Descrição |
|------------------------|-----------|
| `game_init()`          | Inicializa estruturas e variáveis do jogo |
| `reset_ball()`         | Reseta a bolinha após perda ou início |
| `update_paddle()`      | Atualiza posição da raquete com base no MPU6050 |
| `update_ball()`        | Move a bolinha e detecta colisões |
| `check_paddle_collision()` | Verifica colisão entre bolinha e raquete |
| `check_restart()`      | Detecta se o botão de reinício foi pressionado |
| `draw_game()`          | Renderiza todo o estado do jogo no display |
| `game_task()`          | Loop principal do jogo (FreeRTOS task) |

---

## 🔗 Dependências

- `mpu6050.h` – leitura de inclinação
- `ssd1306.h` – display OLED
- `buzzer.h` – efeitos sonoros
- `sd_card.h` – armazenamento de pontuação
- FreeRTOS, Math, ESP-IDF

---

## 📦 Exemplo de Execução

```c
void app_main() {
    start_paddle_pong_game();
}
```

---

## 📎 Licença

MIT ou compatível com seu projeto.
