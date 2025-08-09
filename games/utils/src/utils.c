#include "utils.h"

void show_game_over(const char *file_path, int score)
{
    char score_text[20];

    ssd1306_clear_buffer();
    ssd1306_draw_string(0, 20, "Game over");
    snprintf(score_text, sizeof(score_text), "Atual: %d", score);
    ssd1306_draw_string(0, 35, score_text);
    snprintf(score_text, sizeof(score_text), "Melhor: %d", read_score_from_file(file_path));
    ssd1306_draw_string(0, 50, score_text);
    ssd1306_update_display();
}

void game_win()
{
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

void game_lose()
{
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

void catch_sound()
{
    // Som rápido e satisfatório quando pega a bola
    // Sequência ascendente curta que dá sensação de "pegou!"

    buzzer_play_note(NOTE_E4, SIXTEENTH_NOTE);
    buzzer_play_note(NOTE_G4, SIXTEENTH_NOTE);
    buzzer_play_note(NOTE_E5, EIGHTH_NOTE);
}

void buzzer_mario_theme()
{
    // Primeira parte da melodia
    buzzer_play_note(NOTE_E5, EIGHTH_NOTE + SIXTEENTH_NOTE);
    buzzer_play_note(NOTE_E5, EIGHTH_NOTE + SIXTEENTH_NOTE);
    vTaskDelay(EIGHTH_NOTE / portTICK_PERIOD_MS);
    buzzer_play_note(NOTE_E5, EIGHTH_NOTE + SIXTEENTH_NOTE);
    vTaskDelay(EIGHTH_NOTE / portTICK_PERIOD_MS);
    buzzer_play_note(NOTE_C5, EIGHTH_NOTE + SIXTEENTH_NOTE);
    buzzer_play_note(NOTE_E5, EIGHTH_NOTE + SIXTEENTH_NOTE);
    buzzer_play_note(NOTE_G5, EIGHTH_NOTE + SIXTEENTH_NOTE);
    vTaskDelay(QUARTER_NOTE / portTICK_PERIOD_MS);

    // Segunda parte
    buzzer_play_note(NOTE_G4, EIGHTH_NOTE + SIXTEENTH_NOTE);
    vTaskDelay(QUARTER_NOTE / portTICK_PERIOD_MS);
}

void hit_ball()
{
    buzzer_play_note(NOTE_D5, SIXTEENTH_NOTE);
}

int update_score(const char *file_path, int new_score)
{
    const int size = 20;
    char data[size];

    read_file(file_path, data, size);

    int score = atoi(data);

    if (new_score > score)
    {
        snprintf(data, size, "%d", score);
        write_file(file_path, data);
        read_file(file_path, data, sizeof(data));
        return 1;
    }
    return 0;
}

int read_score_from_file(const char* file_path)
{
    const int size = 20;
    char data[size];
    read_file(file_path, data, size);

    return atoi(data);
}
