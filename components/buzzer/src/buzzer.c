#include "buzzer.h"
#include <string.h>
#include <stdlib.h>

// Tag para logs
static const char* TAG = "BUZZER";

// Estrutura de controle interno
typedef struct {
    bool initialized;
    buzzer_config_t config;
    TaskHandle_t melody_task_handle;
    QueueHandle_t melody_queue;
    bool is_playing;
    buzzer_melody_t* current_melody;
} buzzer_control_t;

// Instância global de controle
static buzzer_control_t g_buzzer = {0};

// Estrutura para comunicação com task de melodia
typedef struct {
    buzzer_melody_t* melody;
    bool stop_current;
} melody_command_t;

// Declarações de funções internas
static void buzzer_melody_task(void* parameter);
static esp_err_t buzzer_set_frequency_internal(uint32_t frequency_hz);
static esp_err_t buzzer_set_duty_cycle(uint32_t duty_percent);
static uint32_t buzzer_calculate_duty(uint32_t duty_percent);

// ============================================================================
// FUNÇÕES DE INICIALIZAÇÃO
// ============================================================================

esp_err_t buzzer_init(gpio_num_t gpio_num) {
    buzzer_config_t config = {
        .gpio_num = gpio_num,
        .timer_num = LEDC_TIMER_0,
        .channel = LEDC_CHANNEL_0,
        .speed_mode = BUZZER_DEFAULT_SPEED_MODE,
        .duty_resolution = BUZZER_DEFAULT_DUTY_RES,
        .freq_hz = 1000,
        .volume = BUZZER_DEFAULT_VOLUME
    };
    
    return buzzer_init_advanced(&config);
}

