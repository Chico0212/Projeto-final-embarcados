# 📦 Biblioteca MPU6050 para ESP32 (I2C)

Esta biblioteca facilita a comunicação com o sensor MPU6050 via barramento I2C, utilizando ESP-IDF. O MPU6050 combina um acelerômetro de 3 eixos e um giroscópio de 3 eixos, permitindo leituras de aceleração, rotação angular e temperatura.

---

## 🎯 Funcionalidades

- Inicialização I2C e do sensor MPU6050
- Leitura completa dos dados brutos (aceleração, giroscópio, temperatura)
- Conversão para valores físicos:
  - Aceleração em **g**
  - Giroscópio em **graus por segundo (°/s)**
  - Temperatura em **graus Celsius**
- Função `task` para uso direto no FreeRTOS

---

## 🔌 Conexões Padrão (I2C)

| Sinal | GPIO Padrão |
|-------|--------------|
| SDA   | GPIO 21      |
| SCL   | GPIO 22      |
| Addr  | 0x68 (fixo para GND) |

---

## 🧩 Constantes Importantes

### Endereços de registradores

```c
#define MPU6050_PWR_MGMT_1        0x6B
#define MPU6050_SMPLRT_DIV        0x19
#define MPU6050_CONFIG            0x1A
#define MPU6050_GYRO_CONFIG       0x1B
#define MPU6050_ACCEL_CONFIG      0x1C
#define MPU6050_ACCEL_XOUT_H      0x3B
#define MPU6050_GYRO_ZOUT_L       0x48
#define MPU6050_WHO_AM_I          0x75
```

---

## 🧪 Tipo de dado principal

```c
typedef struct {
    int16_t accel_x;
    int16_t accel_y;
    int16_t accel_z;
    int16_t temp;
    int16_t gyro_x;
    int16_t gyro_y;
    int16_t gyro_z;
} mpu6050_data_t;
```

---

## 🚀 Como Usar

### 1. Inicializar o barramento I2C

```c
i2c_master_init();
```

### 2. Inicializar o sensor MPU6050

```c
mpu6050_init();
```

### 3. Ler os dados brutos

```c
mpu6050_data_t raw;
mpu6050_read_all(&raw);
```

### 4. Converter para valores físicos

```c
float accel[3], gyro[3], temp;
mpu6050_convert_data(&raw, accel, gyro, &temp);
```

### 5. Ler aceleração diretamente

```c
float ax, ay, az;
mpu6050_read_acceleration(&ax, &ay, &az);
```

---

## 📚 Referência da API

| Função                            | Descrição |
|-----------------------------------|-----------|
| `i2c_master_init()`               | Inicializa o I2C com pinos e clock padrão |
| `mpu6050_init()`                  | Inicializa o sensor MPU6050 |
| `mpu6050_write_byte()`            | Escreve 1 byte em registrador do sensor |
| `mpu6050_read_byte()`             | Lê 1 byte de um registrador |
| `mpu6050_read_bytes()`            | Lê múltiplos bytes a partir de um registrador |
| `mpu6050_read_all()`              | Lê todos os dados brutos do sensor |
| `mpu6050_convert_data()`          | Converte dados brutos em unidades físicas |
| `mpu6050_task()`                  | Task de leitura contínua para FreeRTOS |
| `mpu6050_read_acceleration()`     | Lê e converte apenas aceleração em g |

---

## ⚙️ Dependências

- ESP-IDF
- `freertos/FreeRTOS.h`
- `driver/i2c.h`
- `esp_log.h`, `esp_err.h`

---

## 🧪 Exemplo Completo

```c
void app_main() {
    i2c_master_init();
    mpu6050_init();

    mpu6050_data_t raw;
    float accel[3], gyro[3], temp;

    while (1) {
        mpu6050_read_all(&raw);
        mpu6050_convert_data(&raw, accel, gyro, &temp);

        printf("Aceleração: X=%.2fg Y=%.2fg Z=%.2fg\n", accel[0], accel[1], accel[2]);
        printf("Giroscópio: X=%.2f°/s Y=%.2f°/s Z=%.2f°/s\n", gyro[0], gyro[1], gyro[2]);
        printf("Temperatura: %.2f°C\n", temp);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
```

---

## 📎 Licença

MIT ou compatível com o projeto.

