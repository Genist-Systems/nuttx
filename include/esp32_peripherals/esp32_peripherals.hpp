#pragma once
extern "C"
{
    
    #include <fcntl.h>
    #include <unistd.h>
    #include <errno.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <netutils/netlib.h>

    #include <nuttx/ioexpander/gpio.h>
    #include <nuttx/timers/pwm.h>
    #include <nuttx/i2c/i2c_master.h>
    #include <nuttx/i2c/i2c_slave.h>

    #include <sys/select.h>
    #include <sys/socket.h>
    #include <sys/ioctl.h>

    // #include "protocol.h"
    // #include "lorawan/uart_lorawan_layer.h"

    #include <wireless/wapi.h>
    


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

    #ifdef CONFIG_DEV_GPIO
    enum class PinStatus
    {
        GPIO_LOW = 0,
        GPIO_HIGH = 1
    };

    

    class GPIO {
        public:
            GPIO();
            ~GPIO();

            bool setPinType(const char* devPath, enum gpio_pintype_e type);
            bool writePin(PinStatus value);
            bool readPin(PinStatus& value);
            bool attachInterrupt(int signo, void (*user_callback)(void));
            bool detachInterrupt(int signo);

        private:
            static void (*_userCallback)(void); 
            static void _signal_handler(int signo);

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
            bool write(uint8_t* data, size_t length);
            bool writeRegister(uint8_t reg, uint8_t value);
            bool writeRegister16(uint8_t reg, uint16_t value);

            bool readRegister(uint8_t reg, uint8_t* buffer, size_t length);
            bool readRegister(uint8_t* reg, uint8_t* buffer, size_t length);

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

namespace ESP32::WiFi::UDP
{
    

class UDPServer
{
public:
    UDPServer(uint16_t port, const char* ifname, const char* ip);
    ~UDPServer();
    int receiveMessage(uint8_t* buffer, size_t bufSize);

private:
    int _sockfd;
    socklen_t _addrLen;
    struct sockaddr_in6 _server;
    struct sockaddr_in6 _client;

    bool configureInterfaceIPv6(const char* ifname, const char* ip);
};


class UDPClient {
public:
    UDPClient(uint16_t server_port, uint16_t local_port, const char* ifname, const char* server_ip);
    ~UDPClient();

    int sendMessage(const char* msg, size_t msgLen);

private:
    int _sockfd;
    struct sockaddr_in6 _serverAddr;
    struct sockaddr_in6 _addr;
    socklen_t _addrLen;

    int createSocket(uint16_t local_port);
};


}

namespace ESP32::WiFi::TCP
{
    struct TCP_Settings {
        uint16_t port = 5000;
        const char* server_ip = nullptr;
        const char* client_ip = nullptr;
        const char* ifname = nullptr;
        const char* ssid = nullptr;
        const char* password = nullptr;
    };

    
    class TCPServer {
        public:
            TCPServer(const TCP_Settings& settings);
            ~TCPServer();

            bool init();
            bool acceptClient();

            template<typename T>
            bool receiveMessage(T& data)
            {

                int received = read(_client_fd, &data, sizeof(T));
                if (received <= 0)
                {
                    if (received == 0)
                        printf("Client disconnected\n");
                    else
                        perror("read");

                    return false;
                }

                printf("Received %d bytes\n", received);
                return true;
            }
            
            template<typename T>
            bool sendMessage(const T& data)
            {

                int sent = send(_client_fd, &data, sizeof(T), 0);
                if (sent <= 0)
                {
                    perror("send");
                    return false;
                }

                printf("Sent %d bytes\n", sent);
                return true;
            }

            void closeAll();

        private:

            uint16_t _port;
            const char* _server_ip;

            const char* _ifname;

            const char* _ssid;
            const char* _password;

            int _server_fd;
            int _client_fd;

            struct sockaddr_in _server_addr;
            struct sockaddr_storage _client_addr; 

        
            bool configureWiFi();
    };

    class TCPClient {
        public:
            TCPClient(const TCP_Settings& settings);
            ~TCPClient();

            bool connectToServer();
            
            template<typename T>
            bool sendMessage(const T& data)
            {
            //   static_assert(std::is_trivially_copyable<T>::value, "Data must be trivially copyable");

            int sent = write(_sockfd, &data, sizeof(T));
            if (sent <= 0)
            {
                perror("write");
                return false;
            }
            return true;
            }

            template<typename T>
            bool receiveMessage(T& data)
            {
            //   static_assert(std::is_trivially_copyable<T>::value, "Data must be trivially copyable");

            int received = recv(_sockfd, &data, sizeof(T), 0);
            if (received <= 0)
            {
                perror("recv");
                return false;
            }
            return true;
            }



            void closeSocket();

        private:
            uint16_t _port;
            const char* _server_ip;
            const char* _client_ip;

            const char* _ifname;

            const char* _ssid;
            const char* _password;

            int _sockfd;
            struct sockaddr_in _server_addr;

            bool configureWiFi();
    };
}








