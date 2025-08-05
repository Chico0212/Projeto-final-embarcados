#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "ssd1306.h"
#include "button.h"
#include "tilt_maze.h"

// GPIO definitions for buttons
#define BUTTON_UP_GPIO       GPIO_NUM_4   // Button for navigating up/previous
#define BUTTON_SELECT_GPIO   GPIO_NUM_5   // Button for selecting/confirming

// Menu configuration
#define MAX_MENU_ITEMS       4
#define MENU_ITEM_HEIGHT     12
#define MENU_START_Y         8
#define MENU_ITEM_SPACING    2

// Game menu items
typedef enum {
    GAME_TILT_MAZE = 0,
    GAME_DODGE_BLOCKS,
    GAME_SNAKE_TILT,
    GAME_PADDLE_PONG
} game_selection_t;

// Menu state
typedef struct {
    int current_selection;
    bool menu_active;
} menu_state_t;

static menu_state_t menu_state = {0, true};

static const char* menu_items[MAX_MENU_ITEMS] = {
    "Tilt Maze",
    "Dodge the Blocks", 
    "Snake Tilt",
    "Paddle Pong"
};

// Button event queue
static QueueHandle_t button_queue = NULL;

// Button ISR callback
static void button_isr_callback(gpio_num_t gpio_num, button_event_t event)
{
    button_event_data_t event_data = {
        .gpio_num = gpio_num,
        .event = event,
        .timestamp = xTaskGetTickCount()
    };
    
    if (button_queue != NULL) {
        xQueueSendFromISR(button_queue, &event_data, NULL);
    }
}

// Initialize buttons
static esp_err_t init_menu_buttons(void)
{
    // Create button event queue
    button_queue = xQueueCreate(10, sizeof(button_event_data_t));
    if (button_queue == NULL) {
        ESP_LOGE("MENU", "Failed to create button queue");
        return ESP_FAIL;
    }

    // Configure GPIO for buttons
    gpio_config_t gpio_config = {
        .pin_bit_mask = (1ULL << BUTTON_UP_GPIO) | (1ULL << BUTTON_SELECT_GPIO),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_NEGEDGE
    };

    esp_err_t ret = init_buttons_isr(&gpio_config, button_isr_callback);
    if (ret != ESP_OK) {
        ESP_LOGE("MENU", "Failed to configure GPIO");
        return ret;
    }

    // Initialize buttons with ISR
    ret = init_buttons_isr(&gpio_config, button_isr_callback);
    if (ret != ESP_OK) {
        ESP_LOGE("MENU", "Failed to initialize buttons");
        return ret;
    }

    ESP_LOGI("MENU", "Buttons initialized successfully");
    return ESP_OK;
}

// Draw menu on display
static void draw_menu(void)
{
    // Clear display
    ssd1306_clear_buffer();
    
    // Draw title
    ssd1306_draw_string(0, 0, "GAME MENU");
    
    // Draw menu items
    for (int i = 0; i < MAX_MENU_ITEMS; i++) {
        int y_pos = MENU_START_Y + (i * (MENU_ITEM_HEIGHT + MENU_ITEM_SPACING));
        
        // Highlight selected item
        if (i == menu_state.current_selection) {
            // Draw selection box
            ssd1306_draw_rect(0, y_pos - 1, SSD1306_WIDTH, MENU_ITEM_HEIGHT + 2, false);
            // Draw arrow indicator
            ssd1306_draw_string(2, y_pos, ">");
        }
        
        // Draw menu item text
        ssd1306_draw_string(12, y_pos, menu_items[i]);
    }
    
    // Draw instructions at bottom
    ssd1306_draw_string(0, SSD1306_HEIGHT - 8, "UP:Nav  SEL:Play");
    
    // Update display
    ssd1306_update_display();
}

// Handle button events
static void handle_button_event(button_event_data_t* event_data)
{
    if (event_data->event == BUTTON_EVENT_PRESSED) {
        if (event_data->gpio_num == BUTTON_UP_GPIO) {
            // Navigate up in menu
            menu_state.current_selection = (menu_state.current_selection - 1 + MAX_MENU_ITEMS) % MAX_MENU_ITEMS;
            ESP_LOGI("MENU", "Navigated to: %s", menu_items[menu_state.current_selection]);
            draw_menu();
        }
        else if (event_data->gpio_num == BUTTON_SELECT_GPIO) {
            // Select current game
            game_selection_t selected_game = (game_selection_t)menu_state.current_selection;
            ESP_LOGI("MENU", "Selected game: %s", menu_items[selected_game]);
            
            // TODO: Launch selected game
            // For now, just show a selection message
            ssd1306_clear_buffer();
            ssd1306_draw_string(0, 10, "Selected:");
            ssd1306_draw_string(0, 25, menu_items[selected_game]);
            ssd1306_draw_string(0, 45, "Press UP to return");
            ssd1306_update_display();
            
            menu_state.menu_active = false;
        }
    }
}

// Menu task
static void menu_task(void* pvParameters)
{
    button_event_data_t event_data;
    
    // Draw initial menu
    draw_menu();
    
    while (1) {
        // Wait for button events
        if (xQueueReceive(button_queue, &event_data, pdMS_TO_TICKS(100)) == pdTRUE) {
            handle_button_event(&event_data);
        }
        
        // If menu is not active, check for return to menu
        if (!menu_state.menu_active) {
            // Check if UP button is pressed to return to menu
            if (button_read_debounced(BUTTON_UP_GPIO) == 0) {
                menu_state.menu_active = true;
                draw_menu();
            }
        }
        
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void app_main(void)
{
    ESP_LOGI("MAIN", "Starting Game Menu System");
    
    // Initialize SSD1306 display
    ssd1306_init();
    ESP_LOGI("MAIN", "SSD1306 display initialized");
    
    // Initialize buttons
    esp_err_t ret = init_menu_buttons();
    if (ret != ESP_OK) {
        ESP_LOGE("MAIN", "Failed to initialize buttons");
        return;
    }

    draw_menu();

    
    // Create menu task
    // xTaskCreate(menu_task, "menu_task", 4096, NULL, 5, NULL);
    
    ESP_LOGI("MAIN", "Game menu system started successfully");
}
