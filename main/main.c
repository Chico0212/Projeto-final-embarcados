#include "menu.h"

const char* TAG = "MAIN";

void app_main(void)
{
    // esp_log_level_set("*", ESP_LOG_VERBOSE);
    // esp_log_level_set("SSD1306", ESP_LOG_VERBOSE);
    buzzer_pwm_init();
    ESP_LOGI(TAG, "Starting Game Menu System");

    // Initialize I2C and sensors
    esp_err_t ret = i2c_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize I2C: %s", esp_err_to_name(ret));
        // Continue anyway, games might still work without accelerometer
    }

    i2c_scan();
    
    ssd1306_init();
    
    vTaskDelay(pdMS_TO_TICKS(100));
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize I2C: %s", esp_err_to_name(ret));
        return;
    }
    // // ssd1306_test_pattern();

    // ssd1306_clear_buffer();
    // vTaskDelay(pdMS_TO_TICKS(10));
    
    // ssd1306_update_display();
    // vTaskDelay(pdMS_TO_TICKS(10));

    // ssd1306_draw_string(50, 0, "Teste");
    // vTaskDelay(pdMS_TO_TICKS(10));

    // ssd1306_test_pattern();
    // ESP_LOGI(TAG, "SSD1306 display initialized");
    
    
    ret = mpu6050_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize MPU6050: %s", esp_err_to_name(ret));
        // Continue anyway, games might still work without accelerometer
    }

    // mpu6050_data_t teste;

    // mpu6050_read_all(&teste);

    // float a[3], b[3], c[3];

    // mpu6050_convert_data(&teste, a, b, c);

    // // Initialize SSD1306 display

    // Initialize buttons
    ret = init_menu_buttons();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize buttons: %s", esp_err_to_name(ret));
        return;
    }

    // Create menu task
    xTaskCreate(menu_task, "menu_task", 4096, NULL, 5, NULL);

    ESP_LOGI(TAG, "Game menu system started successfully");
    
    // Keep main task alive
    while (1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}