#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include <string.h>
#include "inc/ssd1306.h" 

#define AHT10_ADDR 0x38

// === Configurações ===
const uint I2C_SDA = 14;
const uint I2C_SCL = 15;
const uint BUZZER_PIN = 21;  // GPIO do buzzer
#define BUZZER_FREQUENCY 100

struct render_area frame_area;

// === Inicialização do OLED ===
void setup_oled() {
    i2c_init(i2c1, 400 * 1000);  // 400kHz
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

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

// === Inicialização do AHT10 ===
void aht10_init() {
    uint8_t init_cmd[] = {0xBE, 0x08, 0x00};
    i2c_write_blocking(i2c1, AHT10_ADDR, init_cmd, 3, false);
    sleep_ms(20);
}

// === Leitura do AHT10 ===
bool aht10_read(float *temperature, float *humidity) {
    uint8_t trigger[] = {0xAC, 0x33, 0x00};
    uint8_t raw[6];

    i2c_write_blocking(i2c1, AHT10_ADDR, trigger, 3, false);
    sleep_ms(80);

    if (i2c_read_blocking(i2c1, AHT10_ADDR, raw, 6, false) != 6) {
        return false;
    }

    if ((raw[0] & 0x80) != 0) return false;

    uint32_t hum_raw = ((raw[1] << 12) | (raw[2] << 4) | (raw[3] >> 4));
    *humidity = (hum_raw * 100.0) / 1048576;

    uint32_t temp_raw = (((raw[3] & 0x0F) << 16) | (raw[4] << 8) | raw[5]);
    *temperature = (temp_raw * 200.0 / 1048576) - 50;

    return true;
}

// === Inicializa o buzzer (PWM) ===
void pwm_init_buzzer(uint pin) {
    gpio_set_function(pin, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(pin);

    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, clock_get_hz(clk_sys) / (BUZZER_FREQUENCY * 4096));
    pwm_init(slice_num, &config, true);

    pwm_set_gpio_level(pin, 0);  // Começa desligado
}

// === Beep do buzzer ===
void beep(uint pin, uint duration_ms) {
    uint slice_num = pwm_gpio_to_slice_num(pin);
    pwm_set_gpio_level(pin, 2048);  // 50% duty
    sleep_ms(duration_ms);
    pwm_set_gpio_level(pin, 0);     // Desliga
    sleep_ms(100);                  // Pausa
}

// === Exibir dados e alertar com buzzer ===
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
        beep(BUZZER_PIN, 500);  // Bipe por 500ms
    } else {
        display_text(linhas, 2);
    }
}

// === Função principal ===
int main() {
    stdio_init_all();
    setup_oled();
    aht10_init();
    pwm_init_buzzer(BUZZER_PIN);

    float temp, umid;

    while (true) {
        if (aht10_read(&temp, &umid)) {
            mostrar_dados(temp, umid);
        } else {
            char *erro[] = {"Erro leitura AHT10"};
            display_text(erro, 1);
            beep(BUZZER_PIN, 100);  // Aviso de erro
        }
        sleep_ms(2000);
    }

    return 0;
}
