#include "./lib/fiip-base/fiip.h"
#include "./lib/fiip-link/linkSerial.h"
#include "./lib/fiip-link/linkUdp.h"
#include "./lib/fiip-protocol/stdp.h"
#include "mbed.h"
DigitalOut led1(LED1);

uint8_t myId[8] = {0x00, 0x00, 0x04, 0x01, 0x00, 0x00, 0x00, 0x01};
uint8_t myKey[4] = {0x95, 0x71, 0xF9, 0xAE};

void fiip_connectCloud() {
  NetworkInterface* net = NetworkInterface::get_default_instance();
  char* hostname = "0.fiip.fengdid.cn";
  SocketAddress* hostAddress;

  if (net->gethostbyname(hostname, hostAddress, NSAPI_IPv4) != 0) {
    perror("DNS error");
  } else {
    LinkCfgStruct* centerServer = (LinkCfgStruct*)malloc(sizeof(LinkCfgStruct));
    centerServer->type = linkType_udp;
    centerServer->address = (uint8_t*)malloc(4);
    memcpy(centerServer->address, hostAddress->get_addr().bytes, 4);
    centerServer->port = 16464;
    fiip_connect(fiip_centerId, centerServer);
    printf("cloud is:%d.%d.%d.%d,%d.\n", centerServer->address[0],
           centerServer->address[1], centerServer->address[2],
           centerServer->address[3], centerServer->port);
  }
}

void funListener0xFFFF(Stdp_FieldStruct* msg) {
  printf("funListener0xFFFF recv.\n");
}

int main(int argc, char* argv[]) {
  static uint8_t myIP[4] = {0x00, 0x00, 0x00, 0x00};
  static uint8_t dev[] = serialType_usb;

  fiip_init();
  fiip_setId(myId, myKey);
  startSerial(dev, 9600);
  startUdp(myIP, 16464);

  fiip_connectCloud();

  stdp_start();
  stdp_addListener(0xFFFF, funListener0xFFFF);

  StdpStruct* txd = stdp_new();
  stdp_setVar(txd, stdp_field_body, dev, 12);
  fiip_send(txd->frame, myId, NULL);

  while (1) {
    led1 = !led1;
    wait_ms(500);
  }
  return 0;
}
