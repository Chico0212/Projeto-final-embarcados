# 🧠 `button.h` – Biblioteca de Gerenciamento de Botões com Debounce, ISR e Eventos

Esta biblioteca fornece uma API robusta para gerenciar botões no **ESP32** (ou outros microcontroladores compatíveis com ESP-IDF), permitindo leitura confiável via **debounce**, detecção de eventos e integração com **interrupções (ISR)**.

O sistema oferece suporte para:

* **Debounce automático** via temporização por microssegundos
* **Leitura direta** (com e sem debounce)
* **Eventos por interrupção**
* **Espera e leitura de eventos via fila**
* **Callback configurável**
* **Configuração rápida com pull-up/pull-down**

Ideal para aplicações onde a resposta do botão deve ser confiável, eliminando múltiplos acionamentos falsos e permitindo uma arquitetura de software bem organizada.

---

## 📦 Funcionalidades

* Suporte para **até 8 botões simultâneos** (`MAX_BUTTONS`)
* Debounce configurado em microssegundos
* Configuração rápida **pull-up** ou **pull-down**
* Integração com ISR via `gpio_install_isr_service`
* Leitura de eventos via **fila FreeRTOS**
* Ativar/desativar detecção de eventos individualmente

---

## 📄 Estruturas Principais

### `button_event_t`

Enumeração dos tipos de eventos:

```c
typedef enum {
    BUTTON_EVENT_PRESSED,
    BUTTON_EVENT_RELEASED,
    BUTTON_EVENT_LONG_PRESS,
    BUTTON_EVENT_DOUBLE_CLICK
} button_event_t;
```

### `button_event_data_t`

Estrutura para descrever um evento detectado:

```c
typedef struct {
    gpio_num_t gpio_num;      // Pino do botão
    button_event_t event;     // Tipo de evento
    uint32_t timestamp;       // Momento do evento (us)
} button_event_data_t;
```

### `button_config_t`

Configuração avançada de um botão:

```c
typedef struct {
    gpio_num_t gpio_num;
    gpio_pull_mode_t pull_mode;
    bool active_low;                    // true se botão é ativo em nível baixo
    uint32_t debounce_time_ms;
    uint32_t long_press_time_ms;
    button_isr_callback_t callback;     // Função de callback associada
} button_config_t;
```

---

## 🚀 Como Usar

### 1. Inicialização rápida com pull-up e ISR

```c
button_init_pullup(GPIO_NUM_25, my_button_callback);
```

### 2. Inicialização básica sem ISR

```c
gpio_config_t cfg = {
    .pin_bit_mask = (1ULL << GPIO_NUM_26),
    .mode = GPIO_MODE_INPUT,
    .pull_up_en = GPIO_PULLUP_ENABLE,
    .intr_type = GPIO_INTR_DISABLE
};
init_buttons(&cfg);
```

### 3. Configuração avançada

```c
button_config_t cfg = {
    .gpio_num = GPIO_NUM_25,
    .pull_mode = GPIO_PULLUP_ONLY,
    .active_low = true,
    .debounce_time_ms = 50,
    .long_press_time_ms = 1000,
    .callback = my_button_callback
};
button_config_advanced(&cfg);
```

### 4. Leitura com debounce

```c
if (button_read_debounced(GPIO_NUM_25) == 0) {
    printf("Botão pressionado!\n");
}
```

### 5. Leitura de evento via fila

```c
button_event_data_t event;
if (button_get_event(&event) == ESP_OK) {
    printf("Botão %d, evento %d\n", event.gpio_num, event.event);
}
```

---

## 🛠️ Principais Funções

| Função                     | Descrição                                   |
| -------------------------- | ------------------------------------------- |
| `init_buttons()`           | Inicializa botões com configuração básica   |
| `init_buttons_isr()`       | Inicializa botões com ISR e fila de eventos |
| `button_config_advanced()` | Configuração avançada individual            |
| `button_read()`            | Lê estado do botão sem debounce             |
| `button_read_debounced()`  | Lê estado aplicando debounce                |
| `button_wait_event()`      | Bloqueia até receber evento ou timeout      |
| `button_get_event()`       | Obtém evento imediatamente (não bloqueante) |
| `button_enable()`          | Ativa/desativa interrupção de um botão      |
| `button_clear_events()`    | Limpa eventos pendentes na fila             |
| `buttons_deinit()`         | Remove handlers e libera recursos           |
| `button_init_pullup()`     | Inicialização rápida com pull-up            |
| `button_init_pulldown()`   | Inicialização rápida com pull-down          |

---

## ⚠️ Notas

* `BUTTON_DEBOUNCE_TIME_US` define o tempo mínimo entre eventos no ISR.
* A fila de eventos (`xQueue`) é criada automaticamente quando necessário.
* É necessário instalar o serviço de interrupção `gpio_install_isr_service()` — feito internamente pela função.
* `button_read_debounced()` mantém o estado anterior até que haja variação estável.

---

## 👨‍💻 Exemplo de Callback

```c
void my_button_callback(button_event_data_t *event) {
    if (event->event == BUTTON_EVENT_PRESSED) {
        printf("Botão %d pressionado!\n", event->gpio_num);
    }
}
```

---

## 📚 Requisitos

* ESP-IDF 4.x ou superior
* FreeRTOS
* `driver/gpio.h`, `esp_timer.h`, `esp_log.h`

