#ifndef BUZZER_H
#define BUZZER_H

#include "driver/ledc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

// Definições de constantes
#define BUZZER_DEFAULT_VOLUME       50      // Volume padrão (0-100%)
#define BUZZER_MAX_VOLUME          100      // Volume máximo
#define BUZZER_DEFAULT_DUTY_RES    LEDC_TIMER_13_BIT
#define BUZZER_DEFAULT_SPEED_MODE  LEDC_LOW_SPEED_MODE
#define BUZZER_MAX_MELODY_SIZE     50       // Máximo de notas em uma melodia

// Enumeração para notas musicais (frequências em Hz)
typedef enum {
    NOTE_SILENCE = 0,
    
    // Oitava 3
    NOTE_C3 = 131,
    NOTE_CS3 = 139,    // C# / Db
    NOTE_D3 = 147,
    NOTE_DS3 = 156,    // D# / Eb
    NOTE_E3 = 165,
    NOTE_F3 = 175,
    NOTE_FS3 = 185,    // F# / Gb
    NOTE_G3 = 196,
    NOTE_GS3 = 208,    // G# / Ab
    NOTE_A3 = 220,
    NOTE_AS3 = 233,    // A# / Bb
    NOTE_B3 = 247,
    
    // Oitava 4 (mais comum)
    NOTE_C4 = 262,     // Dó central
    NOTE_CS4 = 277,
    NOTE_D4 = 294,
    NOTE_DS4 = 311,
    NOTE_E4 = 330,
    NOTE_F4 = 349,
    NOTE_FS4 = 370,
    NOTE_G4 = 392,
    NOTE_GS4 = 415,
    NOTE_A4 = 440,     // Lá padrão
    NOTE_AS4 = 466,
    NOTE_B4 = 494,
    
    // Oitava 5
    NOTE_C5 = 523,
    NOTE_CS5 = 554,
    NOTE_D5 = 587,
    NOTE_DS5 = 622,
    NOTE_E5 = 659,
    NOTE_F5 = 698,
    NOTE_FS5 = 740,
    NOTE_G5 = 784,
    NOTE_GS5 = 831,
    NOTE_A5 = 880,
    NOTE_AS5 = 932,
    NOTE_B5 = 988,
    
    // Oitava 6
    NOTE_C6 = 1047,
    NOTE_CS6 = 1109,
    NOTE_D6 = 1175,
    NOTE_DS6 = 1245,
    NOTE_E6 = 1319,
    NOTE_F6 = 1397,
    NOTE_FS6 = 1480,
    NOTE_G6 = 1568,
    NOTE_GS6 = 1661,
    NOTE_A6 = 1760,
    NOTE_AS6 = 1865,
    NOTE_B6 = 1976
} buzzer_note_t;

// Estrutura para uma nota com duração
typedef struct {
    buzzer_note_t note;      // Frequência da nota
    uint32_t duration_ms;    // Duração em milissegundos
} buzzer_melody_note_t;

// Estrutura para configuração do buzzer
typedef struct {
    gpio_num_t gpio_num;             // GPIO do buzzer
    ledc_timer_t timer_num;          // Timer LEDC
    ledc_channel_t channel;          // Canal LEDC
    ledc_mode_t speed_mode;          // Modo de velocidade
    ledc_timer_bit_t duty_resolution; // Resolução do duty cycle
    uint32_t freq_hz;                // Frequência inicial
    uint8_t volume;                  // Volume (0-100%)
} buzzer_config_t;

// Estrutura para melodia
typedef struct {
    buzzer_melody_note_t* notes;     // Array de notas
    size_t num_notes;                // Número de notas
    bool loop;                       // Se deve repetir a melodia
    uint32_t tempo_bpm;              // Tempo em BPM (beats por minuto)
} buzzer_melody_t;

// Enumeração para tipos de efeitos sonoros
typedef enum {
    BUZZER_EFFECT_BEEP,          // Beep simples
    BUZZER_EFFECT_DOUBLE_BEEP,   // Dois beeps
    BUZZER_EFFECT_TRIPLE_BEEP,   // Três beeps
    BUZZER_EFFECT_ALARM,         // Alarme alternando frequências
    BUZZER_EFFECT_SIREN,         // Sirene com sweep de frequência
    BUZZER_EFFECT_STARTUP,       // Som de inicialização
    BUZZER_EFFECT_SUCCESS,       // Som de sucesso
    BUZZER_EFFECT_ERROR,         // Som de erro
    BUZZER_EFFECT_NOTIFICATION   // Som de notificação
} buzzer_effect_t;

// Funções principais de inicialização
/**
 * @brief Inicializa o buzzer com configuração padrão
 * @param gpio_num GPIO conectado ao buzzer
 * @return esp_err_t Status da operação
 */
esp_err_t buzzer_init(gpio_num_t gpio_num);

/**
 * @brief Inicializa o buzzer com configuração avançada
 * @param config Configuração do buzzer
 * @return esp_err_t Status da operação
 */
