#include "inc/sensor_bh1750.h"

void bh1750_init() {
    i2c_init(I2C_PORT, 100 * 1000);  // 100kHz
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
}

void bh1750_start_measurement() {
    uint8_t cmd = BH1750_CMD_START;
    i2c_write_blocking(I2C_PORT, BH1750_ADDR, &cmd, 1, false);
}

uint16_t bh1750_read_lux() {
    uint8_t data[2];
    int read = i2c_read_blocking(I2C_PORT, BH1750_ADDR, data, 2, false);
    if (read != 2) {
        printf("Erro ao ler dados do sensor BH1750\n");
        return 0xFFFF;
    }
    return (data[0] << 8) | data[1];
}
