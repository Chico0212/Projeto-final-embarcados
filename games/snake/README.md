# 🐍 Snake com Controle por Inclinação – ESP32

Este módulo implementa o clássico jogo da cobrinha (**Snake**) com visualização em display OLED e controle via sensor de inclinação **MPU6050**. Inclui lógica de colisão, crescimento da cobra, geração de frutas, som com buzzer e registro de pontuação com cartão SD.

---

## 🧩 Elementos do Jogo

| Elemento       | Descrição                            |
|----------------|----------------------------------------|
| Tela           | 128x64 pixels (SSD1306)                |
| Tile (bloco)   | 4x4 pixels                             |
| Símbolo cobra  | `#`                                    |
| Símbolo fruta  | `*`                                    |
| Tamanho inicial| 4 blocos                               |
| Velocidade     | 150ms por frame (`GAME_SPEED_MS`)      |

---

## 🧠 Estruturas de Dados

```c
typedef struct {
    int x, y;
} Point;
```

A cobra é representada como um vetor de `Point`, com movimentação contínua e crescimento ao comer frutas.

---

## 🚀 Funções Disponíveis

| Função                    | Descrição |
|---------------------------|-----------|
| `start_snake_game()`      | Inicia o jogo da cobrinha (tarefa FreeRTOS) |
| `reset_snake_game()`      | Reinicia o jogo para o estado inicial |
| `place_fruit()`           | Posiciona uma nova fruta aleatoriamente |
| `draw_tile(x, y, symbol)` | Desenha uma célula com símbolo na tela |
| `collision(Point head)`   | Verifica se houve colisão com a parede ou a própria cobra |

---

## 🎮 Mecânica do Jogo

- A movimentação é controlada por **inclinação lateral** do dispositivo via MPU6050
- A cada fruta comida:
  - A cobra cresce
  - Um novo ponto é somado
  - Um som é reproduzido via buzzer
- Se colidir com parede ou consigo mesma:
  - O jogo é encerrado
  - A pontuação pode ser salva no cartão SD

---

## 📏 Tamanho Máximo da Cobra

```c
#define MAX_SNAKE_LEN 64
```

---

## 🛠 Dependências

- `ssd1306.h` – exibição gráfica
- `mpu6050.h` – controle por inclinação
- `buzzer.h` – efeitos sonoros
- `sd_card.h` – registro de pontuação
- `freertos/task.h` – execução como tarefa

---

## ⏱️ Velocidade do Jogo

```c
#define GAME_SPEED_MS 150
```

Você pode ajustar o intervalo de atualização para acelerar ou desacelerar a jogabilidade.

---

## 📦 Exemplo de Execução

```c
void app_main() {
    start_snake_game();
}
```

---

## 📎 Licença

MIT ou compatível com seu projeto.
