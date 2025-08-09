# 🧱 Dodge the Blocks com Controle por Inclinação

Este módulo implementa um jogo onde o jogador deve **desviar de blocos** que caem do topo da tela. O movimento do jogador é controlado por **inclinação com MPU6050**, com feedback visual no display OLED e som via buzzer. Pontuações podem ser armazenadas via cartão SD.

---

## 🎮 Objetivo

Movimentar o jogador lateralmente para **evitar colisões** com blocos que descem cada vez mais rápido. O jogo termina quando há uma colisão com qualquer bloco ativo.

---

## 📐 Layout do Jogo

| Elemento       | Tamanho (pixels) |
|----------------|------------------|
| Jogador        | 8×8              |
| Bloco inimigo  | 6×6              |
| Tela           | 128×64           |

O jogador fica fixo próximo à base da tela:

```c
#define PLAYER_Y (DISPLAY_HEIGHT - PLAYER_HEIGHT - 2)
```

---

## 🧠 Estruturas

```c
typedef struct {
    int x, y;
    float speed;
    bool active;
} Block;
```

Cada bloco possui posição, velocidade individual e estado (ativo ou não).

---

## 🚀 Funções Disponíveis

| Função                      | Descrição |
|-----------------------------|-----------|
| `start_dodge_blocks_game()` | Inicia o jogo como uma tarefa FreeRTOS |
| `draw_player()`             | Desenha o jogador na tela OLED |
| `draw_block(Block* b)`      | Desenha um bloco inimigo |
| `reset_game()`              | Reinicia o estado do jogo |
| `check_collision(Block* b)` | Verifica colisão entre o jogador e o bloco |

---

## 🕹️ Mecânica

- O jogador movimenta-se no eixo **X** com base na inclinação lateral.
- Os blocos caem com velocidade crescente.
- Ao colidir com qualquer bloco:
  - O jogo termina
  - O som de falha é tocado
  - A pontuação pode ser registrada via cartão SD

---

## ⚙️ Parâmetros do Jogo

```c
#define MAX_BLOCKS         5
#define BLOCK_SPEED_INITIAL 1.0f
#define GAME_DELAY_MS      50
```

---

## 🔗 Dependências

- `ssd1306.h` – Display OLED
- `mpu6050.h` – Sensor de inclinação
- `buzzer.h` – Efeitos sonoros
- `sd_card.h` – Armazenamento de pontuação
- FreeRTOS, GPIO, Math, ESP-IDF

---

## 📦 Exemplo de Execução

```c
void app_main() {
    start_dodge_blocks_game();
}
```

---

## 📎 Licença

MIT ou compatível com seu projeto.
