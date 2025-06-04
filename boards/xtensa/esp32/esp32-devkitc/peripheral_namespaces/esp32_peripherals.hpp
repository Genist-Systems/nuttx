#pragma once

extern "C" {
    #include <arch/esp32/esp32_gpio.h>
}


namespace ESP32_Devkitc {
    namespace GPIO
    {
        void setup(int pin);
        void write(int pin, int status);
        int gpio_read(int pin);
    }

    namespace PWM
    {

    }

    namespace I2C
    {

    }

    namespace SPI
    {

    }
}
