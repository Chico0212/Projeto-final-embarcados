# 🎵 Biblioteca PWM para Buzzer – ESP32

Esta biblioteca fornece um conjunto de funções utilitárias para controlar um buzzer usando PWM no ESP32 via o driver `ledc`. Inclui notas musicais pré-definidas, durações e melodias para eventos como vitória, derrota e efeitos especiais — ideal para jogos embarcados e sistemas interativos.

---

## 📌 Funcionalidades

- Controle PWM de buzzer piezoelétrico (via `ledc`)
- Notas musicais definidas (C4 a G5)
- Durações musicais (semínima, colcheia, semicolcheia)
- API simples para:
  - Tocar uma nota
  - Tocar melodias (vitória/derrota)
  - Acionar efeitos sonoros (ex: pegar item)
- Tema do **Mario Bros** incluído 🎮

---

## 🧩 Constantes

### 🎼 Notas

```c
#define NOTE_C4  262
#define NOTE_D4  294
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_G4  392
#define NOTE_A4  440
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_D5  587
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_G5  784
#define NOTE_A5  880
```

### ⏱️ Durações

```c
#define QUARTER_NOTE     300  // Semínima
#define EIGHTH_NOTE      150  // Colcheia
#define SIXTEENTH_NOTE    75  // Semicolcheia
```

---

## 🔧 Configuração

### Pino do Buzzer

```c
#define BUZZER_GPIO GPIO_NUM_18
```

Certifique-se de conectar fisicamente o buzzer a esse pino.

---

## 🚀 Como Usar

### 1. Inicializar o PWM

```c
buzzer_pwm_init();
```

### 2. Tocar notas individuais

```c
buzzer_play_note(NOTE_C4, QUARTER_NOTE);
```

### 3. Tocar melodias prontas

```c
buzzer_victory_melody();  // Jingle de vitória
buzzer_defeat_melody();   // Tom de derrota
buzzer_mario_theme();     // Tema do Mario Bros 🎮
```

### 4. Acionar efeitos rápidos

```c
game_win();
game_lose();
catch_sound();
```

### 5. Controle manual

```c
buzzer_on();
buzzer_off();
```

---

## 📚 Referência da API

| Função                         | Descrição                                      |
|--------------------------------|------------------------------------------------|
| `buzzer_pwm_init()`            | Inicializa o canal/pino PWM                   |
| `buzzer_play_note(freq, dur)`  | Toca uma nota na frequência e duração desejada |
| `buzzer_victory_melody()`      | Toca jingle de vitória                        |
| `buzzer_defeat_melody()`       | Toca som de derrota                           |
| `buzzer_mario_theme()`         | Toca o tema do Mario Bros                     |
| `buzzer_on()`                  | Liga o buzzer (PWM ativo)                     |
| `buzzer_off()`                 | Desliga o buzzer                              |
| `game_win()`                   | Alias para som de vitória                     |
| `game_lose()`                  | Alias para som de derrota                     |
| `catch_sound()`                | Som curto (ex: item capturado)                |

---

## 🛠 Dependências

- [ESP-IDF](https://docs.espressif.com/projects/esp-idf/pt/latest/)
- FreeRTOS
- `driver/ledc.h`

---

## 🧪 Exemplo

```c
void app_main() {
    buzzer_pwm_init();
    game_win();
    vTaskDelay(pdMS_TO_TICKS(500));
    buzzer_play_note(NOTE_G4, QUARTER_NOTE);
}
```

---

## 📎 Licença

MIT ou compatível com o projeto principal.
