#include "esp32_peripherals.hpp"

namespace ESP32::PWM
{
    bool setup(const std::string& devPath, const Config& config)
    {
        int fd = open(devPath.c_str(), O_RDWR);
        if (fd < 0) {
            perror(("PWM: open failed on " + devPath).c_str());
            return false;
        }

        struct pwm_info_s info;
        info.frequency = config.frequency;

        // 👇 Flip duty if inverted
        uint8_t duty = config.inverted
            ? (100 - config.dutyPercent)
            : config.dutyPercent;

        info.duty = duty
            ? b16divi(uitoub16(duty) - 1, 100)
            : 0;

        int ret = ioctl(fd, PWMIOC_SETCHARACTERISTICS, (unsigned long)((uintptr_t)&info));
        if (ret < 0) {
            perror(("PWM: ioctl(PWMIOC_SETCHARACTERISTICS) failed on " + devPath).c_str());
        }

        close(fd);
        return ret == 0;
    }

    bool start(const std::string& devPath)
    {
        int fd = open(devPath.c_str(), O_RDWR);
        if (fd < 0) {
            perror(("PWM: open failed on " + devPath).c_str());
            return false;
        }

        int ret = ioctl(fd, PWMIOC_START, 0);
        if (ret < 0) {
            perror(("PWM: ioctl(PWMIOC_START) failed on " + devPath).c_str());
        }

        close(fd);
        return ret == 0;
    }

    bool stop(const std::string& devPath)
    {
        int fd = open(devPath.c_str(), O_RDWR);
        if (fd < 0) {
            perror(("PWM: open failed on " + devPath).c_str());
            return false;
        }

        int ret = ioctl(fd, PWMIOC_STOP, 0);
        if (ret < 0) {
            perror(("PWM: ioctl(PWMIOC_STOP) failed on " + devPath).c_str());
        }

        close(fd);
        return ret == 0;
    }

}