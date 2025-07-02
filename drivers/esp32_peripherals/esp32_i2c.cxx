#include "esp32_peripherals/esp32_peripherals.hpp"


using namespace ESP32::I2C;

I2C_Master::I2C_Master() = default;

I2C_Master::~I2C_Master()
{
    shutdown();
}


bool I2C_Master::setup(const char* devPath, struct i2c_config_s* config)
{
    shutdown();  // Close if already open

    if (config == nullptr)
    {
        perror("i2c config");
        return false;
    }

    int fd = open(devPath, O_RDWR);
    if (fd < 0)
    {
        perror("open");
        return false;
    }

    

    _fd = fd;
    _config = config;
    
    return true;
}


bool I2C_Master::writeRegister(uint8_t reg, uint8_t value)
{
    uint8_t buffer[2] = { reg, value };

    struct i2c_msg_s msg;
    msg.addr = _config->address;
    msg.flags = 0;
    msg.buffer = buffer;
    msg.length = sizeof(buffer);
    msg.frequency = _config->frequency;

    struct i2c_transfer_s xfer = { .msgv = &msg, .msgc = 1 };

    int result = ioctl(_fd, I2CIOC_TRANSFER, (unsigned long)&xfer);
    if (result < 0) {
        perror("I2C writeRegister");
    }
    return result == 0;
}

bool I2C_Master::readRegister(uint8_t reg, uint8_t* buffer, int len)
{
    struct i2c_msg_s msgs[2];

    msgs[0].addr = _config->address;
    msgs[0].flags = 0;
    msgs[0].buffer = &reg;
    msgs[0].length = 1;
    msgs[0].frequency = _config->frequency;

    msgs[1].addr = _config->address;
    msgs[1].flags = I2C_M_READ;
    msgs[1].buffer = buffer;
    msgs[1].length = len;
    msgs[1].frequency = _config->frequency;

    struct i2c_transfer_s xfer = { .msgv = msgs, .msgc = 2 };

    int result = ioctl(_fd, I2CIOC_TRANSFER, (unsigned long)&xfer);
    if (result < 0) {
        perror("I2C readRegister");
    }
    return result == 0;
}


bool I2C_Master::shutdown()
{
    if (_fd >= 0)
    {
        close(_fd);
        _fd = -1;
        return true;
    }
    return false;
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
    

    

