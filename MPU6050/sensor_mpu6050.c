#include "inc/sensor_mpu6050.h"

void init_mpu6050() {
    uint8_t reset[2] = {0x6B, 0x00};  // Desliga sleep mode
    i2c_write_blocking(I2C_PORT, MPU6050_ADRR, reset, 2, false);
}

void read_acceleration(int16_t *ax, int16_t *ay, int16_t *az) {
    uint8_t buffer[6];
    // Endereço de leitura dos dados de aceleração
    i2c_write_blocking(I2C_PORT, MPU6050_ADRR, (uint8_t[]){0x3B}, 1, true);
    i2c_read_blocking(I2C_PORT, MPU6050_ADRR, buffer, 6, false);

    *ax = (buffer[0] << 8) | buffer[1];
    *ay = (buffer[2] << 8) | buffer[3];
    *az = (buffer[4] << 8) | buffer[5];
}

float calculate_slope(int16_t ax, int16_t ay, int16_t az) {
    float ax_g = ax / 16384.0f;
    float ay_g = ay / 16384.0f;
    float az_g = az / 16384.0f;

    float slope = atan2(ax_g, sqrtf(ay_g * ay_g + az_g * az_g)) * (180.0f / M_PI);
    return slope;
}
