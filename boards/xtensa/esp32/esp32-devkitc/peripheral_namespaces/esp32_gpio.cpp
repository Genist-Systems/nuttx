#include "esp32_peripherals.hpp"


namespace ESP32::GPIO
{
    bool setPinType(const std::string& devPath, PinType type)
    {
        int fd = open(devPath.c_str(), O_RDWR);
        if (fd < 0)
        {
            perror(("GPIO: open failed on " + devPath).c_str());
            return false;
        }

        int ret = ioctl(fd, GPIOC_SETPINTYPE, static_cast<unsigned long>(type));
        if (ret < 0)
        {
            perror(("GPIO: ioctl(GPIOC_SETPINTYPE) failed on " + devPath).c_str());
        }

        close(fd);
        return ret == 0;
    }

    bool writePin(const std::string& devPath, bool value)
    {
        int fd = open(devPath.c_str(), O_RDWR);
        if (fd < 0)
        {
            perror(("GPIO: open failed on " + devPath).c_str());
            return false;
        }

        int ret = ioctl(fd, GPIOC_WRITE, static_cast<unsigned long>(value));
        if (ret < 0)
        {
            perror(("GPIO: ioctl(GPIOC_WRITE) failed on " + devPath).c_str());
        }

        close(fd);
        return ret == 0;
    }

    bool readPin(const std::string& devPath, bool& value)
    {
        int fd = open(devPath.c_str(), O_RDWR);
        if (fd < 0)
        {
            perror(("GPIO: open failed on " + devPath).c_str());
            return false;
        }

        int val = 0;
        int ret = ioctl(fd, GPIOC_READ, (unsigned long)((uintptr_t)&val));
        if (ret == 0)
        {
            value = (val != 0);
        }
        else
        {
            perror(("GPIO: ioctl(GPIOC_READ) failed on " + devPath).c_str());
        }

        close(fd);
        return ret == 0;
    }

}

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