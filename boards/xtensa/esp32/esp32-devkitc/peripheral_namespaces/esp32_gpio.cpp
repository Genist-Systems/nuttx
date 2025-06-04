#include "esp32_peripherals.hpp"

namespace ESP32_Devkitc::GPIO
{
    enum class gpio_direction {
        Input,
        Output,
        InputOutput
    };
    
    enum class gpio_pull {
        None,
        Up,
        Down
    };
    
    enum class gpio_strength {
        Weak = 0,
        Medium = 1,
        Default = 2,
        Strong = 2,
        Max = 3
    };

    enum class gpio_function {
        GPIO = 0,
        Function1 = 1,
        Function2 = 2,
        Function3 = 3,
        Function4 = 4,
        Function5 = 5,
        Function6 = 6 
    };

    enum class gpio_state {
        GPIO_PIN_LOW = 0,
        GPIO_PIN_HIGH = 1
    };

    struct gpio_config {
        gpio_direction dir;
        gpio_pull pull = gpio_pull::None;
        bool open_drain = false;
        gpio_strength drive = gpio_strength::Default;
        gpio_function func = gpio_function::GPIO;
    };
    
    int setup(int pin, const gpio_config& config) {
        gpio_pinattr_t attr = 0;
        
        // Direction
        if (config.dir == gpio_direction::Input || config.dir == gpio_direction::InputOutput) {
            attr |= INPUT;
        }
        if (config.dir == gpio_direction::Output || config.dir == gpio_direction::InputOutput) {
            attr |= OUTPUT;
        }
        
        // Pull resistors
        if (config.pull == gpio_pull::Up) {
            attr |= PULLUP;
        } else if (config.pull == gpio_pull::Down) {
            attr |= PULLDOWN;
        }
        
        // Open drain
        if (config.open_drain) {
            attr |= OPEN_DRAIN;
        }
        
        // Drive strength (clear + set)
        attr &= ~(0b11 << DRIVE_SHIFT);
        attr |= ((static_cast<uint32_t>(config.drive) + 1) << DRIVE_SHIFT);

        // Function (clear + set)
        attr &= ~(0b1111 << FUNCTION_SHIFT);
        attr |= ((static_cast<uint32_t>(config.func) + 1) << FUNCTION_SHIFT);
        
        return esp32_configgpio(pin, attr);
    }

    void write(int pin, gpio_state value) {
        esp32_gpiowrite(pin, static_cast<bool>(value));
    }

    gpio_state read(int pin) {
        return static_cast<gpio_state>(esp32_gpioread(pin));
    }
}
