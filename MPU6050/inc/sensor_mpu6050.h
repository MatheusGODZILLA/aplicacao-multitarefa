#ifndef SENSOR_MPU6050_H
#define SENSOR_MPU6050_H

#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// Configurações do sensor
#define MPU6050_ADRR 0x68
#define I2C_PORT i2c0
#define I2C_SDA 0
#define I2C_SCL 1
#define M_PI 3.14159265358979323846


// Funções
void init_mpu6050();
void read_acceleration(int16_t *ax, int16_t *ay, int16_t *az);
float calculate_slope(int16_t ax, int16_t ay, int16_t az);

#endif