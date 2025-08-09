idf_component_register(
    SRCS "src/utils.c"
    INCLUDE_DIRS "inc"
    REQUIRES driver mpu6050 ssd1306 buzzer sd_card
)
