#pragma once
extern "C"
{
    #include <sys/ioctl.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include <errno.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <nuttx/ioexpander/gpio.h>
    #include <nuttx/timers/pwm.h>
    #include <nuttx/i2c/i2c_master.h>
    #include <nuttx/i2c/i2c_slave.h>

    #include <sys/select.h>

}

// #include <string>
#include <cstring>
#include <cstdio>
#include <cerrno>
// #include <vector>
// #include <cstdint>
// #include <cstddef>


namespace ESP32::GPIO
{

    // STILL HAVE TO FIGURE OUT HOW TO DO INTERRUPTS
    #ifdef CONFIG_DEV_GPIO
    enum class PinStatus
    {
        GPIO_LOW = 0,
        GPIO_HIGH = 1
    };

    class GPIO
    {
    public:
        GPIO();
        ~GPIO();

        bool setPinType(const char* devPath, enum gpio_pintype_e type);
        bool writePin(PinStatus value);
        bool readPin(PinStatus& value);

    private:
        const char* _devPath = nullptr;
        gpio_pintype_e _pinType = GPIO_OUTPUT_PIN;
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

        bool setup(const char* devPath, struct pwm_info_s* info);
        bool editFreq(uint32_t newFreq);
        bool editDuty(uint8_t newDutyPercent);
        bool start(void);
        bool stop(void);

    private:
        int _fd = -1;
        struct pwm_info_s* _config = nullptr;
    };
}


namespace ESP32::I2C
{
    class I2C_Master
    {
        public:

            I2C_Master();
            ~I2C_Master();

            bool setup(const char* devPath, struct i2c_config_s* config);
            bool writeRegister(uint8_t reg, uint8_t value);
            bool readRegister(uint8_t reg, uint8_t* buffer, int len);
            bool shutdown();

        private:
            int _fd = -1;
            struct i2c_config_s* _config = nullptr;
    };
}



namespace ESP32::SPI
{

    class SPI_Slave
    {
        public:
            SPI_Slave();
            ~SPI_Slave();

            bool setup(const char* devicePath, int timeoutSec, bool read_blocking);
            void shutdown();

            bool transmit(const uint8_t* data, size_t length);
            bool receive(uint8_t* outBuffer, size_t maxLength, size_t& bytesReceived);

        private:
            const char* _devicePath;
            int _fd;
            int _timeoutSec;
            bool _isBlocking;

            bool setBlocking(bool enable);
            bool waitForRead();
    };


}

namespace ESP32::WiFi
{


class UDPServer
{
public:
    UDPServer(uint16_t port, size_t bufSize);  // Changed from uint16_t
    ~UDPServer();
    void recvLoop();

private:
    int _sockfd;
    socklen_t _addrLen;
    size_t _bufSize;  // Also updated from uint16_t
    struct sockaddr_in6 _server;
    struct sockaddr_in6 _client;

    int check_buffer(unsigned char *buf);  // Declare this function
};


class UDPClient {
public:
    UDPClient(const char* server_ip, uint16_t server_port, uint16_t local_port, size_t bufSize);  // Changed to size_t
    ~UDPClient();

    void sendLoop();

private:
    int _sockfd;
    struct sockaddr_in6 _serverAddr;
    struct sockaddr_in6 _addr;
    socklen_t _addrLen;
    size_t _bufSize;

    int createSocket(uint16_t local_port);
    void fillBuffer(unsigned char *buf, int offset);
};


}








