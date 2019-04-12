#include "./linkUdp.h"
void linkUdp_send(uint8_t* data, uint16_t dataLen, LinkCfgStruct* link);

static NetworkInterface* net;
static Thread thread;
static void thread_recvData(void* x);

LinkCfgStruct* linkUdp_startServer(uint8_t* ip, uint32_t port) {
  net = NetworkInterface::get_default_instance();
  if (!net) {
    printf("Error! No network inteface found.\n");
  }

  LinkCfgStruct* link = link_new(NULL);
  link->type = linkType_udp;
  link->status = linkStatus_server;
  mem_cpy(link->address, ip, 4);
  link->port = port;
  link->fd = (void*)new UDPSocket;
  link->send = linkUdp_send;
  link->recv = fiip_recv;

  ((UDPSocket*)link->fd)->open(net);
  if (((UDPSocket*)link->fd)->bind(link->port) < 0) {
    perror("bind error:");
  }

  thread.start(callback(thread_recvData, link));
  return link;
}
LinkCfgStruct* linkUdp_startClient(uint8_t* ip, uint32_t port) {
  net = NetworkInterface::get_default_instance();
  if (!net) {
    printf("Error! No network inteface found.\n");
  }

  LinkCfgStruct* link = link_new(NULL);
  link->type = linkType_udp;
  link->status = linkStatus_client;
  mem_cpy(link->address, ip, 4);
  link->port = port;
  link->fd = (void*)new UDPSocket;
  link->send = linkUdp_send;
  link->recv = fiip_recv;

  ((UDPSocket*)link->fd)->open(net);

  thread.start(callback(thread_recvData, link));
  return link;
}

void linkUdp_send(uint8_t* data, uint16_t dataLen, LinkCfgStruct* rlink) {
  UDPSocket* fd = (UDPSocket*)rlink->fd;
  SocketAddress clientCfg(rlink->address, NSAPI_IPv4, rlink->port);

  if (fd->sendto(clientCfg, data, dataLen) < 0) {
    printf("Error sending data\n");
  }
}

void thread_recvData(void* x) {
  LinkCfgStruct* self = (LinkCfgStruct*)x;
  UDPSocket* fd = (UDPSocket*)self->fd;
  SocketAddress clientCfg;
  uint8_t recvBuf[256];
  int16_t recvNum;

  while (1) {
    recvNum = fd->recvfrom(&clientCfg, recvBuf, 256);
    if (recvNum < 0) {
      perror("recvfrom error:");
      break;
    }
    if (self->status == linkStatus_client) {
      self->recv(recvBuf, recvNum, self);
    } else {
      LinkCfgStruct rlink;
      rlink.type = linkType_udp;
      rlink.status = linkStatus_temp;
      rlink.address = (uint8_t*)clientCfg.get_addr().bytes;
      rlink.port = clientCfg.get_port();
      rlink.fd = self->fd;
      rlink.send = linkUdp_send;
      rlink.recv = fiip_recv;

      self->recv(recvBuf, recvNum, &rlink);
    }
  }
  fd->close();
  perror("LinkUdp stop");
};
