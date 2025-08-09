#ifndef I2C_COMMONS_H
#define I2C_COMMONS_H

#include "driver/i2c.h"
#include "esp_log.h"

// Configurações I2C
#define I2C_MASTER_SCL_IO           22    // GPIO para SCL
#define I2C_MASTER_SDA_IO           21    // GPIO para SDA
#define I2C_MASTER_NUM              0     // Número da porta I2C
#define I2C_MASTER_FREQ_HZ          100000 // Frequência I2C
#define I2C_MASTER_TX_BUF_DISABLE   0
#define I2C_MASTER_RX_BUF_DISABLE   0
#define I2C_MASTER_TIMEOUT_MS       1000

esp_err_t i2c_init(void);

void i2c_scan(void);

#endif
