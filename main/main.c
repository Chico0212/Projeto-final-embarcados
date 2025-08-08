#include "menu.h"

const char* TAG = "MAIN";

void app_main(void)
{
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
    
    ret = mpu6050_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize MPU6050: %s", esp_err_to_name(ret));
        // Continue anyway, games might still work without accelerometer
    }

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