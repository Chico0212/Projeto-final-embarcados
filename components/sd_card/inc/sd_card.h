#ifndef SD_CARD
#define SD_CARD

#include <sys/stat.h>
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "esp_log.h"

#define SD_MNT_POINT "/files"
#define MISO         GPIO_NUM_2
#define MOSI         GPIO_NUM_15
#define SCLK         GPIO_NUM_14
#define CS           GPIO_NUM_13

void sd_card_init();
void write_file(char* path, char* content);
void read_file(char* path, char* out_content, int size);

#endif
