#include "esp32_peripherals.hpp"

using namespace ESP32::GPIO;

#ifdef CONFIG_DEV_GPIO

GPIO::GPIO() = default;

GPIO::~GPIO()
{
    if (_fd >= 0) {
        close(_fd);
    }
}

bool GPIO::setPinType(const char* devPath, gpio_pintype_e type)
{
    int fd = open(devPath, O_RDWR);
    if (fd < 0) {
        perror("gpio open");
        return false;
    }

    int ret = ioctl(fd, GPIOC_SETPINTYPE, static_cast<unsigned long>(type));
    if (ret < 0) {
        perror("gpio ioctl setpintype");
        close(fd);
        return false;
    }

    _fd = fd;
    _devPath = devPath;
    _pinType = type;
    return true;
}

bool GPIO::writePin(PinStatus value)
{

    int ret = ioctl(_fd, GPIOC_WRITE, static_cast<unsigned long>(value));
    if (ret < 0) {
        perror("gpio ioctl write");
    }

    return ret == 0;
}

bool GPIO::readPin(PinStatus& value)
{

    int val = 0;
    int ret = ioctl(_fd, GPIOC_READ, (unsigned long)((uintptr_t)&val));
    if (ret == 0) {
        value = static_cast<PinStatus>(val != 0);
    } else {
        perror("gpio ioctl read");
    }

    return ret == 0;
}

#endif

    



/*

int main()
{
    std::string pin = "/dev/gpio0";

    // Set as output
    if (!GPIO::setPinType(pin, GPIO::PinType::Output)) {
        std::cerr << "Failed to set pin type\n";
        return 1;
    }

    // Write HIGH
    if (!GPIO::writePin(pin, true)) {
        std::cerr << "Failed to write to pin\n";
        return 1;
    }

    // Read back
    bool value = false;
    if (GPIO::readPin(pin, value)) {
        std::cout << "Pin value: " << value << "\n";
    } else {
        std::cerr << "Failed to read pin\n";
    }

    return 0;
}
*/