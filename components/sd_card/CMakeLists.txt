idf_component_register(
    SRCS "src/sd_card.c"
    INCLUDE_DIRS "inc"
    REQUIRES driver fatfs sdmmc unity
)
