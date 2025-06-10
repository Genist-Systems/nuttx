#include "esp32_peripherals.hpp"

using namespace ESP32::SPI;

SPISlave::SPISlave(const std::string& devPath, int timeoutSeconds) : _devPath(devPath), _timeout(timeoutSeconds) {}
SPISlave::~SPISlave(){}

bool SPISlave::openDevice(int& fd)
{
  fd = ::open(_devPath.c_str(), O_RDWR);
  if (fd < 0)
    {
      perror("open");
      return false;
    }

  int flags = fcntl(fd, F_GETFL, 0);
  if (flags == -1 || fcntl(fd, F_SETFL, flags & ~O_NONBLOCK) == -1)
    {
      perror("fcntl");
      closeDevice(fd);
      return false;
    }

  return true;
}

void SPISlave::closeDevice(int fd)
{
  if (fd >= 0)
    {
      ::close(fd);
    }
}

bool SPISlave::waitForReadReady(int fd)
{
  fd_set rfds;
  FD_ZERO(&rfds);
  FD_SET(fd, &rfds);

  struct timeval tv { _timeout, 0 };
  int ret = select(fd + 1, &rfds, nullptr, nullptr, &tv);

  if (ret < 0)
    {
      perror("select");
      return false;
    }

  return ret > 0;
}

bool SPISlave::isAvailable()
{
  int fd;
  if (!openDevice(fd))
    return false;

  bool available = waitForReadReady(fd);
  closeDevice(fd);  
  return available;
}

bool SPISlave::Transmit(const std::vector<uint8_t>& data)
{
  int fd;
  if (!openDevice(fd))
    return false;

  ssize_t bytes = write(fd, data.data(), data.size());
  closeDevice(fd);

  if (bytes != (ssize_t)data.size())
    {
      fprintf(stderr, "Transmit error: %zd of %zu bytes written\n", bytes, data.size());
      return false;
    }

  return true;
}

bool SPISlave::Receive(std::vector<uint8_t>& data, size_t maxLen)
{
  int fd;
  if (!openDevice(fd))
    return false;

  if (!waitForReadReady(fd))
  {
    closeDevice(fd);
    return false;
  }

  data.resize(maxLen);
  ssize_t bytes = read(fd, data.data(), maxLen);
  closeDevice(fd);

  if (bytes < 0)
    {
      perror("read");
      return false;
    }

  data.resize(bytes);
  return true;
}

bool SPISlave::TransmitReceive(const std::vector<uint8_t>& tx, std::vector<uint8_t>& rx)
{
  int fd;
  if (!openDevice(fd))
    return false;

  ssize_t bytes_tx = write(fd, tx.data(), tx.size());
  if (bytes_tx != (ssize_t)tx.size())
    {
      fprintf(stderr, "Transmit failed during TransmitReceive.\n");
      closeDevice(fd);
      return false;
    }

  if (!waitForReadReady(fd))
    {
      closeDevice(fd);
      return false;
    }

  rx.resize(tx.size());
  ssize_t bytes_rx = read(fd, rx.data(), tx.size());
  closeDevice(fd);

  if (bytes_rx < 0)
    {
      perror("read");
      return false;
    }

  rx.resize(bytes_rx);
  return true;
}
