#ifndef __INCLUDE_NUTTX_WIRELESS_ESPNOW_H
#define __INCLUDE_NUTTX_WIRELESS_ESPNOW_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/ioctl.h>
#include <stdint.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* IOCTL commands for ESP-NOW driver */

#define ESPNOWIOC_REGISTER_RECV_CB _WLCIOC(0x01)
#define ESPNOWIOC_SEND             _WLCIOC(0x02)

/****************************************************************************
 * Public Types
 ****************************************************************************/

/* Structure used to send data via ESP-NOW */

struct espnow_send_s
{
  uint8_t mac[6];             /* Destination MAC address */
  FAR const uint8_t *buf;     /* Pointer to data buffer */
  uint16_t len;               /* Length of data */
};

/* Callback type for receiving ESP-NOW data */

typedef int (*espnow_recv_cb_t)(const uint8_t *mac,
                                const uint8_t *data,
                                uint16_t len);

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/* Register the ESP-NOW driver as /dev/espnowX */

int espnow_register(int minor);

/* Called internally by espnow backend (e.g., ESP-NOW recv callback)
 * to dispatch to user-registered callback
 */
int espnow_callback_c(const uint8_t *mac, const uint8_t *data, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif /* __INCLUDE_NUTTX_WIRELESS_ESPNOW_H */
