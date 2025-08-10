#include "menu.h"

static menu_state_t menu_state = {0, true, false};

static button_state_t button_states[2] = {
    {false, false, 0, 0, false},
    {false, false, 0, 0, false}};

static const char *menu_items[MAX_MENU_ITEMS] = {
    "Tilt maze",
    "Dodge the blocks",
    "Snake tilt",
    "Paddle pong"};

static QueueHandle_t button_queue = NULL;

// void IRAM_ATTR button_isr_handler(void *arg) // pode ir pra lib dos botões tranquilamente
// {
//     gpio_num_t gpio_num = (gpio_num_t)(uint32_t)arg;

//     static TickType_t last_isr_time[2] = {0, 0};
//     int button_index = (gpio_num == BUTTON_NAV_GPIO) ? 0 : 1;

//     TickType_t current_time = xTaskGetTickCountFromISR();

//     if ((current_time - last_isr_time[button_index]) < pdMS_TO_TICKS(BUTTON_DEBOUNCE_MS))
//     {
//         return;
//     }

//     last_isr_time[button_index] = current_time;

//     button_event_data_t event_data = {
//         .gpio_num = gpio_num,
//         .event = BUTTON_EVENT_PRESSED,
//         .timestamp = xTaskGetTickCountFromISR()
//     };

//     BaseType_t xHigherPriorityTaskWoken = pdFALSE;
//     xQueueSendFromISR(button_queue, &event_data, &xHigherPriorityTaskWoken);

//     if (xHigherPriorityTaskWoken)
//     {
//         portYIELD_FROM_ISR();
//     }
// }

void draw_title(void)
{
    // Draw main title with border
    ssd1306_draw_string(0, 0, "Game menu");

    // Draw underline for title
    ssd1306_draw_line(0, TITLE_HEIGHT - 2, DISPLAY_WIDTH - 1, TITLE_HEIGHT - 2);
}

void draw_menu_item(int item_index, bool is_selected)
{
    int y_pos = MENU_START_Y + (item_index * (MENU_ITEM_HEIGHT + MENU_ITEM_SPACING));

    if (is_selected)
    {
        // Draw selection background box (filled rectangle)
        ssd1306_draw_rect(0, y_pos - SELECTION_BOX_MARGIN,
                          DISPLAY_WIDTH, MENU_ITEM_HEIGHT + (2 * SELECTION_BOX_MARGIN),
                          false); // false = filled rectangle

        // Draw selection arrow
        ssd1306_draw_string(SELECTION_ARROW_X, y_pos, ">");

        // Draw item text normally (will appear inverted due to filled background)
        ssd1306_draw_string(MENU_TEXT_X, y_pos, menu_items[item_index]);
    }
    else
    {
        // Draw normal item text
        ssd1306_draw_string(MENU_TEXT_X, y_pos, menu_items[item_index]);
    }
}

void draw_instructions(void)
{
    ssd1306_clear_buffer();

    // Show loading screen
    ssd1306_draw_string(0, 15, "Red: Navigate");
    ssd1306_draw_string(0, 30, "Green: Select");

    ssd1306_update_display();
    vTaskDelay(3000 / portTICK_PERIOD_MS);

    ssd1306_clear_buffer();

    // Show loading screen
    ssd1306_draw_string(0, 15, "Press anything");
    ssd1306_draw_string(0, 30, "during the game");
    ssd1306_draw_string(0, 45, "to exit");

    ssd1306_update_display();
    vTaskDelay(3000 / portTICK_PERIOD_MS);
}

void draw_status_info(void)
{
    // Optional: Draw current selection info
    char status_text[32];
    snprintf(status_text, sizeof(status_text), "%d/%d",
             menu_state.current_selection + 1, MAX_MENU_ITEMS);
    ssd1306_draw_string(DISPLAY_WIDTH - 24, 0, status_text);
}

void draw_complete_menu(void)
{
    // Clear the display buffer
    ssd1306_clear_buffer();

    // Draw all menu components
    draw_title();

    // Draw all menu items
    for (int i = 0; i < MAX_MENU_ITEMS; i++)
    {
        draw_menu_item(i, (i == menu_state.current_selection));
    }

    draw_status_info();

    // draw_instructions();
    // Update the physical display
    ssd1306_update_display();
}

void show_game_loading(game_selection_t game)
{
    ssd1306_clear_buffer();

    // Show loading screen
    ssd1306_draw_string(0, 15, "Loading...");
    ssd1306_draw_string(0, 30, menu_items[game]);

    // Draw simple loading animation
    ssd1306_draw_rect(0, 45, 88, 8, true);  // Loading bar border
    ssd1306_draw_rect(0, 47, 84, 4, false); // Loading bar fill

    ssd1306_update_display();
    vTaskDelay(1500 / portTICK_PERIOD_MS); // Simulate loading
}

