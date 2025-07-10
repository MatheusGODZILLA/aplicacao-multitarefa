#ifndef SENSOR_BH1750_H
#define SENSOR_BH1750_H

#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include <stdio.h>

// Configurações do I2C e sensor
#define I2C_PORT i2c0
#define I2C_SDA 0
#define I2C_SCL 1
#define BH1750_ADDR 0x23
#define BH1750_CMD_START 0x10

// Inicializa a interface I2C
void bh1750_init();

// Envia comando de início de medição
void bh1750_start_measurement();

// Lê e retorna o valor da luminosidade (lux)
uint16_t bh1750_read_lux();

#endif