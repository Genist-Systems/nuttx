#include "esp32_peripherals.hpp"

namespace ESP32::I2C
{
    enum class Port : int
    {
        #if defined(CONFIG_ESP32_I2C0)
            I2C0 = 0,
        #endif
        #if defined(CONFIG_ESP32_I2C1)
            I2C1 = 1,
        #endif
    };
    
    namespace Master
    {
        


        
        struct Config
        {
            i2c_master_s* dev;
            uint32_t frequency;
            uint16_t address;
            uint8_t addressLength;
        };

        inline i2c_master_s* initialize(Port port)
        {
        #ifdef CONFIG_ESPRESSIF_I2C_PERIPH_MASTER_MODE
                return esp32_i2cbus_initialize(static_cast<int>(port));
        #else
                return nullptr;
        #endif
        }

        inline bool uninitialize(const Config& config)
        {
        #ifdef CONFIG_ESPRESSIF_I2C_PERIPH_MASTER_MODE
                return config.dev && esp32_i2cbus_uninitialize(config.dev) == 0;
        #else
                return false;
        #endif
        }

        inline i2c_config_s toLowLevel(const Config& config)
        {
            return i2c_config_s{
                .frequency = config.frequency,
                .address = config.address,
                .addrlen = config.addressLength
            };
        }

        inline bool setup(const Config& config)
        {
            return config.dev && I2C_SETUP(config.dev) == 0;
        }

        inline bool shutdown(const Config& config)
        {
            return config.dev && I2C_SHUTDOWN(config.dev) == 0;
        }

        #ifdef CONFIG_I2C_RESET
        inline bool reset(const Config& config)
        {
            return config.dev && I2C_RESET(config.dev) == 0;
        }
        #endif

        inline bool write(const Config& config, const std::vector<uint8_t>& data)
        {
            auto low = toLowLevel(config);
            return config.dev &&
                i2c_write(config.dev, &low, data.data(), data.size()) == 0;
        }

        inline bool read(const Config& config, std::vector<uint8_t>& data)
        {
            auto low = toLowLevel(config);
            return config.dev &&
                i2c_read(config.dev, &low, data.data(), data.size()) == 0;
        }

        inline bool writeRead(const Config& config,
                            const std::vector<uint8_t>& wdata,
                            std::vector<uint8_t>& rdata)
        {
            auto low = toLowLevel(config);
            return config.dev &&
                i2c_writeread(config.dev,
                                &low,
                                wdata.data(), wdata.size(),
                                rdata.data(), rdata.size()) == 0;
        }

        inline bool registerDevice(const Config& config, int bus)
        {
        #ifdef CONFIG_I2C_DRIVER
                return config.dev && i2c_register(config.dev, bus) == 0;
        #else
                (void)config;
                (void)bus;
                return false;
        #endif
        }
    }
    
    #ifdef CONFIG_I2C_SLAVE
    namespace Slave
    {
        using Callback = i2c_slave_callback_t;
    
        struct Config
        {
            i2c_slave_s* dev;
            uint16_t ownAddress;
            uint8_t addressLength;  // 7 or 10
        };

        inline bool setAddress(const Config& config)
        {
            return config.dev &&
                I2CS_SETOWNADDRESS(config.dev, config.ownAddress, config.addressLength) == 0;
        }

        inline bool setup(const Config& config)
        {
            return config.dev && I2CS_SETUP(config.dev) == 0;
        }

        inline bool shutdown(const Config& config)
        {
            return config.dev && I2CS_SHUTDOWN(config.dev) == 0;
        }

        inline bool write(const Config& config, const uint8_t* data, size_t len)
        {
            return config.dev && I2CS_WRITE(config.dev, data, len) == 0;
        }

        inline bool read(const Config& config, uint8_t* buffer, size_t len)
        {
            return config.dev && I2CS_READ(config.dev, buffer, len) == 0;
        }

        inline bool registerCallback(const Config& config, Callback* cb, void* arg)
        {
            return config.dev && I2CS_REGISTERCALLBACK(config.dev, cb, arg) == 0;
        }

        #ifdef CONFIG_I2C_SLAVE_DRIVER
        inline bool registerDevice(const Config& config, int bus)
        {
            return config.dev && i2c_slave_register(config.dev, bus, config.ownAddress, config.addressLength) == 0;
        }
        #endif
    }
    #endif
    

    
}
