#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#include "PacketSender.h"

PacketSender::PacketSender():
  sockfd_(-1){
}

PacketSender::~PacketSender() {
  if (sockfd_ > 0) {
    close(sockfd_);
  }
}

int PacketSender::Init(){
  memset(&dst_addr_, 0, sizeof(struct sockaddr_in));
  dst_addr_.sin_family = AF_INET;
  if ((sockfd_ = socket(AF_INET, SOCK_RAW, IPPROTO_UDP))< 0) {
    perror("build send raw socket error!\n");
    return -1;
  }
  unsigned int tmp = 1;
  if(setsockopt(sockfd_, IPPROTO_IP, IP_HDRINCL, &tmp, sizeof(tmp)) < 0) {
    perror("setsockopt send raw error!\n");
    return -1;
  }
  return 0;
}

ssize_t PacketSender::SendIpPacket(char*buf, size_t len) {
  assert(sockfd_ > 0);
  struct iphdr* ipHead = (iphdr*)buf;
  dst_addr_.sin_addr.s_addr = ipHead->daddr;
  ssize_t bytes_sent = sendto(sockfd_, buf, len, 0, 
      (struct sockaddr*)&dst_addr_, sizeof(dst_addr_));
  if (bytes_sent < 0) {
    fprintf(stderr,"IP:%s!\n",inet_ntoa(dst_addr_.sin_addr));
    perror("send ip pack error!\n");
  }
  return bytes_sent;
}
