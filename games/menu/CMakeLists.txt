idf_component_register(
    SRCS "src/menu.c"
    INCLUDE_DIRS "inc"
    REQUIRES driver mpu6050 ssd1306 dodge snake paddle_pong tilt_maze buzzer
)
