#include "esp32_peripherals.hpp"

using namespace ESP32::SPI;


SPI_Slave::SPI_Slave()
    : _devicePath(nullptr), _fd(-1), _timeoutSec(10), _isBlocking(true)
{
}

SPI_Slave::~SPI_Slave()
{
    shutdown();
}

bool SPI_Slave::setup(const char* devicePath, int timeoutSec, bool read_blocking)
{
    _devicePath = devicePath;
    _timeoutSec = timeoutSec;
    _isBlocking = read_blocking;


    _fd = open(_devicePath, O_RDWR);
    if (_fd < 0)
    {
        perror("open");
        return false;
    }

    return setBlocking(read_blocking);
}

void SPI_Slave::shutdown()
{
    if (_fd >= 0)
    {
        close(_fd);
        _fd = -1;
    }
}

bool SPI_Slave::setBlocking(bool enable)
{
    int flags = fcntl(_fd, F_GETFL, 0);
    if (flags == -1)
    {
        perror("fcntl get");
        return false;
    }

    if (enable)
        flags &= ~O_NONBLOCK;
    else
        flags |= O_NONBLOCK;

    if (fcntl(_fd, F_SETFL, flags) == -1)
    {
        perror("fcntl set");
        return false;
    }

    return true;
}

bool SPI_Slave::transmit(const uint8_t* data, size_t length)
{
    ssize_t bytes = write(_fd, data, length);
    if (bytes < 0)
    {
        perror("write");
        return false;
    }
    if ((size_t)bytes != length)
    {
        fprintf(stderr, "Incomplete write: %zd of %zu\n", bytes, length);
        return false;
    }
    return true;
}

bool SPI_Slave::waitForRead()
{
    fd_set rfds;
    struct timeval timeout;

    FD_ZERO(&rfds);
    FD_SET(_fd, &rfds);

    timeout.tv_sec = _timeoutSec;
    timeout.tv_usec = 0;

    int ret = select(_fd + 1, &rfds, nullptr, nullptr, &timeout);
    if (ret < 0)
    {
        perror("select");
        return false;
    }
    else if (ret == 0)
    {
        fprintf(stderr, "Timeout after %d seconds\n", _timeoutSec);
        return false;
    }

    return true;
}

bool SPI_Slave::receive(uint8_t* outBuffer, size_t maxLength, size_t& bytesReceived)
{
    if (!waitForRead())
        return false;

    ssize_t bytes = read(_fd, outBuffer, maxLength);
    if (bytes < 0)
    {
        perror("read");
        return false;
    }

    bytesReceived = static_cast<size_t>(bytes);
    return true;
}