esp_err_t buzzer_init_advanced(buzzer_config_t* config);

/**
 * @brief Deinicializa o buzzer e libera recursos
 * @return esp_err_t Status da operação
 */
esp_err_t buzzer_deinit(void);

// Funções de controle básico
/**
 * @brief Liga o buzzer com uma frequência específica
 * @param frequency_hz Frequência em Hz
 * @return esp_err_t Status da operação
 */
esp_err_t buzzer_on(uint32_t frequency_hz);

/**
 * @brief Desliga o buzzer
 * @return esp_err_t Status da operação
 */
esp_err_t buzzer_off(void);

/**
 * @brief Toca uma nota musical
 * @param note Nota musical a ser tocada
 * @return esp_err_t Status da operação
 */
esp_err_t buzzer_play_note(buzzer_note_t note);

/**
 * @brief Toca uma nota por um tempo específico
 * @param note Nota musical
 * @param duration_ms Duração em milissegundos
 * @return esp_err_t Status da operação
 */
esp_err_t buzzer_play_note_timed(buzzer_note_t note, uint32_t duration_ms);

// Funções de volume
/**
 * @brief Define o volume do buzzer
 * @param volume Volume de 0 a 100%
 * @return esp_err_t Status da operação
 */
esp_err_t buzzer_set_volume(uint8_t volume);

/**
 * @brief Obtém o volume atual
 * @return uint8_t Volume atual (0-100%)
 */
uint8_t buzzer_get_volume(void);

// Funções para efeitos sonoros
/**
 * @brief Toca um efeito sonoro pré-definido
 * @param effect Tipo do efeito sonoro
 * @return esp_err_t Status da operação
 */
esp_err_t buzzer_play_effect(buzzer_effect_t effect);

/**
 * @brief Toca um beep simples
 * @param frequency_hz Frequência do beep
 * @param duration_ms Duração do beep
 * @return esp_err_t Status da operação
 */
esp_err_t buzzer_beep(uint32_t frequency_hz, uint32_t duration_ms);

/**
 * @brief Toca múltiplos beeps
 * @param frequency_hz Frequência dos beeps
 * @param duration_ms Duração de cada beep
 * @param pause_ms Pausa entre beeps
 * @param count Número de beeps
 * @return esp_err_t Status da operação
 */
esp_err_t buzzer_beep_multiple(uint32_t frequency_hz, uint32_t duration_ms, 
                               uint32_t pause_ms, uint8_t count);

// Funções para melodias
/**
 * @brief Toca uma melodia
 * @param melody Estrutura da melodia
 * @return esp_err_t Status da operação
 */
esp_err_t buzzer_play_melody(buzzer_melody_t* melody);

/**
 * @brief Para a reprodução de melodia atual
 * @return esp_err_t Status da operação
 */
esp_err_t buzzer_stop_melody(void);

/**
 * @brief Verifica se uma melodia está sendo reproduzida
 * @return bool true se está tocando, false caso contrário
 */
bool buzzer_is_playing(void);

// Funções avançadas
/**
 * @brief Faz um sweep de frequência (sirene)
 * @param start_freq_hz Frequência inicial
 * @param end_freq_hz Frequência final
 * @param duration_ms Duração total do sweep
 * @param steps Número de passos no sweep
 * @return esp_err_t Status da operação
 */
esp_err_t buzzer_frequency_sweep(uint32_t start_freq_hz, uint32_t end_freq_hz, 
                                uint32_t duration_ms, uint16_t steps);

/**
 * @brief Toca uma sequência de frequências customizada
 * @param frequencies Array de frequências
 * @param durations Array de durações correspondentes
 * @param count Número de elementos nos arrays
 * @return esp_err_t Status da operação
 */
esp_err_t buzzer_play_sequence(uint32_t* frequencies, uint32_t* durations, size_t count);

// Funções auxiliares para criação de melodias
/**
 * @brief Cria uma melodia simples a partir de arrays
 * @param notes Array de notas
 * @param durations Array de durações
 * @param count Número de notas
 * @param tempo_bpm Tempo em BPM
 * @return buzzer_melody_t* Ponteiro para a melodia ou NULL em caso de erro
 */
buzzer_melody_t* buzzer_create_melody(buzzer_note_t* notes, uint32_t* durations, 
                                     size_t count, uint32_t tempo_bpm);

/**
 * @brief Libera memória de uma melodia criada
 * @param melody Ponteiro para a melodia
 */
void buzzer_free_melody(buzzer_melody_t* melody);

// Melodias pré-definidas
extern const buzzer_melody_t buzzer_melody_mario_coin;
extern const buzzer_melody_t buzzer_melody_nokia_ringtone;
extern const buzzer_melody_t buzzer_melody_twinkle_star;
extern const buzzer_melody_t buzzer_melody_happy_birthday;

#ifdef __cplusplus
}
#endif

#endif // BUZZER_H