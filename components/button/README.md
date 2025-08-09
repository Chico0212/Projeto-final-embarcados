# 🧠 `button.h` – Biblioteca de Gerenciamento de Botões com Debounce e Eventos

Esta biblioteca facilita o uso de botões físicos no ESP32 (ou outros microcontroladores compatíveis com ESP-IDF), fornecendo uma camada de abstração com suporte a:

- **Debounce automático**
- **Cliques simples**
- **Cliques longos**
- **Cliques duplos**
- **Leitura com ou sem ISR**
- **Callback customizado por evento**

Ideal para projetos com interface física onde o comportamento dos botões precisa ser tratado com precisão e robustez.

---

## 📦 Funcionalidades

- Configuração de até **8 botões simultâneos** (`MAX_BUTTONS`)
- Suporte a botões **ativos em nível baixo ou alto**
- Controle de tempo de **debounce** e **clique longo**
- **Callbacks** personalizáveis por botão
- Leitura **bloqueante ou não bloqueante** de eventos

---

## 📄 Estruturas Principais

### `button_event_t`
Enumeração dos tipos de eventos reconhecidos:

```c
typedef enum {
    BUTTON_EVENT_PRESSED,
    BUTTON_EVENT_RELEASED,
    BUTTON_EVENT_LONG_PRESS,
    BUTTON_EVENT_DOUBLE_CLICK
} button_event_t;
```

### `button_event_data_t`
Contém informações sobre um evento detectado:

```c
typedef struct {
    gpio_num_t gpio_num;
    button_event_t event;
    uint32_t timestamp; // em milissegundos
} button_event_data_t;
```

### `button_config_t`
Estrutura de configuração avançada para cada botão:

```c
typedef struct {
    gpio_num_t gpio_num;
    gpio_pull_mode_t pull_mode;
    bool active_low;
    uint32_t debounce_time_ms;
    uint32_t long_press_time_ms;
    button_isr_callback_t callback;
} button_config_t;
```

---

## 🚀 Como Usar

### 1. Inicialização rápida com ISR:

```c
gpio_config_t gpio_cfg = {
    .pin_bit_mask = (1ULL << GPIO_NUM_5),
    .mode = GPIO_MODE_INPUT,
    .pull_up_en = GPIO_PULLUP_ENABLE,
};

init_buttons_isr(&gpio_cfg, my_button_callback);
```

### 2. Configuração avançada:

```c
button_config_t cfg = {
    .gpio_num = GPIO_NUM_5,
    .pull_mode = GPIO_PULLUP_ONLY,
    .active_low = true,
    .debounce_time_ms = 50,
    .long_press_time_ms = 1000,
    .callback = my_button_callback
};

button_config_advanced(&cfg);
```

### 3. Leitura de eventos (modo polling):

```c
button_event_data_t event;
if (button_get_event(&event) == ESP_OK) {
    printf("Botão %d evento: %d\n", event.gpio_num, event.event);
}
```

---

## 🛠️ Principais Funções

| Função                                 | Descrição |
|----------------------------------------|-----------|
| `init_buttons()`                       | Inicializa botões com configuração básica |
| `init_buttons_isr()`                   | Inicializa botões com suporte a interrupções e callback |
| `button_config_advanced()`             | Inicialização detalhada de cada botão |
| `button_read()`                        | Lê estado atual (sem debounce) |
| `button_read_debounced()`              | Lê estado com debounce aplicado |
| `button_get_event()`                   | Obtém evento mais recente (não bloqueante) |
| `button_wait_event()`                  | Espera por evento com timeout (bloqueante) |
| `button_enable()`                      | Ativa/desativa detecção de eventos para um botão específico |
| `button_clear_events()`                | Limpa a fila de eventos pendentes |
| `buttons_deinit()`                     | Libera todos os recursos |
| `button_init_pullup()` / `pulldown()`  | Inicialização rápida com pull-up/down |

---

## ⚠️ Notas

- Todos os tempos (debounce, long press) são definidos em **milissegundos**.
- Eventos são enfileirados internamente e devem ser tratados com `button_get_event()` ou via callback.
- Requer `freertos`, `esp_timer`, `driver/gpio` e `esp_log`.

---

## 👨‍💻 Exemplo de Callback

```c
void my_button_callback(gpio_num_t gpio, button_event_t event) {
    if (event == BUTTON_EVENT_LONG_PRESS) {
        printf("Long press no botão %d!\n", gpio);
    }
}
```

---

## 📚 Requisitos

- ESP-IDF (qualquer versão recente)
- FreeRTOS
- GPIO, ISR, timer e fila configurados no `sdkconfig`
