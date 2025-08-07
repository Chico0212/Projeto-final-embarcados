#include "buzzer.h"

void buzzer_pwm_init() {
  ledc_timer_config_t timer_conf = {
    .speed_mode       = LEDC_LOW_SPEED_MODE,
    .timer_num        = LEDC_TIMER_0,
    .duty_resolution  = LEDC_TIMER_10_BIT,
    .freq_hz          = 2000, 
    .clk_cfg          = LEDC_AUTO_CLK
  };
  ledc_timer_config(&timer_conf);

  ledc_channel_config_t channel_conf = {
    .gpio_num       = BUZZER_GPIO,
    .speed_mode     = LEDC_LOW_SPEED_MODE,
    .channel        = LEDC_CHANNEL_0,
    .timer_sel      = LEDC_TIMER_0,
    .duty           = 0,
    .hpoint         = 0
  };
  ledc_channel_config(&channel_conf);
}

void buzzer_play_note(uint32_t frequency, uint32_t duration_ms) {
  if (frequency > 0) {
    // Configura a frequência da nota
    ledc_set_freq(LEDC_LOW_SPEED_MODE, LEDC_TIMER_0, frequency);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 512);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
  }
  
  vTaskDelay(duration_ms / portTICK_PERIOD_MS);
  
  // Silêncio entre notas
  ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0);
  ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
  vTaskDelay(50 / portTICK_PERIOD_MS); // Pequena pausa entre notas
}

void buzzer_victory_melody() {
  // Melodia de vitória - sequência ascendente e alegre
  // Baseada em uma progressão triunfante
  
  buzzer_play_note(NOTE_C4, EIGHTH_NOTE);
  buzzer_play_note(NOTE_E4, EIGHTH_NOTE);
  buzzer_play_note(NOTE_G4, EIGHTH_NOTE);
  buzzer_play_note(NOTE_C5, QUARTER_NOTE);
  
  vTaskDelay(100 / portTICK_PERIOD_MS);
  
  buzzer_play_note(NOTE_G4, EIGHTH_NOTE);
  buzzer_play_note(NOTE_C5, QUARTER_NOTE);
  
  vTaskDelay(100 / portTICK_PERIOD_MS);
  
  buzzer_play_note(NOTE_E5, EIGHTH_NOTE);
  buzzer_play_note(NOTE_C5, EIGHTH_NOTE);
  buzzer_play_note(NOTE_G4, EIGHTH_NOTE);
  buzzer_play_note(NOTE_C5, QUARTER_NOTE + EIGHTH_NOTE);
}

void buzzer_defeat_melody() {
  // Melodia de derrota - sequência descendente e melancólica
  // Tom mais grave e ritmo mais lento
  
  buzzer_play_note(NOTE_G4, QUARTER_NOTE);
  buzzer_play_note(NOTE_F4, QUARTER_NOTE);
  buzzer_play_note(NOTE_E4, QUARTER_NOTE);
  buzzer_play_note(NOTE_D4, QUARTER_NOTE);
  
  vTaskDelay(200 / portTICK_PERIOD_MS);
  
  buzzer_play_note(NOTE_C4, QUARTER_NOTE + EIGHTH_NOTE);
  
  vTaskDelay(300 / portTICK_PERIOD_MS);
  
  buzzer_play_note(NOTE_C4, SIXTEENTH_NOTE);
  buzzer_play_note(NOTE_C4, SIXTEENTH_NOTE);
  buzzer_play_note(NOTE_C4, QUARTER_NOTE + QUARTER_NOTE);
}

void buzzer_on() {
  ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 512); 
  ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
}

void buzzer_off() {
  ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0);
  ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
}

void game_win() {
  printf("Parabéns! Você ganhou!\n");
  buzzer_victory_melody();
}

void game_lose() {
  printf("Que pena! Você perdeu!\n");
  buzzer_defeat_melody();
}