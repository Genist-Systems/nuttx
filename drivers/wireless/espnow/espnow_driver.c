#include <nuttx/fs/fs.h>
#include <nuttx/wireless/espnow.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>

#include "esp_wifi.h"

#include "esp_now.h"
#include "esp_log.h"

#define TAG "espnow_driver"

static espnow_recv_cb_t g_recv_cb = NULL;
static bool g_initialized = false;

/****************************************************************************
 * Internal Init
 ****************************************************************************/

static int espnow_internal_init(void)
{
  if (g_initialized)
    return 0;

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  if (esp_wifi_init(&cfg) != ESP_OK)
    {
      printf("esp_wifi_init failed\n");
      return -1;
    }

  esp_wifi_set_mode(WIFI_MODE_STA);
  esp_wifi_start();

  if (esp_now_init() != ESP_OK)
    {
      printf("esp_now_init failed\n");
      return -1;
    }

  g_initialized = true;
  return 0;
}

/****************************************************************************
 * espnow_ioctl
 ****************************************************************************/

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
          if (!msg || !msg->buf || msg->len == 0)
            return -EINVAL;

          if (espnow_internal_init() != 0)
            return -EIO;

          // Add peer if not exists
          esp_now_peer_info_t peer = {
            .channel = 0,
            .ifidx = ESP_IF_WIFI_STA,
            .encrypt = false
          };
          memcpy(peer.peer_addr, msg->mac, 6);

          if (!esp_now_is_peer_exist(msg->mac))
            {
              if (esp_now_add_peer(&peer) != ESP_OK)
                {
                  printf("Failed to add peer\n");
                  return -EIO;
                }
            }

          if (esp_now_send(msg->mac, msg->buf, msg->len) != ESP_OK)
            {
              printf("esp_now_send() failed\n");
              return -EIO;
            }

          return 0;
        }

      default:
        return -ENOTTY;
    }
}

/****************************************************************************
 * Read/Write not implemented
 ****************************************************************************/

static const struct file_operations g_espnow_fops =
{
  NULL, // open
  NULL, // close
  NULL, // read
  NULL, // write
  NULL, // seek
  espnow_ioctl,
};

/****************************************************************************
 * Driver registration
 ****************************************************************************/

int espnow_register(int minor)
{
  char devname[20];
  snprintf(devname, sizeof(devname), "/dev/espnow%d", minor);
  return register_driver(devname, &g_espnow_fops, 0666, NULL);
}

/****************************************************************************
 * C-callable receive dispatch
 ****************************************************************************/

int espnow_callback_c(const uint8_t *mac, const uint8_t *data, uint16_t len)
{
  if (g_recv_cb)
    return g_recv_cb(mac, data, len);
  return 0;
}
