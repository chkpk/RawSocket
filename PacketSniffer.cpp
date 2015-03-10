#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include "PacketSniffer.h"

PacketSniffer::PacketSniffer(const std::string& if_name):
  sockfd_(-1),
  if_name_(if_name){
}

PacketSniffer::~PacketSniffer() {
  if (sockfd_ > 0) {
    close(sockfd_);
  }
}

int PacketSniffer::Init(){
  if ((sockfd_ = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_IP))) < 0) {
    perror("build recv raw socket error!\n");
    return -1;
  }

  struct ifreq ifstruct;
  strcpy(ifstruct.ifr_name, if_name_.c_str());
  if (ioctl(sockfd_, SIOCGIFINDEX, &ifstruct) < 0) {
    perror("get inet index error!\n");
    return -1;
  }

  memset(&addr_sll_, 0, sizeof(struct sockaddr_ll));
  addr_sll_.sll_family = AF_PACKET;
  addr_sll_.sll_ifindex = ifstruct.ifr_ifindex;
  addr_sll_.sll_protocol = htons(ETH_P_IP);

  if(bind(sockfd_, (struct sockaddr *) &addr_sll_, sizeof(addr_sll_)) == -1 ) {
    perror("bind socket error!\n");
    return -1;
  }
  return 0;
}

int PacketSniffer::SetPromisc(bool flag) {
  struct ifreq ifr;
  strcpy(ifr.ifr_name, if_name_.c_str());
  if(ioctl(sockfd_, SIOCGIFFLAGS, &ifr) == -1) {
    perror("iotcl get promisc flag error!\n");
    return -1;
  }

  if (flag == true)
    ifr.ifr_flags |= IFF_PROMISC;
  else
    ifr.ifr_flags &= ~IFF_PROMISC;

  if(ioctl(sockfd_, SIOCSIFFLAGS, &ifr) == -1) {
    perror("iotcl set promisc flag error!\n");
    return -1;
  }
  return 0;
}

ssize_t PacketSniffer::RecvEthPacket(char*buf, size_t len) {
  assert(sockfd_ > 0);
  ssize_t bytes_recv = recv(sockfd_, buf, len, 0);
  if (bytes_recv == -1)
    perror("socket recved error!\n");
  return bytes_recv;
}
