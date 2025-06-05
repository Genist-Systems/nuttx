#pragma once
extern "C"
{
    #include <sys/ioctl.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include <errno.h>
    #include <cerrno>
    #include <cstdio>


    #include <nuttx/ioexpander/gpio.h>
    #include <nuttx/timers/pwm.h>
}

#include <string>

namespace ESP32 {
    namespace GPIO
    {
        enum class PinType {
            Input                         = 0,  // GPIO_INPUT_PIN
            InputPullUp                   = 1,  // GPIO_INPUT_PIN_PULLUP
            InputPullDown                 = 2,  // GPIO_INPUT_PIN_PULLDOWN
            Output                        = 3,  // GPIO_OUTPUT_PIN
            OutputOpenDrain               = 4,  // GPIO_OUTPUT_PIN_OPENDRAIN

            Interrupt                     = 5,  // GPIO_INTERRUPT_PIN
            InterruptHigh                 = 6,  // GPIO_INTERRUPT_HIGH_PIN
            InterruptLow                  = 7,  // GPIO_INTERRUPT_LOW_PIN
            InterruptRising               = 8,  // GPIO_INTERRUPT_RISING_PIN
            InterruptFalling              = 9,  // GPIO_INTERRUPT_FALLING_PIN
            InterruptBoth                 = 10, // GPIO_INTERRUPT_BOTH_PIN

            InterruptWakeup               = 11, // GPIO_INTERRUPT_PIN_WAKEUP
            InterruptHighWakeup           = 12, // GPIO_INTERRUPT_HIGH_PIN_WAKEUP
            InterruptLowWakeup            = 13, // GPIO_INTERRUPT_LOW_PIN_WAKEUP
            InterruptRisingWakeup         = 14, // GPIO_INTERRUPT_RISING_PIN_WAKEUP
            InterruptFallingWakeup        = 15, // GPIO_INTERRUPT_FALLING_PIN_WAKEUP
            InterruptBothWakeup           = 16  // GPIO_INTERRUPT_BOTH_PIN_WAKEUP
        };

        bool setPinType(const std::string& devPath, PinType type);
        bool writePin(const std::string& devPath, bool value);
        bool readPin(const std::string& devPath, bool& value);
    }

    namespace PWM
    {
        struct Config {
            uint32_t frequency;
            uint8_t dutyPercent; 
            bool inverted = False;
        };

        bool setup(const std::string& devPath, const Config& config);
        bool start(const std::string& devPath);
        bool stop(const std::string& devPath);
    }

    namespace I2C
    {

    }

    namespace SPI
    {

    }
}
