idf_component_register(
    SRCS "src/snake.c"
    INCLUDE_DIRS "inc"
    REQUIRES driver mpu6050 ssd1306 sd_card buzzer
)
