#include "esp32_peripherals.hpp"




using namespace ESP32::WiFi::UDP;




UDPServer::UDPServer(uint16_t port, const char* ifname, const char* ip)
  : _sockfd(-1), _addrLen(sizeof(sockaddr_in6))
{
   configureInterfaceIPv6(ifname, ip);


   _sockfd = socket(PF_INET6, SOCK_DGRAM, 0);
   if (_sockfd < 0)
   {
       perror("socket");
       return;
   }


   int optval = 1;
   if (setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) < 0)
   {
       perror("setsockopt SO_REUSEADDR");
       close(_sockfd);
       return;
   }


   _server.sin6_family     = AF_INET6;
   _server.sin6_port       = HTONS(port);
   memset(&_server.sin6_addr, 0, sizeof(struct in6_addr));


   _addrLen = sizeof(struct sockaddr_in6);


   if (bind(_sockfd, (struct sockaddr *)&_server, _addrLen) < 0)
   {
       perror("bind");
       close(_sockfd);
       return;
   }


   printf("UDPServer bound to port %d\n", port);
}






UDPServer::~UDPServer()
{
  if (_sockfd >= 0)
      close(_sockfd);
}




bool UDPServer::configureInterfaceIPv6(const char* ifname, const char* ip)
{
   struct in6_addr addr;
   struct in6_addr netmask;


   if (inet_pton(AF_INET6, ip, &addr) != 1)
   {
       perror("UDPServer: Failed to parse IPv6 address");
       return false;
   }


   // Typically /64 for link-local or static setup
   netlib_prefix2ipv6netmask(64, &netmask);


   if (netlib_set_ipv6addr(ifname, &addr) < 0)
   {
       perror("UDPServer: netlib_set_ipv6addr");
       return false;
   }


   if (netlib_set_ipv6netmask(ifname, &netmask) < 0)
   {
       perror("UDPServer: netlib_set_ipv6netmask");
       return false;
   }


   if (netlib_ifup(ifname) < 0)
   {
       perror("UDPServer: netlib_ifup");
       return false;
   }


   return true;
}



int UDPServer::receiveMessage(uint8_t* buffer, size_t bufSize)
{
    if (_sockfd < 0 || buffer == nullptr)
        return -1;

    socklen_t recvlen = _addrLen;
    int nbytes = recvfrom(_sockfd, buffer, bufSize, 0,
                          (struct sockaddr *)&_client, &recvlen);

    if (nbytes < 0)
    {
        perror("recvfrom");
        return -1;
    }

    printf("UDPServer: Received %d bytes\n", nbytes);
    printf("UDPServer: Message: \"%.*s\"\n", nbytes, buffer);

    return nbytes;
}

// printf("Starting UDPServer on wpan0...\n");

    // UDPServer server(5471, "wpan0", "fe80::ff:fe00:a");

    // uint8_t buffer[250];
    // while (1)

    // {
    // int n = server.receiveMessage(buffer, sizeof(buffer));
    //     if (n > 0)
    //     {
    //         printf("Client received: \"%.*s\"\n", n, buffer);
    //     }
    // }




UDPClient::UDPClient(uint16_t server_port, uint16_t local_port, const char* ifname, const char* server_ip)
  : _sockfd(-1), _addrLen(sizeof(struct sockaddr_in6))
{
  if (netlib_ifup(ifname) < 0)
   {
       perror("UDPServer: netlib_ifup");
   }


 
   _sockfd = createSocket(local_port);
  if (_sockfd < 0)
  {
      fprintf(stderr, "_client ERROR: Failed to create socket\n");
  }


  memset(&_serverAddr, 0, sizeof(_serverAddr));
  _serverAddr.sin6_family = AF_INET6;
  _serverAddr.sin6_port = htons(server_port);
  if (inet_pton(AF_INET6, server_ip, &_serverAddr.sin6_addr) != 1)
  {
      fprintf(stderr, "_client ERROR: Invalid server IP address\n");
  }


}


UDPClient::~UDPClient()
{
  if (_sockfd >= 0)
      close(_sockfd);
}

int UDPClient::createSocket(uint16_t local_port)
{
  int sockfd = socket(AF_INET6, SOCK_DGRAM, 0);
  if (sockfd < 0)
  {
      perror("socket");
      return -1;
  }




  _addr.sin6_family     = AF_INET6;
  _addr.sin6_port       = HTONS(local_port);
  memset(_addr.sin6_addr.s6_addr, 0, sizeof(struct in6_addr));
  _addrLen              = sizeof(struct sockaddr_in6);




  if (bind(sockfd, (struct sockaddr *)&_addr, sizeof(_addr)) < 0)
  {
      perror("bind");
      close(sockfd);
      return -1;
  }




  return sockfd;
}



int UDPClient::sendMessage(const char* msg, size_t msgLen)
{
    
    if (_sockfd < 0 || msg == nullptr)
        return -1;

    int nbytes = sendto(_sockfd, msg, msgLen, 0,
                        (struct sockaddr *)&_serverAddr, _addrLen);

    if (nbytes < 0)
    {
        perror("sendto");
        return -1;
    }
    else if ((size_t)nbytes != msgLen)
    {
        fprintf(stderr, "Bad send length: %d vs %zu\n", nbytes, msgLen);
        return -1;
    }

    return nbytes;
}

// const char* server_ip = "fe80::ff:fe00:a"; // Replace with actual server IP
//     uint16_t server_port = 5471;
//     uint16_t local_port = 5472;

//     printf("Starting UDPClient to [%s]:%d...\n", server_ip, server_port);

//     UDPClient client(server_port, local_port, "wpan0", server_ip);

//     const char* msg = "Hello from client!";
//     while(1)
//     {
//         client.sendMessage(msg, strlen(msg));
//         usleep(1000000);
//     }





























