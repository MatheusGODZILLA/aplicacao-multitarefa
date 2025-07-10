#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "inc/acceleration_sensor.h"
#include "inc/ssd1306.h"

// === OLED config ===
#define OLED_SDA 14
#define OLED_SCL 15

struct render_area frame_area;

// === MPU6050 (funções unidas aqui) ===
void init_mpu6050() {
    uint8_t reset[2] = {0x6B, 0x00};
    i2c_write_blocking(I2C_PORT, MPU6050_ADRR, reset, 2, false);
}

void read_acceleration(int16_t *ax, int16_t *ay, int16_t *az) {
    uint8_t buffer[6];
    i2c_write_blocking(I2C_PORT, MPU6050_ADRR, (uint8_t[]){0x3B}, 1, true);
    i2c_read_blocking(I2C_PORT, MPU6050_ADRR, buffer, 6, false);

    *ax = (buffer[0] << 8) | buffer[1];
    *ay = (buffer[2] << 8) | buffer[3];
    *az = (buffer[4] << 8) | buffer[5];
}

float calculate_slope(int16_t ax, int16_t ay, int16_t az) {
    float ax_g = ax / 16384.0;
    float ay_g = ay / 16384.0;
    float az_g = az / 16384.0;
    float slope = atan2(ax_g, sqrt(ay_g * ay_g + az_g * az_g)) * (180.0 / M_PI);
    return slope;
}

// === OLED setup ===
void setup_oled() {
    i2c_init(i2c1, 400 * 1000); // I2C1 para o OLED
    gpio_set_function(OLED_SDA, GPIO_FUNC_I2C);
    gpio_set_function(OLED_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(OLED_SDA);
    gpio_pull_up(OLED_SCL);

    ssd1306_init();

    memset(&frame_area, 0, sizeof(frame_area));
    frame_area.start_column = 0;
    frame_area.end_column = ssd1306_width - 1;
    frame_area.start_page = 0;
    frame_area.end_page = ssd1306_n_pages - 1;

    calculate_render_area_buffer_length(&frame_area);
}

// === Exibição no OLED ===
void display_oled(float slope) {
    char buffer1[32];
    char buffer2[32];

    sprintf(buffer1, "Inclinacao: %.2f°", slope);

    if (slope > 30.0) {
        sprintf(buffer2, "ALERTA: > 30 graus!");
    } else {
        sprintf(buffer2, "OK");
    }

    char *lines[] = { buffer1, buffer2 };

    uint8_t ssd[ssd1306_buffer_length];
    memset(ssd, 0, ssd1306_buffer_length);

    int y = 10;
    for (uint i = 0; i < 2; i++) {
        ssd1306_draw_string(ssd, 10, y, lines[i]);
        y += 12;
    }

    render_on_display(ssd, &frame_area);
}

// === Main ===
int main() {
    stdio_init_all();
    printf("Sistema Iniciado...\n");

    // Inicializa MPU6050 (I2C0)
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    sleep_ms(100);
    init_mpu6050();

    // Inicializa OLED (I2C1)
    setup_oled();

    int16_t ax, ay, az;

    while (true) {
        read_acceleration(&ax, &ay, &az);
        float slope = calculate_slope(ax, ay, az);

        printf("Inclinação: %.2f°\n", slope);

        display_oled(slope);

        sleep_ms(1000);
    }
}
