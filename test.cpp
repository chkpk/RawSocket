#include <stdlib.h>
#include <stdio.h>
#include <netinet/if_ether.h>

#include "PacketSniffer.h"
#include "PacketSender.h"
#include "CheckSum.h"

int main(int argc, char* argv[]){

  uint16_t port = 0;
  if (argc < 2){
    printf("test ifname <port>\neg:test eth0 1234\n");
    return -1;
  }
  if (argc > 2){
    port = static_cast<uint16_t>(atoi(argv[2]));
    if (port > 0)
      printf("dst port : %u\n", port);
  }

  PacketSender sender;
  if( sender.Init() < 0)
    return 0;

  PacketSniffer sniffer(argv[1]);
  if( sniffer.Init() < 0)
    return 0;
  if( sniffer.SetPromisc(true) < 0)
    return 0;

  char buf[0xffff];
  ssize_t len;
  for (;;){
    len = sniffer.RecvEthPacket(buf, 0xffff);
    
    // eth
    ethhdr* pEth = (ethhdr*) buf;   // begin from ethhdr
    if (pEth->h_proto != 0x08) continue;  // non IP packet.

    // ip
    iphdr* pIp = (iphdr*) (buf + sizeof(ethhdr));
    sockaddr_in addr_src, addr_dst;
    addr_src.sin_addr.s_addr = pIp->saddr;
    addr_dst.sin_addr.s_addr = pIp->daddr;

    // tcp/udp
    switch( pIp->protocol){
      case 0x11:  // udp packet
        {
          udphdr* pUdp = (udphdr*) (buf + sizeof(ethhdr) + pIp->ihl * 4);
          if (port > 0 && ntohs(pUdp->dest) != port) break;
          printf("[udp %-16s : %-6u -> %-16s : %-6u , len : %-4u] ", 
              inet_ntoa(addr_src.sin_addr), ntohs(pUdp->source), 
              inet_ntoa(addr_dst.sin_addr), ntohs(pUdp->source), 
              ntohs(pUdp->len)); 

          // test CheckSum.cpp (IP/UDP)
          printf("origin cksum(ip,udp): %x %x, %x %x\t",
              pIp->check >> 8, pIp->check & 0xff,
              pUdp->check >> 8, pUdp->check & 0xff);
          uint16_t ckIP = cksumIp(pIp);
          uint16_t ckUDP = cksumUdp(pIp,pUdp);
          printf("cal cksum: %x %x, %x %x\n", ckIP >> 8, ckIP & 0xff, ckUDP >> 8, ckUDP & 0xff);

          // test PacketSender
          // 1.swap src_ip and dst_ip 
          uint32_t tmp = pIp->saddr;
          pIp->saddr = pIp->daddr;
          pIp->daddr = tmp;
          // 2.change src_port and dst_port to 20000
          pUdp->dest = htons(20000);
          pUdp->source = htons(20000);
          // 3.calculate new checksums
          pIp->check = cksumIp(pIp);
          pUdp->check = cksumUdp(pIp,pUdp);
          // 4.send packet
          sender.SendIpPacket((char*)pIp, len - sizeof(ethhdr));
          break;
        }
      case 0x06:  //tcp
        {
          tcphdr* pTcp = (tcphdr*) (buf + sizeof(ethhdr) + pIp->ihl * 4);
          if (port > 0 && ntohs(pTcp->dest) != port) break;
          printf("[tcp %-16s : %-6u -> %-16s : %-6u , len : %-4u] ", 
              inet_ntoa(addr_src.sin_addr), ntohs(pTcp->source), 
              inet_ntoa(addr_dst.sin_addr), ntohs(pTcp->source), 
              ntohs(pIp->tot_len) - 34); 

          // test CheckSum.cpp (IP/TCP)
          printf("origin cksum(ip,tcp): %x %x, %x %x\t",
              pIp->check >> 8, pIp->check & 0xff,
              pTcp->check >> 8, pTcp->check & 0xff);
          uint16_t ckIP = cksumIp(pIp);
          uint16_t ckTCP = cksumTcp(pIp,pTcp);
          printf("cal cksum: %x %x, %x %x\n", ckIP >> 8, ckIP & 0xff, ckTCP >> 8, ckTCP & 0xff);
          break;
        }
      default:
        continue;
    }
  }
  return 0;
}