void launch_game(game_selection_t selected_game)
{
    ESP_LOGI("menu", "Launching game: %s", menu_items[selected_game]);

    show_game_loading(selected_game);

    menu_state.in_game = true;
    menu_state.menu_active = false;

    switch (selected_game)
    {
    case GAME_TILT_MAZE:
        start_tilt_maze_game();
        break;
    case GAME_DODGE_BLOCKS:
        start_dodge_blocks_game();
        break;
    case GAME_SNAKE_TILT:
        start_snake_game();
        break;
    case GAME_PADDLE_PONG:
        start_paddle_pong_game();
        break;
    }
}

const char *get_task_state(eTaskState state)
{
    switch (state)
    {
    case eRunning:
        return "Running";
    case eReady:
        return "Ready";
    case eBlocked:
        return "Blocked";
    case eSuspended:
        return "Suspended";
    case eDeleted:
        return "Deleted";
    case eInvalid:
        return "Invalid";
    default:
        return "Unknown";
    }
}

void kill_task_safely(TaskHandle_t task)
{
    if (task)
    {
        xTaskNotify(task, NOTIF_STOP, eSetBits);
    }
}

void return_to_menu(void)
{
    ESP_LOGI("MENU", "Returning to main menu");

    kill_task_safely(tilt_maze_task_handle);
    kill_task_safely(dodge_blocks_game_task_handle);
    kill_task_safely(paddle_pong_game_task_handle);
    kill_task_safely(snake_game_task_handle);

    ESP_LOGI("MENU", "Task finalizada com sucesso");

    menu_state.menu_active = true;
    menu_state.in_game = false;

    // Redraw the menu
    draw_complete_menu();
}

void handle_menu_navigation(void)
{
    // Move to previous item (wrapping around)
    menu_state.current_selection = (menu_state.current_selection + 1 + MAX_MENU_ITEMS) % MAX_MENU_ITEMS;
    ESP_LOGI("MENU", "Navigated to: %s", menu_items[menu_state.current_selection]);

    // Redraw menu with new selection
    draw_complete_menu();
}

void handle_menu_selection(void)
{
    game_selection_t selected_game = (game_selection_t)menu_state.current_selection;
    launch_game(selected_game);
}

void handle_button_event(button_event_data_t *event_data)
{
    if (event_data->event != BUTTON_EVENT_PRESSED)
    {
        return;
    }

    ESP_LOGI("MENU", "Evento de botão processado: GPIO %d", event_data->gpio_num);

    if (menu_state.menu_active)
    {
        if (event_data->gpio_num == BUTTON_NAV_GPIO)
        {
            handle_menu_navigation();
        }
        else if (event_data->gpio_num == BUTTON_SELECT_GPIO)
        {
            handle_menu_selection();
        }
    }
    else if (menu_state.in_game)
    {
        return_to_menu();
    }
}

esp_err_t init_menu_buttons(void) // adaptar pra usar a lib
{
    ESP_LOGI("MENU", "Initializing buttons...");

    // Create button event queue
    button_queue = xQueueCreate(10, sizeof(button_event_data_t)); // acho que isso é criada na lib
    if (button_queue == NULL)
    {
        ESP_LOGE("MENU", "Failed to create button queue");
        return ESP_FAIL;
    }

    // Configure GPIO for buttons with pull-up resistors
    gpio_config_t gpio_conf = {
        .pin_bit_mask = (1ULL << BUTTON_NAV_GPIO) | (1ULL << BUTTON_SELECT_GPIO),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_NEGEDGE};

    esp_err_t ret = init_buttons_isr(&gpio_conf, handle_button_event);
    if (ret != ESP_OK)
    {
        ESP_LOGE("MENU", "Failed to configure GPIO: %s", esp_err_to_name(ret));
        return ret;
    }

    // gpio_install_isr_service(0);
    // gpio_isr_handler_add(BUTTON_NAV_GPIO, button_isr_handler, (void *)BUTTON_NAV_GPIO);
    // gpio_isr_handler_add(BUTTON_SELECT_GPIO, button_isr_handler, (void *)BUTTON_SELECT_GPIO);

    ESP_LOGI("MENU", "Buttons initialized successfully");
    return ESP_OK;
}

void show_boot_screen(void)
{
    ssd1306_clear_buffer();

    // Boot animation
    ssd1306_draw_string(0, 15, "Game console");
    ssd1306_draw_string(0, 30, "v1.0");
    ssd1306_draw_string(0, 45, "Loading...");

    ssd1306_update_display();
    vTaskDelay(2000 / portTICK_PERIOD_MS);
}

void menu_task(void *pvParameters)
{
    button_event_data_t event_data;

    // Show boot screen
    show_boot_screen();

    draw_instructions();

    buzzer_mario_theme();

    // Draw initial menu
    draw_complete_menu();

    ESP_LOGI("MENU", "Menu system ready");

    while (1)
    {
        // Wait for button events with timeout
        if (button_wait_event(&event_data, BUTTON_DEBOUNCE_TIME_US) == ESP_OK)
        {
            ESP_LOGI("MENU", "Button event received: GPIO %d", event_data.gpio_num);
            handle_button_event(&event_data);
        }

        // Small delay to prevent excessive CPU usage
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
