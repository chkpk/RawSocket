#ifndef PACKETSENDER_H
#define PACKETSENDER_H

#include <netinet/in.h>  // sockaddr_in
#include <sys/types.h>  // ssize_t

class PacketSender {
  public:
    PacketSender();
    virtual ~PacketSender();
    int Init();
    ssize_t SendIpPacket(char*buf, size_t len);  
  private:
    int sockfd_;
    struct sockaddr_in  dst_addr_;
};

#endif // PACKETSENDER_H
