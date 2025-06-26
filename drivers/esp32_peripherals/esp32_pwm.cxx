#include "esp32_peripherals/esp32_peripherals.hpp"

using namespace ESP32::PWM;

PWM::PWM() = default;
PWM::~PWM()
{
    if (_fd >=0)
        close(_fd);
}

bool PWM::setup(const char* devPath, struct pwm_info_s* info)
{
    if (info == nullptr)
    {
        perror("pwm config");
        return false;
    }
    
    if (_fd >= 0) {
        close(_fd);
        _fd = -1;
    }

    _fd = open(devPath, O_RDWR);
    if (_fd < 0) {
        perror("pwm open");
        return false;
    }

    _config = info;

    int ret = ioctl(_fd, PWMIOC_SETCHARACTERISTICS, (unsigned long)(uintptr_t)_config);
    if (ret < 0) {
        perror("pwm ioctl set characteristics");
        close(_fd);
        _fd = -1;
        return false;
    }

    return true;
}

bool PWM::editFreq(uint32_t newFreq)
{
    if (_fd < 0) return false;

    _config->frequency = newFreq;

    int ret = ioctl(_fd, PWMIOC_SETCHARACTERISTICS, (unsigned long)(uintptr_t)_config);
    if (ret < 0) {
        perror("pwm ioctl editFreq");
        return false;
    }

    return true;
}

bool PWM::editDuty(uint8_t newDutyPercent)
{
    if (_fd < 0) return false;

    if (newDutyPercent > 100) {
        newDutyPercent = 100;
    }

    _config->duty = newDutyPercent
        ? b16divi(uitoub16(newDutyPercent) - 1, 100)
        : 0;

    int ret = ioctl(_fd, PWMIOC_SETCHARACTERISTICS, (unsigned long)(uintptr_t)_config);
    if (ret < 0) {
        perror("pwm ioctl editDuty");
        return false;
    }

    return true;
}

bool PWM::start()
{
    if (_fd < 0) return false;

    int ret = ioctl(_fd, PWMIOC_START, 0);
    if (ret < 0) {
        perror("pwm ioctl start");
        return false;
    }

    return true;
}

bool PWM::stop()
{
    if (_fd < 0) return false;

    int ret = ioctl(_fd, PWMIOC_STOP, 0);
    if (ret < 0) {
        perror("pwm ioctl stop");
        return false;
    }

    return true;
}
