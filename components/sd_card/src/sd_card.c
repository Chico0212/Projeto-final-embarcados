#include "sd_card.h"

const char *SD_CARD_TAG = "sd_card";

void sd_card_init()
{
    esp_vfs_fat_sdmmc_mount_config_t mnt_config = {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024};

    sdmmc_card_t *sd_card;

    sdmmc_host_t host = SDSPI_HOST_DEFAULT();

    spi_bus_config_t spi_io = {
        .mosi_io_num = MOSI,
        .miso_io_num = MISO,
        .sclk_io_num = SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4000};

    esp_err_t init_status = spi_bus_initialize(host.slot, &spi_io, SDSPI_DEFAULT_DMA);

    if (init_status != ESP_OK)
    {
        ESP_LOGE(SD_CARD_TAG, "Falha ao inicializar o spi");
        return;
    }

    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = CS;
    slot_config.host_id = host.slot;

    init_status = esp_vfs_fat_sdspi_mount(SD_MNT_POINT, &host, &slot_config, &mnt_config, &sd_card);

    if (init_status != ESP_OK)
    {
        if (init_status == ESP_FAIL)
        {
            ESP_LOGE(SD_CARD_TAG, "Falha ao montar o sistema de arquivos");
        }
        else
        {
            ESP_LOGE(SD_CARD_TAG, "Falha ao inicializar o sd card");
            return;
        }
    }

    sdmmc_card_print_info(stdout, sd_card);
}

void write_file(char *path, char *data)
{
    char *EOL = "\n";

    FILE *file = fopen(path, "w+a");
    if (file == NULL)
    {
        ESP_LOGE(SD_CARD_TAG, "Falha ao criar o arquivo");
        return;
    }

    fprintf(file, data);
    fprintf(file, EOL);
    fclose(file);
}

void read_file(char *path, char *out_content, int size)
{
    FILE *file = fopen(path, "r");
    if (file == NULL)
    {
        printf("Falha o ler o arquivo");
        return;
    }

    fgets(out_content, size, file);
    fclose(file);
}

int update_score(char *file_path, int new_score)
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