esp_err_t buzzer_init_advanced(buzzer_config_t* config) {
    if (config == NULL) {
        ESP_LOGE(TAG, "Config cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    
    if (g_buzzer.initialized) {
        ESP_LOGW(TAG, "Buzzer already initialized");
        return ESP_OK;
    }
    
    // Copia configuração
    memcpy(&g_buzzer.config, config, sizeof(buzzer_config_t));
    
    // Configuração do timer LEDC
    ledc_timer_config_t timer_config = {
        .speed_mode = config->speed_mode,
        .timer_num = config->timer_num,
        .duty_resolution = config->duty_resolution,
        .freq_hz = config->freq_hz,
        .clk_cfg = LEDC_AUTO_CLK
    };
    
    esp_err_t ret = ledc_timer_config(&timer_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure LEDC timer: %s", esp_err_to_name(ret));
        return ret;
    }
    
    // Configuração do canal LEDC
    ledc_channel_config_t channel_config = {
        .speed_mode = config->speed_mode,
        .channel = config->channel,
        .timer_sel = config->timer_num,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = config->gpio_num,
        .duty = 0,
        .hpoint = 0
    };
    
    ret = ledc_channel_config(&channel_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure LEDC channel: %s", esp_err_to_name(ret));
        return ret;
    }
    
    // Criar queue para comandos de melodia
    g_buzzer.melody_queue = xQueueCreate(5, sizeof(melody_command_t));
    if (g_buzzer.melody_queue == NULL) {
        ESP_LOGE(TAG, "Failed to create melody queue");
        return ESP_ERR_NO_MEM;
    }
    
    // Criar task para reprodução de melodias
    BaseType_t task_ret = xTaskCreate(
        buzzer_melody_task,
        "buzzer_melody",
        4096,
        NULL,
        5,
        &g_buzzer.melody_task_handle
    );
    
    if (task_ret != pdPASS) {
        ESP_LOGE(TAG, "Failed to create melody task");
        vQueueDelete(g_buzzer.melody_queue);
        return ESP_ERR_NO_MEM;
    }
    
    g_buzzer.initialized = true;
    g_buzzer.is_playing = false;
    g_buzzer.current_melody = NULL;
    
    ESP_LOGI(TAG, "Buzzer initialized on GPIO %d", config->gpio_num);
    return ESP_OK;
}

esp_err_t buzzer_deinit(void) {
    if (!g_buzzer.initialized) {
        return ESP_OK;
    }
    
    // Parar melodia atual
    buzzer_stop_melody();
    
    // Desligar buzzer
    buzzer_off();
    
    // Deletar task e queue
    if (g_buzzer.melody_task_handle) {
        vTaskDelete(g_buzzer.melody_task_handle);
        g_buzzer.melody_task_handle = NULL;
    }
    
    if (g_buzzer.melody_queue) {
        vQueueDelete(g_buzzer.melody_queue);
        g_buzzer.melody_queue = NULL;
    }
    
    // Limpar estrutura de controle
    memset(&g_buzzer, 0, sizeof(buzzer_control_t));
    
    ESP_LOGI(TAG, "Buzzer deinitialized");
    return ESP_OK;
}

// ============================================================================
// FUNÇÕES DE CONTROLE BÁSICO
// ============================================================================

esp_err_t buzzer_on(uint32_t frequency_hz) {
    if (!g_buzzer.initialized) {
        ESP_LOGE(TAG, "Buzzer not initialized");
        return ESP_ERR_INVALID_STATE;
    }
    
    esp_err_t ret = buzzer_set_frequency_internal(frequency_hz);
    if (ret != ESP_OK) {
        return ret;
    }
    
    return buzzer_set_duty_cycle(g_buzzer.config.volume);
}

esp_err_t buzzer_off(void) {
    if (!g_buzzer.initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    
    return ledc_set_duty(g_buzzer.config.speed_mode, g_buzzer.config.channel, 0) == ESP_OK ?
           ledc_update_duty(g_buzzer.config.speed_mode, g_buzzer.config.channel) : ESP_FAIL;
}

esp_err_t buzzer_play_note(buzzer_note_t note) {
    if (note == NOTE_SILENCE) {
        return buzzer_off();
    }
    
    return buzzer_on(note);
}

esp_err_t buzzer_play_note_timed(buzzer_note_t note, uint32_t duration_ms) {
    esp_err_t ret = buzzer_play_note(note);
    if (ret != ESP_OK) {
        return ret;
    }
    
    vTaskDelay(pdMS_TO_TICKS(duration_ms));
    return buzzer_off();
}

// ============================================================================
// FUNÇÕES DE VOLUME
// ============================================================================

esp_err_t buzzer_set_volume(uint8_t volume) {
    if (!g_buzzer.initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    
    if (volume > BUZZER_MAX_VOLUME) {
        volume = BUZZER_MAX_VOLUME;
    }
    
    g_buzzer.config.volume = volume;
    ESP_LOGD(TAG, "Volume set to %d%%", volume);
    return ESP_OK;
}

uint8_t buzzer_get_volume(void) {
    return g_buzzer.config.volume;
}

// ============================================================================
// FUNÇÕES PARA EFEITOS SONOROS
// ============================================================================

esp_err_t buzzer_play_effect(buzzer_effect_t effect) {
    if (!g_buzzer.initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    
    switch (effect) {
        case BUZZER_EFFECT_BEEP:
            return buzzer_beep(1000, 200);
            
        case BUZZER_EFFECT_DOUBLE_BEEP:
            return buzzer_beep_multiple(1000, 150, 100, 2);
            
        case BUZZER_EFFECT_TRIPLE_BEEP:
            return buzzer_beep_multiple(1000, 100, 100, 3);
            
        case BUZZER_EFFECT_ALARM:
            return buzzer_beep_multiple(800, 300, 300, 3);
            
        case BUZZER_EFFECT_SIREN:
            return buzzer_frequency_sweep(400, 1200, 1000, 50);
            
        case BUZZER_EFFECT_STARTUP: {
            uint32_t freqs[] = {523, 659, 784, 1047};
            uint32_t durs[] = {150, 150, 150, 300};
            return buzzer_play_sequence(freqs, durs, 4);
        }
        
        case BUZZER_EFFECT_SUCCESS: {
            uint32_t freqs[] = {659, 784, 1047};
            uint32_t durs[] = {200, 200, 400};
            return buzzer_play_sequence(freqs, durs, 3);
        }
        
        case BUZZER_EFFECT_ERROR: {
            uint32_t freqs[] = {392, 330, 277};
            uint32_t durs[] = {300, 300, 600};
            return buzzer_play_sequence(freqs, durs, 3);
        }
        
        case BUZZER_EFFECT_NOTIFICATION:
            return buzzer_beep_multiple(880, 100, 50, 2);
            
        default:
            ESP_LOGE(TAG, "Unknown effect: %d", effect);
            return ESP_ERR_INVALID_ARG;
    }
}

esp_err_t buzzer_beep(uint32_t frequency_hz, uint32_t duration_ms) {
    esp_err_t ret = buzzer_on(frequency_hz);
    if (ret != ESP_OK) {
        return ret;
    }
    
    vTaskDelay(pdMS_TO_TICKS(duration_ms));
    return buzzer_off();
}

esp_err_t buzzer_beep_multiple(uint32_t frequency_hz, uint32_t duration_ms, 
                               uint32_t pause_ms, uint8_t count) {
    for (uint8_t i = 0; i < count; i++) {
        esp_err_t ret = buzzer_beep(frequency_hz, duration_ms);
        if (ret != ESP_OK) {
            return ret;
        }
        
        if (i < count - 1) {  // Não pausar após o último beep
            vTaskDelay(pdMS_TO_TICKS(pause_ms));
        }
    }
    
    return ESP_OK;
}

// ============================================================================
// FUNÇÕES PARA MELODIAS
// ============================================================================

esp_err_t buzzer_play_melody(buzzer_melody_t* melody) {
    if (!g_buzzer.initialized || melody == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    
    melody_command_t cmd = {
        .melody = melody,
        .stop_current = true
    };
    
    if (xQueueSend(g_buzzer.melody_queue, &cmd, pdMS_TO_TICKS(100)) != pdPASS) {
        ESP_LOGE(TAG, "Failed to queue melody");
        return ESP_ERR_TIMEOUT;
    }
    
    return ESP_OK;
}

esp_err_t buzzer_stop_melody(void) {
    if (!g_buzzer.initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    
    melody_command_t cmd = {
        .melody = NULL,
        .stop_current = true
    };
    
    xQueueSend(g_buzzer.melody_queue, &cmd, 0);
    return ESP_OK;
}

bool buzzer_is_playing(void) {
    return g_buzzer.is_playing;
}

// ============================================================================
// FUNÇÕES AVANÇADAS
// ============================================================================

esp_err_t buzzer_frequency_sweep(uint32_t start_freq_hz, uint32_t end_freq_hz, 
                                uint32_t duration_ms, uint16_t steps) {
    if (!g_buzzer.initialized || steps == 0) {
        return ESP_ERR_INVALID_ARG;
    }
    
    uint32_t step_duration = duration_ms / steps;
    int32_t freq_step = (int32_t)(end_freq_hz - start_freq_hz) / steps;
    
    for (uint16_t i = 0; i < steps; i++) {
        uint32_t current_freq = start_freq_hz + (freq_step * i);
        
        esp_err_t ret = buzzer_on(current_freq);
        if (ret != ESP_OK) {
            buzzer_off();
            return ret;
        }
        
        vTaskDelay(pdMS_TO_TICKS(step_duration));
    }
    
    return buzzer_off();
}

esp_err_t buzzer_play_sequence(uint32_t* frequencies, uint32_t* durations, size_t count) {
    if (!g_buzzer.initialized || frequencies == NULL || durations == NULL || count == 0) {
        return ESP_ERR_INVALID_ARG;
    }
    
    for (size_t i = 0; i < count; i++) {
        esp_err_t ret = buzzer_beep(frequencies[i], durations[i]);
        if (ret != ESP_OK) {
            return ret;
        }
        
        // Pequena pausa entre notas
        if (i < count - 1) {
            vTaskDelay(pdMS_TO_TICKS(50));
        }
    }
    
    return ESP_OK;
}

// ============================================================================
// FUNÇÕES AUXILIARES PARA CRIAÇÃO DE MELODIAS
// ============================================================================

buzzer_melody_t* buzzer_create_melody(buzzer_note_t* notes, uint32_t* durations, 
                                     size_t count, uint32_t tempo_bpm) {
    if (notes == NULL || durations == NULL || count == 0 || count > BUZZER_MAX_MELODY_SIZE) {
        return NULL;
    }
    
    buzzer_melody_t* melody = malloc(sizeof(buzzer_melody_t));
    if (melody == NULL) {
        return NULL;
    }
    
    melody->notes = malloc(sizeof(buzzer_melody_note_t) * count);
    if (melody->notes == NULL) {
        free(melody);
        return NULL;
    }
    
    // Copiar notas e durações
    for (size_t i = 0; i < count; i++) {
        melody->notes[i].note = notes[i];
        melody->notes[i].duration_ms = durations[i];
    }
    
    melody->num_notes = count;
    melody->loop = false;
    melody->tempo_bpm = tempo_bpm;
    
    return melody;
}

void buzzer_free_melody(buzzer_melody_t* melody) {
    if (melody != NULL) {
        if (melody->notes != NULL) {
            free(melody->notes);
        }
        free(melody);
    }
}

// ============================================================================
// FUNÇÕES INTERNAS
// ============================================================================

static void buzzer_melody_task(void* parameter) {
    melody_command_t cmd;
    
    while (1) {
        if (xQueueReceive(g_buzzer.melody_queue, &cmd, portMAX_DELAY) == pdPASS) {
            if (cmd.stop_current) {
                buzzer_off();
                g_buzzer.is_playing = false;
                g_buzzer.current_melody = NULL;
            }
            
            if (cmd.melody != NULL) {
                g_buzzer.is_playing = true;
                g_buzzer.current_melody = cmd.melody;
                
                do {
                    for (size_t i = 0; i < cmd.melody->num_notes && g_buzzer.is_playing; i++) {
                        buzzer_melody_note_t* note = &cmd.melody->notes[i];
                        
                        if (note->note == NOTE_SILENCE) {
                            buzzer_off();
                        } else {
                            buzzer_on(note->note);
                        }
                        
                        vTaskDelay(pdMS_TO_TICKS(note->duration_ms));
                        
                        // Verificar se há comando para parar
                        if (uxQueueMessagesWaiting(g_buzzer.melody_queue) > 0) {
                            break;
                        }
                    }
                } while (cmd.melody->loop && g_buzzer.is_playing);
                
                buzzer_off();
                g_buzzer.is_playing = false;
                g_buzzer.current_melody = NULL;
            }
        }
    }
}

static esp_err_t buzzer_set_frequency_internal(uint32_t frequency_hz) {
    if (frequency_hz == 0) {
        return buzzer_off();
    }
    
    esp_err_t ret = ledc_set_freq(g_buzzer.config.speed_mode, 
                                  g_buzzer.config.timer_num, 
                                  frequency_hz);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set frequency: %s", esp_err_to_name(ret));
        return ret;
    }
    
    g_buzzer.config.freq_hz = frequency_hz;
    return ESP_OK;
}

static esp_err_t buzzer_set_duty_cycle(uint32_t duty_percent) {
    uint32_t duty = buzzer_calculate_duty(duty_percent);
    
    esp_err_t ret = ledc_set_duty(g_buzzer.config.speed_mode, 
                                  g_buzzer.config.channel, 
                                  duty);
    if (ret != ESP_OK) {
        return ret;
    }
    
    return ledc_update_duty(g_buzzer.config.speed_mode, g_buzzer.config.channel);
}

static uint32_t buzzer_calculate_duty(uint32_t duty_percent) {
    if (duty_percent > 100) {
        duty_percent = 100;
    }
    
    uint32_t max_duty = (1 << g_buzzer.config.duty_resolution) - 1;
    return (max_duty * duty_percent) / 100;
}

// ============================================================================
// MELODIAS PRÉ-DEFINIDAS
// ============================================================================

// Mario Coin Sound
static buzzer_melody_note_t mario_coin_notes[] = {
    {NOTE_B5, 100},
    {NOTE_E6, 200}
};

const buzzer_melody_t buzzer_melody_mario_coin = {
    .notes = mario_coin_notes,
    .num_notes = 2,
    .loop = false,
    .tempo_bpm = 120
};

// Nokia Ringtone
static buzzer_melody_note_t nokia_notes[] = {
    {NOTE_E5, 200}, {NOTE_D5, 200}, {NOTE_FS4, 400}, {NOTE_GS4, 400},
    {NOTE_CS5, 200}, {NOTE_B4, 200}, {NOTE_D4, 400}, {NOTE_E4, 400},
    {NOTE_B4, 200}, {NOTE_A4, 200}, {NOTE_CS4, 400}, {NOTE_E4, 400},
    {NOTE_A4, 800}
};

const buzzer_melody_t buzzer_melody_nokia_ringtone = {
    .notes = nokia_notes,
    .num_notes = 13,
    .loop = false,
    .tempo_bpm = 180
};

// Twinkle Twinkle Little Star
static buzzer_melody_note_t twinkle_notes[] = {
    {NOTE_C4, 400}, {NOTE_C4, 400}, {NOTE_G4, 400}, {NOTE_G4, 400},
    {NOTE_A4, 400}, {NOTE_A4, 400}, {NOTE_G4, 800},
    {NOTE_F4, 400}, {NOTE_F4, 400}, {NOTE_E4, 400}, {NOTE_E4, 400},
    {NOTE_D4, 400}, {NOTE_D4, 400}, {NOTE_C4, 800}
};

const buzzer_melody_t buzzer_melody_twinkle_star = {
    .notes = twinkle_notes,
    .num_notes = 14,
    .loop = false,
    .tempo_bpm = 120
};

// Happy Birthday
static buzzer_melody_note_t birthday_notes[] = {
    {NOTE_C4, 300}, {NOTE_C4, 200}, {NOTE_D4, 500}, {NOTE_C4, 500},
    {NOTE_F4, 500}, {NOTE_E4, 1000},
    {NOTE_C4, 300}, {NOTE_C4, 200}, {NOTE_D4, 500}, {NOTE_C4, 500},
    {NOTE_G4, 500}, {NOTE_F4, 1000},
    {NOTE_C4, 300}, {NOTE_C4, 200}, {NOTE_C5, 500}, {NOTE_A4, 500},
    {NOTE_F4, 500}, {NOTE_E4, 500}, {NOTE_D4, 1000},
    {NOTE_AS4, 300}, {NOTE_AS4, 200}, {NOTE_A4, 500}, {NOTE_F4, 500},
    {NOTE_G4, 500}, {NOTE_F4, 1000}
};

const buzzer_melody_t buzzer_melody_happy_birthday = {
    .notes = birthday_notes,
    .num_notes = 25,
    .loop = false,
    .tempo_bpm = 120
};