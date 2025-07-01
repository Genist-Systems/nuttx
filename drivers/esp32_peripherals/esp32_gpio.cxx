#include "esp32_peripherals/esp32_peripherals.hpp"

#ifdef CONFIG_DEV_GPIO



namespace ESP32::GPIO {

void (*GPIO::_userCallback)(void) = nullptr; 

} 

using namespace ESP32::GPIO;

GPIO::GPIO() = default;

GPIO::~GPIO()
{
    if (_fd >= 0) {
        close(_fd);
        _fd = -1;
    }
}

bool GPIO::setPinType(const char* devPath, enum gpio_pintype_e type)
{
    if (_fd >= 0) {
        close(_fd);
        _fd = -1;
    }

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
    if (_fd < 0) {
        fprintf(stderr, "GPIO not initialized\n");
        return false;
    }

    int ret = ioctl(_fd, GPIOC_WRITE, static_cast<unsigned long>(value));
    if (ret < 0) {
        perror("gpio ioctl write");
        return false;
    }

    return true;
}

bool GPIO::readPin(PinStatus& value)
{
    if (_fd < 0) {
        fprintf(stderr, "GPIO not initialized\n");
        return false;
    }

    int val = 0;
    int ret = ioctl(_fd, GPIOC_READ, (unsigned long)((uintptr_t)&val));
    if (ret == 0) {
        value = static_cast<PinStatus>(val != 0 ? PinStatus::GPIO_HIGH : PinStatus::GPIO_LOW);
        return true;
    } else {
        perror("gpio ioctl read");
        return false;
    }
}

bool GPIO::attachInterrupt(int signo, void (*user_callback)(void))
{
    if (_fd < 0) {
        fprintf(stderr, "GPIO not initialized\n");
        return false;
    }

    if (_pinType != GPIO_INTERRUPT_PIN) {
        fprintf(stderr, "GPIO not set as valid interrupt type\n");
        return false;
    }

    // Save user callback
    _userCallback = user_callback;

    // Register signal handler
    struct sigaction sa = {};
    sa.sa_handler = _signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(signo, &sa, nullptr) < 0) {
        perror("sigaction");
        return false;
    }

    // Register sigevent
    struct sigevent sev = {};
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = signo;

    if (ioctl(_fd, GPIOC_REGISTER, (unsigned long)&sev) < 0) {
        perror("gpio ioctl GPIOC_REGISTER");
        return false;
    }

    return true;
}

bool GPIO::detachInterrupt(int signo)
{
    if (_fd < 0) {
        fprintf(stderr, "GPIO not initialized\n");
        return false;
    }

    if (_pinType != GPIO_INTERRUPT_PIN) {
        fprintf(stderr, "GPIO not configured as interrupt pin\n");
        return false;
    }

    // Unregister from the GPIO driver
    struct sigevent sev = {};
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = signo;

    if (ioctl(_fd, GPIOC_UNREGISTER, (unsigned long)&sev) < 0) {
        perror("gpio ioctl GPIOC_UNREGISTER");
        return false;
    }

    // Remove the signal handler (optional but clean)
    struct sigaction sa = {};
    sa.sa_handler = SIG_DFL;  // Reset to default handler
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(signo, &sa, nullptr) < 0) {
        perror("sigaction detach");
        return false;
    }

    // Clear the static callback
    _userCallback = nullptr;

    return true;
}



void GPIO::_signal_handler(int signo)
{
    if (_userCallback) {
        _userCallback();
    } else {
        printf("Interrupt %d received (no callback set)\n", signo);
    }
}



#endif // CONFIG_DEV_GPIO

