#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "pico/time.h"

#define I2C_PORT i2c0
#define SDA_PIN 4      // GPIO4 para SDA
#define SCL_PIN 5      // GPIO5 para SCL

// Endereço do BH1750
#define BH1750_ADDR 0x23
#define BH1750_CMD_START 0x10  // Medição contínua, alta resolução

// Envia comando ao BH1750
void bh1750_start_measurement() {
    uint8_t cmd = BH1750_CMD_START;
    i2c_write_blocking(I2C_PORT, BH1750_ADDR, &cmd, 1, false);
}

// Lê o valor da luminosidade do BH1750
uint16_t bh1750_read_lux() {
    uint8_t data[2];
    i2c_read_blocking(I2C_PORT, BH1750_ADDR, data, 2, false);
    return (data[0] << 8) | data[1];
}

int main() {
    stdio_init_all();  // Inicializa USB serial
    sleep_ms(3000);    // Aguarda 3 segundos para garantir a conexão USB

    printf("Iniciando leitura de luminosidade com BH1750...\n");

    // Inicializa I2C
    i2c_init(I2C_PORT, 100 * 1000);  // 100 kHz
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    // Configura sensor
    bh1750_start_measurement();

    while (true) {
        uint16_t lux = bh1750_read_lux();
        printf("Luminosidade: %u lux\n", lux);
        sleep_ms(1000);
    }

    return 0;
}
