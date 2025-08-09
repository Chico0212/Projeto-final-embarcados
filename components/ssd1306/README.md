# 🖥️ Biblioteca SSD1306 para ESP32 (Display OLED via I2C)

Esta biblioteca permite controlar um display OLED baseado no controlador **SSD1306** usando a interface **I2C** no ESP32, com suporte a renderização de texto, linhas, retângulos, círculos e pixels individuais.

---

## 📌 Funcionalidades

- Comunicação via I2C (endereços 0x3C ou 0x3D)
- Buffer de exibição com atualização assíncrona
- Renderização de:
  - Texto (`char` e `string`)
  - Linhas e retângulos (cheios ou vazados)
  - Círculos
  - Pixels individuais
- Comandos SSD1306 diretos (liga/desliga display, contraste, etc.)
- Função de teste de padrão gráfico

---

## 🔧 Conexões I2C

| Sinal | GPIO Padrão |
|-------|-------------|
| SDA   | GPIO 21     |
| SCL   | GPIO 22     |

---

## 🧩 Parâmetros do Display

| Definição               | Valor       |
|------------------------|-------------|
| Largura (`WIDTH`)      | 128 pixels  |
| Altura (`HEIGHT`)      | 64 pixels   |
| Páginas (`PAGES`)      | 8 (8 linhas de 8 pixels) |
| Endereço I2C           | `0x3C` (ou `0x3D` em algumas placas) |

---

## 🧪 API - Funções Disponíveis

### 🧠 Inicialização

| Função               | Descrição |
|----------------------|-----------|
| `i2c_init()`         | Inicializa o barramento I2C |
| `ssd1306_init()`     | Inicializa o display SSD1306 |
| `ssd1306_clear_buffer()` | Limpa o buffer local da tela |
| `ssd1306_update_display()` | Envia o buffer para o display |

---

### ✍️ Desenho de elementos

| Função                             | Descrição |
|------------------------------------|-----------|
| `ssd1306_set_pixel(x, y, on)`      | Liga/desliga um pixel |
| `ssd1306_draw_line(x0, y0, x1, y1)`| Desenha uma linha reta |
| `ssd1306_draw_rect(x, y, w, h, filled)` | Retângulo (preenchido ou não) |
| `ssd1306_draw_circle(cx, cy, r, filled)` | Círculo com centro e raio |
| `ssd1306_draw_char(x, y, c)`       | Desenha caractere usando fonte incluída |
| `ssd1306_draw_string(x, y, str)`   | Escreve string a partir da posição dada |

---

### ⚙️ Comandos diretos / testes

| Função                      | Descrição |
|-----------------------------|-----------|
| `ssd1306_write_command(cmd)`| Envia comando direto ao SSD1306 |
| `ssd1306_write_data(data, len)` | Envia dados brutos à RAM do display |
| `ssd1306_test_pattern()`    | Mostra padrão de teste |
| `i2c_scan()`                | Escaneia dispositivos I2C conectados |

---

## 🚀 Exemplo de Uso

```c
void app_main() {
    i2c_init();
    ssd1306_init();

    ssd1306_clear_buffer();
    ssd1306_draw_string(0, 0, "Olá, mundo!");
    ssd1306_draw_rect(0, 16, 60, 20, false);
    ssd1306_draw_circle(96, 32, 10, true);
    ssd1306_update_display();
}
```

---

## 🛠 Dependências

- ESP-IDF
- `freertos/FreeRTOS.h`
- `driver/i2c.h`
- `esp_log.h`
- `font.h` (fonte incluída no projeto)

---

## 🔍 Observações

- Todas as funções de desenho afetam apenas o **buffer local**, sendo necessário chamar `ssd1306_update_display()` para refletir as alterações na tela.
- Os limites máximos de `x` e `y` são: `x < 128`, `y < 64`.

---

## 📎 Licença

MIT ou compatível com seu projeto.

