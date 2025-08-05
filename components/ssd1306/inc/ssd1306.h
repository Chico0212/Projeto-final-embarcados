#ifndef SSD1306_H
#define SSD1306_H

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "font.h"

// Configurações I2C
#define I2C_MASTER_SCL_IO           22    // GPIO para SCL
#define I2C_MASTER_SDA_IO           21    // GPIO para SDA
#define I2C_MASTER_NUM              0     // Número da porta I2C
#define I2C_MASTER_FREQ_HZ          400000 // Frequência I2C
#define I2C_MASTER_TX_BUF_DISABLE   0
#define I2C_MASTER_RX_BUF_DISABLE   0
#define I2C_MASTER_TIMEOUT_MS       1000

// Configurações do SSD1306
#define SSD1306_I2C_ADDR            0x3C  // Endereço I2C (pode ser 0x3D)
#define SSD1306_WIDTH               128
#define SSD1306_HEIGHT              64
#define SSD1306_PAGES               8     // 64/8 = 8 páginas

// Comandos do SSD1306
#define SSD1306_CMD_DISPLAY_OFF     0xAE
#define SSD1306_CMD_DISPLAY_ON      0xAF
#define SSD1306_CMD_SET_CONTRAST    0x81
#define SSD1306_CMD_ENTIRE_DISPLAY_ON 0xA5
#define SSD1306_CMD_NORMAL_DISPLAY  0xA6
#define SSD1306_CMD_INVERT_DISPLAY  0xA7
#define SSD1306_CMD_SET_MULTIPLEX   0xA8
#define SSD1306_CMD_SET_DISPLAY_OFFSET 0xD3
#define SSD1306_CMD_SET_START_LINE  0x40
#define SSD1306_CMD_SEGMENT_REMAP   0xA1
#define SSD1306_CMD_COM_SCAN_DEC    0xC8
#define SSD1306_CMD_SET_COM_PINS    0xDA
#define SSD1306_CMD_SET_CLOCK_DIV   0xD5
#define SSD1306_CMD_SET_PRECHARGE   0xD9
#define SSD1306_CMD_SET_VCOM_DETECT 0xDB
#define SSD1306_CMD_CHARGE_PUMP     0x8D
#define SSD1306_CMD_MEMORY_MODE     0x20
#define SSD1306_CMD_SET_COLUMN_ADDR 0x21
#define SSD1306_CMD_SET_PAGE_ADDR   0x22

// Buffer do display
static uint8_t ssd1306_buffer[SSD1306_WIDTH * SSD1306_PAGES];

// Função para enviar comando I2C
esp_err_t ssd1306_write_command(uint8_t cmd);

// Função para enviar dados I2C
esp_err_t ssd1306_write_data(uint8_t* data, size_t len);

// Inicializar I2C
void i2c_init(void);

// Inicializar display SSD1306
void ssd1306_init(void);

// Limpar buffer
void ssd1306_clear_buffer(void);

// Atualizar display
void ssd1306_update_display(void);

// Definir pixel
void ssd1306_set_pixel(int x, int y, bool on);

// Função auxiliar para desenhar os 8 pontos simétricos do círculo
static void ssd1306_draw_circle_points(int cx, int cy, int x, int y);

// Desenhar círculo usando algoritmo de Bresenham
void ssd1306_draw_circle(int cx, int cy, int radius, bool filled);
// Desenhar caractere
void ssd1306_draw_char(int x, int y, char c);

// Desenhar string
void ssd1306_draw_string(int x, int y, const char* str);

// Desenhar linha
void ssd1306_draw_line(int x0, int y0, int x1, int y1);

// Desenhar retângulo
void ssd1306_draw_rect(int x, int y, int w, int h, bool filled);

#endif
