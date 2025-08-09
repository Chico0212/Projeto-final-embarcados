# 🧭 Tilt Maze – Labirinto com Controle por Inclinação (MPU6050)

Este módulo implementa um jogo de labirinto interativo no ESP32, no qual o jogador move um ponto através de um labirinto utilizando a **inclinação do dispositivo** (sensor MPU6050). A saída gráfica é feita em um display OLED (SSD1306), e sons de feedback são emitidos com buzzer.

---

## 🧩 Estrutura do Labirinto

- **Tamanho do labirinto:** 16 colunas × 8 linhas
- **Tamanho de cada célula:** 8×8 pixels (`TILE_SIZE`)
- **Player:** representado como um quadrado em uma célula
- O movimento é baseado na **inclinação** em X e Y

```c
#define MAZE_WIDTH  16
#define MAZE_HEIGHT 8
#define TILE_SIZE   8
```

---

## 👤 Representação do Jogador

```c
typedef struct {
    int x, y; // posição atual em células do labirinto
} player_pos_t;
```

---

## 🧠 Funções Disponíveis

| Função                        | Descrição |
|-------------------------------|-----------|
| `start_tilt_maze_game()`      | Inicia o jogo como uma task do FreeRTOS |
| `draw_maze()`                 | Renderiza o labirinto e posição inicial do jogador |
| `update_player_from_tilt()`  | Atualiza a posição do jogador com base na inclinação (retorna status de movimento) |

---

## 🎮 Mecânica do Jogo

- O jogador **inclina fisicamente** o dispositivo para mover o personagem no labirinto.
- Cada frame:
  - A posição é atualizada com base nos dados do MPU6050
  - O novo estado é desenhado na tela OLED
  - Sons podem ser tocados (movimento, colisão, vitória)

---

## 🔗 Dependências

- `mpu6050.h` – Sensor de inclinação
- `ssd1306.h` – Display OLED
- `buzzer.h` – Efeitos sonoros
- `freertos/task.h` – Execução em tarefa

---

## 📦 Exemplo de Execução

```c
void app_main() {
    start_tilt_maze_game();
}
```

---

## 🛠 Personalização

- É possível modificar a função `draw_maze()` para definir diferentes layouts de labirinto.
- A detecção de obstáculos e colisões pode ser incluída via matriz `char maze[MAZE_HEIGHT][MAZE_WIDTH]`.

---

## 📎 Licença

MIT ou compatível com o seu projeto.
