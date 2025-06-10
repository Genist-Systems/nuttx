#pragma once
extern "C"
{
    #include <sys/ioctl.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include <errno.h>


    #include <nuttx/ioexpander/gpio.h>
    #include <nuttx/timers/pwm.h>
    #include <nuttx/i2c/i2c_master.h>
    #include <nuttx/i2c/i2c_slave.h>

    #include <sys/select.h>

}

#include <string>
#include <cstring>
#include <cstdio>
#include <cerrno>

namespace ESP32::GPIO
{

    #ifdef CONFIG_DEV_GPIO
    enum class PinStatus 
    {
        GPIO_LOW, 
        GPIO_HIGH
    };
    
    class GPIO
    {
        public:
            GPIO();
            ~GPIO();

            bool setPinType(const char* devPath, gpio_pintype_e type);
            bool writePin(PinStatus value);
            bool readPin(PinStatus& value);
        private:
            const char* _devPath;
            gpio_pintype_e _pinType;
            int _fd = -1;
    };
    #endif
        
}
namespace ESP32::PWM
{
    class PWM
    {
    public:
        PWM();
        ~PWM();

        bool setup(const char* devPath, struct pwm_info_s info);
        bool editFreq(uint32_t newFreq);
        bool editDuty(uint8_t newDutyPercent);
        bool start(void);
        bool stop(void);

    private:
        int _fd = -1;
        struct pwm_info_s _config = {};
    };
}

namespace ESP32::I2C
{
    
}
/*
namespace ESP32::SPI
{
    class SPISlave
    {
        public:
            SPISlave(const std::string& devPath, int timeoutSeconds = 10);

            bool isAvailable();
            bool Transmit(const std::vector<uint8_t>& data);
            bool Receive(std::vector<uint8_t>& data, size_t maxLen);
            bool TransmitReceive(const std::vector<uint8_t>& tx, std::vector<uint8_t>& rx);

        private:
            std::string _devPath;
            int _timeout;

            bool openDevice(int& fd);
            void closeDevice(int fd);
            bool waitForReadReady(int fd);
    };

}
    */

