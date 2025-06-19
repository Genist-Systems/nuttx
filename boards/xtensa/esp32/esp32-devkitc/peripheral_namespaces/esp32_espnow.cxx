#include "esp32_peripherals.hpp"

using namespace ESP32::ESPNOW;

ESPNow::ESPNow(const uint8_t mac[6], int minor)
{
  memcpy(_mac, mac, 6);
  snprintf(_devname, sizeof(_devname), "/dev/espnow%d", minor);
  _fd = open(_devname, O_RDWR);
  if (_fd < 0)
    {
      printf("Failed to open %s\n", _devname);
    }
}

ESPNow::~ESPNow()
{
  if (_fd >= 0)
    {
      close(_fd);
    }
}

bool ESPNow::valid() const
{
  return _fd >= 0;
}

bool ESPNow::send(const uint8_t *data, uint16_t len)
{
  if (_fd < 0 || !data || len == 0) return false;

  struct espnow_send_s msg;
  msg.buf = data;
  msg.len = len;
  memcpy(msg.mac, _mac, 6);

  return ioctl(_fd, ESPNOWIOC_SEND, (unsigned long)(uintptr_t)&msg) == 0;
}

bool ESPNow::register_callback(espnow_recv_cb_t cb)
{
  if (_fd < 0 || cb == nullptr) return false;

  return ioctl(_fd, ESPNOWIOC_REGISTER_RECV_CB,
               (unsigned long)(uintptr_t)cb) == 0;
}

