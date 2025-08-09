#ifndef I2C_COMMONS_H
#define I2C_COMMONS_H

#include "driver/i2c.h"
#include "esp_log.h"
#include "esp_event.h"

extern StaticSemaphore_t i2c_mutex_buffer;

extern SemaphoreHandle_t i2c_mutex;

// Configurações I2C
#define I2C_MASTER_SCL_IO           22    // GPIO para SCL
#define I2C_MASTER_SDA_IO           21    // GPIO para SDA
#define I2C_MASTER_NUM              0     // Número da porta I2C
#define I2C_MASTER_FREQ_HZ          100000 // Frequência I2C
#define I2C_MASTER_TX_BUF_DISABLE   0
#define I2C_MASTER_RX_BUF_DISABLE   0
#define I2C_MASTER_TIMEOUT_MS       1000

// Endereços
#define MPU6050_ADDR                0x68    // Endereço I2C padrão do MPU6050
#define SSD1306_I2C_ADDR            0x3C  // Endereço I2C (pode ser 0x3D)

esp_err_t i2c_init(void);

void i2c_scan(void);

bool i2c_check_bus_active(void);

void i2c_recover_bus(void);

void check_and_recover_i2c_if_needed(void);

esp_err_t check_i2c(void);
#endif
