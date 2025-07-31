idf_component_register(
    SRCS "main.c"
    INCLUDE_DIRS "."
    REQUIRES mpu6050 buzzer buttons mpu6050 sd_card ssd1306
)
