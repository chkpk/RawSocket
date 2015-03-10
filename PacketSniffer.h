#ifndef PACKETSNIFFER_H
#define PACKETSNIFFER_H

#include <netpacket/packet.h>  // sockaddr_ll
#include <sys/types.h>  // ssize_t
#include <string>

class PacketSniffer {
  public:
    PacketSniffer(const std::string& if_name);
    virtual ~PacketSniffer();
    int Init();
    ssize_t RecvEthPacket(char*buf, size_t len);  
    int SetPromisc(bool flag);
  private:
    PacketSniffer();
    int sockfd_;
    std::string if_name_; //本地网卡名称 eth0， eth1
    struct sockaddr_ll  addr_sll_;
};

#endif // PACKETSNIFFER_H
