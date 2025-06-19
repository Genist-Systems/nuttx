#include <nuttx/fs/fs.h>
#include <nuttx/wireless/espnow.h>
#include <stdio.h>
#include <string.h>

static espnow_recv_cb_t g_recv_cb = NULL;

static const struct file_operations g_espnow_fops =
{
  NULL, NULL, NULL, NULL, NULL,
  espnow_ioctl,
};

int espnow_ioctl(FAR struct file *filep, int cmd, unsigned long arg)
{
  switch (cmd)
    {
      case ESPNOWIOC_REGISTER_RECV_CB:
        g_recv_cb = (espnow_recv_cb_t)((uintptr_t)arg);
        return 0;

      case ESPNOWIOC_SEND:
        {
          struct espnow_send_s *msg = (struct espnow_send_s *)(uintptr_t)arg;
          if (!msg || !msg->buf || msg->len == 0) return -EINVAL;

          /* Replace with esp_now_send() or equivalent */
          printf("Sending to %02x:%02x:%02x:%02x:%02x:%02x (%d bytes)\n",
                 msg->mac[0], msg->mac[1], msg->mac[2],
                 msg->mac[3], msg->mac[4], msg->mac[5], msg->len);
          return 0;
        }

      default:
        return -ENOTTY;
    }
}

int espnow_register(int minor)
{
  char devname[20];
  snprintf(devname, sizeof(devname), "/dev/espnow%d", minor);
  return register_driver(devname, &g_espnow_fops, 0666, NULL);
}

/* C-callable recv dispatch */
int espnow_callback_c(const uint8_t *mac, const uint8_t *data, uint16_t len)
{
  if (g_recv_cb)
    return g_recv_cb(mac, data, len);
  return 0;
}
