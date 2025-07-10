#include "inc/sensor_aht10.h"
#include "pico/time.h"

// Aguarda o sensor ficar pronto (bit 7 do status = 0)
static bool aht10_wait_ready() {
    uint8_t status;
    absolute_time_t timeout = make_timeout_time_ms(100);

    do {
        if (i2c_read_blocking(I2C_PORT, AHT10_ADDR, &status, 1, false) == 1) {
            if (!(status & 0x80)) return true;
        }
    } while (!time_reached(timeout));

    return false;
}

bool aht10_init() {
    uint8_t init_cmd[] = { AHT10_CMD_INIT, 0x08, 0x00 };
    int ret = i2c_write_blocking(I2C_PORT, AHT10_ADDR, init_cmd, sizeof(init_cmd), false);
    sleep_ms(20);  // Tempo mínimo recomendado após init
    return ret >= 0;
}

bool aht10_read(float *temperature, float *humidity) {
    uint8_t cmd[] = { AHT10_CMD_TRIGGER, 0x33, 0x00 };
    if (i2c_write_blocking(I2C_PORT, AHT10_ADDR, cmd, sizeof(cmd), false) < 0)
        return false;

    sleep_ms(80);  // Tempo necessário para conversão (~75ms)

    uint8_t data[6];
    if (i2c_read_blocking(I2C_PORT, AHT10_ADDR, data, 6, false) < 0)
        return false;

    if (data[0] & 0x80)  // bit de status indica ocupado
        return false;

    uint32_t hum_raw = ((uint32_t)data[1] << 12) |
                       ((uint32_t)data[2] << 4) |
                       ((uint32_t)data[3] >> 4);

    uint32_t temp_raw = ((uint32_t)(data[3] & 0x0F) << 16) |
                        ((uint32_t)data[4] << 8) |
                        ((uint32_t)data[5]);

    *humidity = ((float)hum_raw * 100.0f) / 1048576.0f;
    *temperature = ((float)temp_raw * 200.0f) / 1048576.0f - 50.0f;

    return true;
}
