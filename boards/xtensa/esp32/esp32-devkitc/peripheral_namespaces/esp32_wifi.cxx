#include "esp32_peripherals.hpp"


using namespace ESP32::WiFi;




UDPServer::UDPServer(uint16_t port, size_t bufSize)
   : _sockfd(-1), _addrLen(sizeof(sockaddr_in6)), _bufSize(bufSize)
{
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


   _addrLen                = sizeof(struct sockaddr_in6);


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




void UDPServer::recvLoop()
{
   // unsigned char *buffer = (unsigned char *)malloc(_bufSize);  // malloc instead of new[]
   // struct sockaddr_in6 _clientAddr;
   // socklen_t _clientLen = sizeof(_clientAddr);


   // printf("Waiting for incoming UDP packets...\n");


   // while (true)
   // {
   //     ssize_t nbytes = recvfrom(_sockfd, buffer, _bufSize - 1, 0,
   //                               (struct sockaddr *)&_clientAddr, &_clientLen);
   //     if (nbytes < 0)
   //     {
   //         perror("recvfrom");
   //         break;
   //     }


   //     buffer[nbytes] = '\0';  // Null-terminate for safe printing
   //     char _clientIP[INET6_ADDRSTRLEN];
   //     inet_ntop(AF_INET6, &_clientAddr.sin6_addr, _clientIP, sizeof(_clientIP));


   //     printf("Received %zd bytes from [%s]:%d\n",
   //            nbytes, _clientIP, ntohs(_clientAddr.sin6_port));
   //     printf("Data: %s\n", buffer);
   // }


   // free(buffer);
   // close(_sockfd);
   socklen_t recvlen;
   int offset;
   int nbytes;
   unsigned char inbuf[1024];
   for (offset = 0; offset < 256; offset++)
   {
     printf("server: %d. Receiving up 1024 bytes\n", offset);
     recvlen = _addrLen;
     nbytes = recvfrom(_sockfd, inbuf, 1024, 0,
                       (struct sockaddr *)&_client, &recvlen);


     printf("server: %d. Received %d bytes from "
            "%02x%02x:%02x%02x:%02x%02x:%02x%02x:"
            "%02x%02x:%02x%02x:%02x%02x:%02x%02x port %d\n",
            offset, nbytes,
            _client.sin6_addr.s6_addr[0], _client.sin6_addr.s6_addr[1],
            _client.sin6_addr.s6_addr[2], _client.sin6_addr.s6_addr[3],
            _client.sin6_addr.s6_addr[4], _client.sin6_addr.s6_addr[5],
            _client.sin6_addr.s6_addr[6], _client.sin6_addr.s6_addr[7],
            _client.sin6_addr.s6_addr[8], _client.sin6_addr.s6_addr[9],
            _client.sin6_addr.s6_addr[10], _client.sin6_addr.s6_addr[11],
            _client.sin6_addr.s6_addr[12], _client.sin6_addr.s6_addr[13],
            _client.sin6_addr.s6_addr[14], _client.sin6_addr.s6_addr[15],
            ntohs(_client.sin6_port));
     if (nbytes < 0)
       {
         printf("server: %d. recv failed: %d\n", offset, errno);
         close(_sockfd);
         exit(-1);
       }


     if (static_cast<size_t>(nbytes) != _bufSize)
       {
         printf("server: %d. recv size incorrect: %d vs %d\n", offset,
                nbytes, _bufSize);
         close(_sockfd);
         exit(-1);
       }


     if (offset < inbuf[0])
       {
         printf("server: %d. %d packets lost, resetting offset\n", offset,
                inbuf[0] - offset);
         offset = inbuf[0];
       }
     else if (offset > inbuf[0])
       {
         printf("server: %d. Bad offset in buffer: %d\n", offset, inbuf[0]);
         close(_sockfd);
         exit(-1);
       }


     if (!check_buffer(inbuf))
       {
         printf("server: %d. Bad buffer contents\n", offset);
         close(_sockfd);
         exit(-1);
       }
   }
}


int UDPServer::check_buffer(unsigned char *buf)
{
 int ret = 1;
 int offset;
 int ch;
 int j;


 offset = buf[0];
 for (ch = 0x20, j = offset + 1; ch < 0x7f; ch++, j++)
   {
     if (static_cast<size_t>(j) >= _bufSize)

       {
         j = 1;
       }


     if (buf[j] != ch)
       {
         printf("server: Buffer content error for offset=%d, index=%d\n",
                offset, j);
         ret = 0;
       }
   }


 return ret;
}




UDPClient::UDPClient(const char* server_ip, uint16_t server_port, uint16_t local_port, size_t bufSize)
   : _sockfd(-1), _addrLen(sizeof(struct sockaddr_in6)), _bufSize(bufSize)
{
   _sockfd = createSocket(local_port);
   if (_sockfd < 0)
   {
       fprintf(stderr, "_client ERROR: Failed to create socket\n");
   }

   memset(&_serverAddr, 0, sizeof(_serverAddr));  // << Do this first
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




void UDPClient::fillBuffer(unsigned char *buf, int offset)
{
   buf[0] = offset;
   int ch = 0x20;
   int j = offset + 1;


   for (; ch < 0x7f; ch++, j++)
   {
       if (static_cast<size_t>(j) >= _bufSize)

           j = 1;
       buf[j] = ch;
   }
}


void UDPClient::sendLoop()
{
   unsigned char outbuf[_bufSize];


   for (int offset = 0; offset < 256; ++offset)
   {
       fillBuffer(outbuf, offset);


       printf("_client: %d. Sending %d bytes\n", offset, _bufSize);
       int nbytes = sendto(_sockfd, outbuf, _bufSize, 0,
                           (struct sockaddr *)&_serverAddr, _addrLen);
       printf("_client: %d. Sent %d bytes\n", offset, nbytes);


       if (nbytes < 0)
       {
           perror("sendto");
           break;
       }
       else if (static_cast<size_t>(nbytes) != _bufSize)

       {
           fprintf(stderr, "_client: %d. Bad send length: %d vs %d\n",
                   offset, nbytes, _bufSize);
           break;
       }


       usleep(2000000);  // Throttle to prevent flooding the server
   }
}











