#include <stdio.h>
#include "pico/stdlib.h"
#include "inc/sensor_bh1750.h"

int main() {
    stdio_init_all();
    sleep_ms(2000);  // Tempo para a conexão da USB

    printf("=== INICIANDO PROGRAMA ===\n");

    // Inicializa I2C e sensor
    bh1750_init();
    printf("I2C inicializado.\n");

    sleep_ms(100);
    bh1750_start_measurement();
    printf("Sensor BH1750 configurado.\n");

    while (true) {
        uint16_t lux = bh1750_read_lux();
        if (lux != 0xFFFF) {
            printf("Luminosidade: %u lux\n", lux);
        }
        sleep_ms(1000);
    }

    return 0;
}
