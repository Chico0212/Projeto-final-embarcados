idf_component_register(
    SRCS "src/tilt_maze.c"
    INCLUDE_DIRS "inc"
    REQUIRES driver mpu6050 ssd1306
)
