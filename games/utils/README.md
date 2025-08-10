# 🎵 `utils.h` – Utilitários de Áudio, Display e Gerenciamento de Pontuação

Este módulo reúne funções para exibir informações no display OLED (SSD1306), tocar efeitos sonoros e músicas no buzzer, além de ler e atualizar a pontuação do jogo em um arquivo.
Foi pensado para jogos no **ESP32** que utilizem **display gráfico** e **buzzer PWM**.

O objetivo é centralizar a lógica de feedback visual e auditivo, mantendo o código do jogo mais limpo e reutilizável.

---

## 📦 Funcionalidades

* Exibir tela de **Game Over** com pontuação atual e recorde.
* Tocar efeitos sonoros para **vitória**, **derrota**, **acerto** e **impacto**.
* Reproduzir a **melodia inicial do Mario Bros** (trecho).
* Ler e atualizar o **recorde de pontuação** em arquivo persistente.

---

## 📄 Principais Funções

### `show_game_over(const char *file_path, int score)`

Exibe a tela de "Game Over" no SSD1306, mostrando:

* Pontuação atual
* Melhor pontuação registrada no arquivo

---

### `game_win()`

Toca uma **melodia de vitória** com sequência ascendente alegre.
Ideal para quando o jogador completa uma fase ou conquista algo importante.

---

### `game_lose()`

Toca uma **melodia de derrota** com notas descendentes e ritmo lento.
Usada quando o jogador perde o jogo.

---

### `catch_sound()`

Efeito sonoro curto e agudo para indicar que o jogador **capturou** ou **acertou** algo.

---

### `buzzer_mario_theme()`

Reproduz a introdução da música tema do Mario Bros.
Serve como efeito nostálgico ou de abertura do jogo.

---

### `hit_ball()`

Som breve para indicar o impacto de uma bola ou objeto.

---

### `update_score(const char *file_path, int new_score)`

Compara a nova pontuação com a registrada no arquivo.

* Se a nova for maior, atualiza o arquivo e retorna `1`.
* Caso contrário, mantém o valor e retorna `0`.

---

### `read_score_from_file(const char *file_path)`

Lê e retorna a pontuação salva no arquivo.

---

## 🚀 Exemplo de Uso

```c
#include "utils.h"

void end_game(int score) {
    if (update_score("/spiffs/score.txt", score)) {
        printf("Novo recorde!\n");
    }
    show_game_over("/spiffs/score.txt", score);
    game_lose();
}
```

---

## ⚠️ Notas

* Todas as funções de som dependem de `buzzer_play_note()` e das definições de notas (ex.: `NOTE_C4`, `EIGHTH_NOTE`) no módulo de buzzer.
* As funções que exibem informações dependem do driver `ssd1306`.
* A manipulação de arquivos depende de `read_file()` e `write_file()`, que devem estar implementados no projeto.
* É necessário que o sistema de arquivos (`SPIFFS` ou `FATFS`) esteja montado antes de acessar pontuações.

---

## 📚 Requisitos

* Driver **SSD1306** configurado e inicializado
* Módulo de **buzzer** com suporte a frequências e duração de notas
* Funções `read_file()` e `write_file()` implementadas
* ESP-IDF com FreeRTOS habilitado
