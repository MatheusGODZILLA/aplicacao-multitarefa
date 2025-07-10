#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "inc/ssd1306.h"
#include "inc/sensor_aht10.h"

#define BUZZER_PIN 21
#define BUZZER_FREQUENCY 100

const uint OLED_I2C_SDA = 14;
const uint OLED_I2C_SCL = 15;

struct render_area frame_area;

// === Inicialização do OLED ===
void setup_oled() {
    i2c_init(i2c1, 400 * 1000);  // 400kHz
    gpio_set_function(OLED_I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(OLED_I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(OLED_I2C_SDA);
    gpio_pull_up(OLED_I2C_SCL);

    ssd1306_init();

    memset(&frame_area, 0, sizeof(frame_area));
    frame_area.start_column = 0;
    frame_area.end_column = ssd1306_width - 1;
    frame_area.start_page = 0;
    frame_area.end_page = ssd1306_n_pages - 1;

    calculate_render_area_buffer_length(&frame_area);

    // Limpa display
    uint8_t ssd[ssd1306_buffer_length];
    memset(ssd, 0, ssd1306_buffer_length);
    render_on_display(ssd, &frame_area);
}

// === Exibir texto no OLED ===
void display_text(char *lines[], uint line_count) {
    uint8_t ssd[ssd1306_buffer_length];
    memset(ssd, 0, ssd1306_buffer_length);

    int y = (ssd1306_n_pages * 8 - (line_count * 10)) / 2;
    for (uint i = 0; i < line_count; i++) {
        ssd1306_draw_string(ssd, 10, y, lines[i]);
        y += 10;
    }

    render_on_display(ssd, &frame_area);
}

// === Inicializa o buzzer ===
void pwm_init_buzzer(uint pin) {
    gpio_set_function(pin, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(pin);

    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, clock_get_hz(clk_sys) / (BUZZER_FREQUENCY * 4096));
    pwm_init(slice_num, &config, true);

    pwm_set_gpio_level(pin, 0);
}

// === Beep ===
void beep(uint pin, uint duration_ms) {
    uint slice_num = pwm_gpio_to_slice_num(pin);
    pwm_set_gpio_level(pin, 2048);  // 50% duty
    sleep_ms(duration_ms);
    pwm_set_gpio_level(pin, 0);
    sleep_ms(100);
}

// === Exibir dados e alerta ===
void mostrar_dados(float temp, float umid) {
    char linha1[32];
    char linha2[32];
    char *linhas[3];

    snprintf(linha1, sizeof(linha1), "Temp: %.1f C", temp);
    snprintf(linha2, sizeof(linha2), "Umid: %.1f %%", umid);

    linhas[0] = linha1;
    linhas[1] = linha2;

    if (temp < 20.0 || umid > 70.0) {
        linhas[2] = "!! ALERTA !!";
        display_text(linhas, 3);
        beep(BUZZER_PIN, 500);
    } else {
        display_text(linhas, 2);
    }
}

int main() {
    stdio_init_all();
    sleep_ms(3000);

    setup_oled();
    pwm_init_buzzer(BUZZER_PIN);

    // Inicializa I2C0 para o sensor AHT10 (GPIO 0 e 1)
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    if (!aht10_init()) {
        char *erro[] = {"Falha init AHT10"};
        display_text(erro, 1);
        beep(BUZZER_PIN, 200);
        return 1;
    }

    float temp, umid;

    while (true) {
        if (aht10_read(&temp, &umid)) {
            printf("Temperatura: %.2f °C | Umidade: %.2f %%\n", temp, umid);
            mostrar_dados(temp, umid);
        } else {
            printf("Erro na leitura do AHT10\n");
            char *erro[] = {"Erro leitura AHT10"};
            display_text(erro, 1);
            beep(BUZZER_PIN, 100);
        }
        sleep_ms(2000);
    }

    return 0;
}
