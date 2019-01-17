#include "./linkUdp.h"
void linkUdp_start(LinkServerStruct* self);
void linkUdp_send(LinkServerStruct* self,
                  uint8_t* data,
                  uint16_t dataLen,
                  LinkCfgStruct* link);

static void thread_recvData(void* x);

NetworkInterface* net;
UDPSocket sock;

void startUdp(uint8_t* ip, uint32_t port) {
  LinkCfgStruct* link = (LinkCfgStruct*)malloc(sizeof(LinkCfgStruct));
  LinkServerStruct* linkServer =
      (LinkServerStruct*)malloc(sizeof(LinkServerStruct));
  link->type = linkType_udp;
  link->address = (uint8_t*)malloc(4);
  memcpy(link->address, ip, 4);
  link->port = port;

  linkServer->link = link;
  linkServer->start = linkUdp_start;
  linkServer->send = linkUdp_send;
  linkServer->recv = fiip_recv;

  fiip_addServer(linkType_udp, linkServer);
  linkServer->start(linkServer);
}

void linkUdp_start(LinkServerStruct* self) {
  LinkCfgStruct* link = self->link;

  net = NetworkInterface::get_default_instance();
  if (!net) {
    printf("Error! No network inteface found.\n");
  }

  if (0 != net->connect()) {
    printf("Error connecting\n");
  }
  sock.open(net);

  /* 绑定socket */
  if (sock.bind(link->port) < 0) {
    perror("bind error:");
  }

  Thread thread;
  thread.start(callback(thread_recvData, self));
}

void linkUdp_send(LinkServerStruct* self,
                  uint8_t* data,
                  uint16_t dataLen,
                  LinkCfgStruct* dstLink) {
  SocketAddress clientCfg(dstLink->address, NSAPI_IPv4, dstLink->port);

  if (sock.sendto(clientCfg, data, dataLen) < 0) {
    printf("Error sending data\n");
  }
}

void thread_recvData(void* x) {
  LinkServerStruct* self = (LinkServerStruct*)x;
  SocketAddress clientCfg;
  uint8_t recvBuf[256];
  uint16_t recvNum;

  while (1) {
    recvNum = sock.recvfrom(&clientCfg, recvBuf, 256);
    if (recvNum < 0) {
      perror("recvfrom error:");
      break;
    }
    LinkCfgStruct* srcLink = (LinkCfgStruct*)malloc(sizeof(LinkCfgStruct));
    srcLink->type = linkType_udp;
    srcLink->address = (uint8_t*)malloc(4);
    memcpy(srcLink->address, (uint8_t*)clientCfg.get_addr().bytes, 4);
    srcLink->port = clientCfg.get_port();
    self->recv(recvBuf, recvNum, srcLink);
  }
  sock.close();
  net->disconnect();
  perror("LinkUdp stop");
}
