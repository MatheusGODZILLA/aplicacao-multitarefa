#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "inc/sensor_mpu6050.h"
#include "inc/ssd1306.h"

// === OLED config ===
#define OLED_SDA 14
#define OLED_SCL 15

struct render_area frame_area;

// === Inicializa o OLED ===
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

    snprintf(buffer1, sizeof(buffer1), "Inclinacao: %.2f°", slope);

    if (slope > 30.0) {
        snprintf(buffer2, sizeof(buffer2), "ALERTA: > 30 graus!");
    } else {
        snprintf(buffer2, sizeof(buffer2), "OK");
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
    sleep_ms(3000);
    printf("Sistema Iniciado...\n");

    // Inicializa I2C0 para MPU6050
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    sleep_ms(100);
    init_mpu6050();
    printf("MPU6050 inicializado\n");

    // Inicializa I2C1 para OLED
    setup_oled();

    int16_t ax, ay, az;

    while (true) {
        read_acceleration(&ax, &ay, &az);
        float slope = calculate_slope(ax, ay, az);

        // Serial
        printf("Acelerômetro: X=%d Y=%d Z=%d\n", ax, ay, az);
        printf("Inclinação (X): %.2f°\n", slope);

        // OLED
        display_oled(slope);

        sleep_ms(1000);
    }
}
