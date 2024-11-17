#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#include "nunchuck.hpp"

// I2C defines
#define I2C_PORT i2c0
#define I2C_SDA 4
#define I2C_SCL 5

int main()
{
    stdio_init_all();

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 100 * 1000);

    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    Nunchuck nunchuck;

    nunchuck.init();

    while (true) {
        if (nunchuck.read()) {
            nunchuck.print();
        }

        sleep_ms(100);
    }

    return 0;
}
