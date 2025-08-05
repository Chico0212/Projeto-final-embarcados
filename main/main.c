#include "menu.h"

// === MAIN APPLICATION ===

void app_main(void)
{
    ESP_LOGI("MAIN", "Starting Game Menu System");

    // Initialize I2C and sensors
    esp_err_t ret = i2c_master_init();
    if (ret != ESP_OK) {
        ESP_LOGE("MAIN", "Failed to initialize I2C: %s", esp_err_to_name(ret));
        return;
    }
    
    ret = mpu6050_init();
    if (ret != ESP_OK) {
        ESP_LOGE("MAIN", "Failed to initialize MPU6050: %s", esp_err_to_name(ret));
        // Continue anyway, games might still work without accelerometer
    }

    // Initialize SSD1306 display
    i2c_init();
    vTaskDelay(100 / portTICK_PERIOD_MS);
    ssd1306_init();
    ESP_LOGI("MAIN", "SSD1306 display initialized");

    // Initialize buttons
    ret = init_menu_buttons();
    if (ret != ESP_OK) {
        ESP_LOGE("MAIN", "Failed to initialize buttons: %s", esp_err_to_name(ret));
        return;
    }

    // Create menu task
    xTaskCreate(menu_task, "menu_task", 4096, NULL, 5, NULL);

    ESP_LOGI("MAIN", "Game menu system started successfully");
    
    // Keep main task alive
    while (1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}