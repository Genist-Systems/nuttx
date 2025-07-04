#include "esp32_peripherals/esp32_peripherals.hpp"


using namespace ESP32::WiFi::TCP;



TCPServer::TCPServer(const TCPSettings& settings)
    : _port(settings.port), _server_ip(settings.server_ip), _ifname(settings.ifname), _ssid(settings.ssid), _password(settings.password), _server_fd(-1), _client_fd(-1)
{
    memset(&_server_addr, 0, sizeof(_server_addr));
    memset(&_client_addr, 0, sizeof(_client_addr));
}


TCPServer::~TCPServer()
{
    closeAll();
}

bool TCPServer::init()
{
    if (!configureWiFi())
        return false;
    
    // config_lorawan_radioenge_t lora_config;
    // snprintf(lora_config.application_session_key, APP_SESSION_KEY_SIZE,
    //             "00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00");
    // snprintf(lora_config.network_session_key, NW_SESSION_KEY_SIZE,
    //             "00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00");
    // snprintf(lora_config.application_eui, APP_EUI_SIZE,
    //             "00:00:00:00:00:00:00:00");
    // snprintf(lora_config.device_address, DEVICE_ADDRESS_SIZE,
    //             "00:00:00:00");
    // snprintf(lora_config.channel_mask, CHANNEL_MASK_SIZE,
    //             "00FF:0000:0000:0000:0000:0000");
    // lorawan_radioenge_init(lora_config);

    _server_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (_server_fd < 0)
    {
        perror("socket");
        return false;
    }

    _server_addr.sin_family = AF_INET;
    _server_addr.sin_port = htons(_port);
    _server_addr.sin_addr.s_addr = inet_addr(_server_ip);

    if (bind(_server_fd, (struct sockaddr*)&_server_addr,
                sizeof(_server_addr)) < 0)
    {
        perror("bind");
        return false;
    }

    if (listen(_server_fd, 1) != 0)
    {
        perror("listen");
        return false;
    }

    printf("Listening on %s:%d\n", _server_ip, _port);
    return true;
}

bool TCPServer::acceptClient()
{
    socklen_t client_len = sizeof(_client_addr);
    _client_fd = accept4(_server_fd,
                            (struct sockaddr*)&_client_addr,
                            &client_len, SOCK_CLOEXEC);

    if (_client_fd < 0)
    {
        perror("accept");
        return false;
    }

    printf("Client connected\n");
    return true;
}

// bool TCPServer::receiveMessage(T& data)
// {
//     // static_assert(std::is_trivially_copyable<T>::value, "T must be trivially copyable");

//     int received = read(_client_fd, &data, sizeof(T));
//     if (received <= 0)
//     {
//         if (received == 0)
//             printf("Client disconnected\n");
//         else
//             perror("read");

//         return false;
//     }

//     printf("Received %d bytes\n", received);
//     return true;
// }

// bool TCPServer::sendMessage(const T& data)
// {
//     // static_assert(std::is_trivially_copyable<T>::value, "T must be trivially copyable");

//     int sent = send(_client_fd, &data, sizeof(T), 0);
//     if (sent <= 0)
//     {
//         perror("send");
//         return false;
//     }

//     printf("Sent %d bytes\n", sent);
//     return true;
// }


bool TCPServer::configureWiFi()
{
    // Create socket for WiFi configuration
    int sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        perror("socket for WiFi config failed");
        return false;
    }

    struct in_addr ipaddr;
    if (!inet_aton(_server_ip, &ipaddr))
    {
        perror("inet_aton (client IP)");
        return false;
    }
    #ifdef CONFIG_NET_IPv4
    if (netlib_set_ipv4addr(_ifname, &ipaddr) < 0)
    {
        perror("netlib_set_ipv4addr");
        return false;
    }
    #endif
    // Set WPA2-PSK with CCMP cipher (equivalent to: wapi psk wlan0 _password 3 2)
    int ret = wpa_driver_wext_set_auth_param(sock, _ifname,
                                           IW_AUTH_WPA_VERSION,
                                           IW_AUTH_WPA_VERSION_WPA2);
    if (ret < 0)
    {
        perror("wpa_driver_wext_set_auth_param (WPA_VERSION) failed");
        return false;
    }

    ret = wpa_driver_wext_set_auth_param(sock, _ifname,
                                       IW_AUTH_CIPHER_PAIRWISE,
                                       IW_AUTH_CIPHER_CCMP);
    if (ret < 0)
    {
        perror("wpa_driver_wext_set_auth_param (CIPHER_PAIRWISE) failed");
        return false;
    }

    ret = wpa_driver_wext_set_key_ext(sock, _ifname, WPA_ALG_CCMP,
                                    _password, strlen(_password));
    if (ret < 0)
    {
        perror("wpa_driver_wext_set_key_ext failed");
        return false;
    }

    // Set ESSID after PSK
    if (wapi_set_essid(sock, "wlan0", _ssid, WAPI_ESSID_ON) < 0)
    {
        perror("wapi_set_essid failed");
        return false;
    }

    // printf("SoftAP (Master) Wi-Fi secured.\n");
    
    // Close the WiFi configuration socket
    close(sock);
    return true;
}





void TCPServer::closeAll()
{
    if (_client_fd >= 0)
        close(_client_fd);
    if (_server_fd >= 0)
        close(_server_fd);
}





TCPClient::TCPClient(const TCPSettings& settings)
  : _port(settings.port), _server_ip(settings.server_ip), _client_ip(settings.client_ip), _ifname(settings.ifname), _ssid(settings.ssid), _password(settings.password), _sockfd(-1)
{
  memset(&_server_addr, 0, sizeof(_server_addr));
}

TCPClient::~TCPClient()
{
    closeSocket();
}

bool TCPClient::connectToServer()
{
  if (!configureWiFi())
        return false;
  
    _sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (_sockfd < 0)
  {
    perror("socket");
    return false;
  }

  _server_addr.sin_family = AF_INET;
  _server_addr.sin_port = htons(_port);
  if (inet_pton(AF_INET, _server_ip, &_server_addr.sin_addr) <= 0)
  {
    perror("inet_pton");
    return false;
  }

  if (connect(_sockfd, (struct sockaddr*)&_server_addr, sizeof(_server_addr)) < 0)
  {
    perror("connect");
    return false;
  }

  printf("Connected to server at %s:%d\n", _server_ip, _port);
  return true;
}

// bool TCPClient::sendMessage(const T& data)
// {
// //   static_assert(std::is_trivially_copyable<T>::value, "Data must be trivially copyable");

//   int sent = write(_sockfd, &data, sizeof(T));
//   if (sent <= 0)
//   {
//     perror("write");
//     return false;
//   }
//   return true;
// }

// bool TCPClient::receiveMessage(T& data)
// {
// //   static_assert(std::is_trivially_copyable<T>::value, "Data must be trivially copyable");

//   int received = recv(_sockfd, &data, sizeof(T), 0);
//   if (received <= 0)
//   {
//     perror("recv");
//     return false;
//   }
//   return true;
// }


bool TCPClient::configureWiFi()
{
    
    int sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        perror("socket for WiFi config failed");
        return false;
    }

    struct in_addr ipaddr;
    if (!inet_aton(_client_ip, &ipaddr))
    {
        perror("inet_aton (client IP)");
        return false;
    }
    #ifdef CONFIG_NET_IPv4
    if (netlib_set_ipv4addr(_ifname, &ipaddr) < 0)
    {
        perror("netlib_set_ipv4addr");
        return false;
    }
    #endif

    int ret = wpa_driver_wext_set_auth_param(sock, _ifname,
                                             IW_AUTH_CIPHER_PAIRWISE,
                                             IW_AUTH_CIPHER_CCMP);
    if (ret < 0)
    {
        perror("wpa_driver_wext_set_auth_param (CIPHER_PAIRWISE) failed");
        close(sock);
        return false;
    }


    ret = wpa_driver_wext_set_key_ext(sock, _ifname, WPA_ALG_CCMP,
                                      _password, strlen(_password));
    if (ret < 0)
    {
        perror("wpa_driver_wext_set_key_ext failed");
        close(sock);
        return false;
    }

    // Set ESSID
    if (wapi_set_essid(sock, _ifname, _ssid, WAPI_ESSID_ON) < 0)
    {
        perror("wapi_set_essid failed");
        close(sock);
        return false;
    }

    // printf("SoftAP (Master) Wi-Fi with CCMP secured (no WPA version).\n");
    close(sock);
    return true;
}


void TCPClient::closeSocket()
{
  if (_sockfd >= 0)
    close(_sockfd);
}


