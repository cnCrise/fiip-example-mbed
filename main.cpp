#include "./config.h"
#include "./lib/fiip-base/fiip.h"
#include "./lib/fiip-link/linkSerial.h"
#include "./lib/fiip-link/linkUdp.h"
#include "./lib/fiip-protocol/stdp.h"
#include "mbed.h"
static uint8_t cmd_body[64];
static uint8_t cmd_getId[] = {0x70, 0x11};
static uint8_t cmd_login[] = {0x70, 0x12};
static uint8_t cmd_heart[] = {0x70, 0x13};
static InterruptIn btn_wps(PB_3);
static InterruptIn btn_cfg(PC_5);
static DigitalOut led_status(PA_3, 1);
static DigitalOut led_link(PA_5, 1);
static StdpStruct* txd;
static NetworkInterface* net;

void fiip_connectCloud() {
  char* hostname = "0.fiip.fengdid.cn";
  SocketAddress hostAddress;

  if (net->gethostbyname(hostname, &hostAddress, NSAPI_IPv4) != 0) {
    perror("DNS error");
  } else {
    LinkCfgStruct* centerServer = (LinkCfgStruct*)malloc(sizeof(LinkCfgStruct));
    centerServer->type = linkType_udp;
    centerServer->address = (uint8_t*)malloc(4);
    memcpy(centerServer->address, hostAddress.get_addr().bytes, 4);
    centerServer->port = 16464;
    fiip_connect(fiip_centerId, centerServer);
    printf("cloud is:%d.%d.%d.%d,%d.\n", centerServer->address[0],
           centerServer->address[1], centerServer->address[2],
           centerServer->address[3], centerServer->port);
    free(centerServer->address);
    free(centerServer);
  }
}

void funListener0xF011(Stdp_FieldStruct* msg) {
  if (msg->body[2] == 0x00) {
    config.setMyId(&msg->body[3]);
    config.setMyKey(&msg->body[11]);
    config.setMyStatus(1, 0x89);
  } else {
    printf("getId err:%02X.\n", msg->body[2]);
  }
  system_reset();
}
void funListener0xF012(Stdp_FieldStruct* msg) {
  if (msg->body[2] == 0x00) {
    printf("login success.\n");
  } else {
    printf("login err:%02X.\n", msg->body[2]);
  }
}
void funListener0xFFFF(Stdp_FieldStruct* msg) {
  printf("funListener0xFFFF recv.\n");
}

void on_btn_wps_fall() {
  config.reset();
}
void on_btn_cfg_fall() {
  led_status = !led_status;
}
int main(int argc, char* argv[]) {
  static uint8_t myIP[4] = {0x00, 0x00, 0x00, 0x00};
  static uint8_t dev[] = serialType_usb;
  printf("\n--- system is running ---\n");
  txd = stdp_new();
  net = NetworkInterface::get_default_instance();
  btn_wps.fall(&on_btn_wps_fall);
  btn_cfg.fall(&on_btn_cfg_fall);

  config.init();
  fiip_init();
  fiip_setId(config.myId, config.myKey);
  // startSerial(dev, 9600);
  startUdp(myIP, 16464);

  fiip_connectCloud();

  stdp_start();
  stdp_addListener(0xFFFF, funListener0xFFFF);
  stdp_addListener(0xF011, funListener0xF011);
  stdp_addListener(0xF012, funListener0xF012);

  if (config.myStatus[1] != 0x89) {
    memcpy(&cmd_body[0], config.myTypeId, 4);
    memcpy(&cmd_body[4], config.myTypeKey, 4);
    stdp_setVar(txd, stdp_field_cmd, cmd_getId, 2);
    stdp_setVar(txd, stdp_field_body, cmd_body, 8);
    fiip_send(txd->frame, fiip_centerId, NULL);
    while (1) {
      wait(90);
    }
  }
  stdp_setVar(txd, stdp_field_cmd, cmd_login, 2);
  stdp_setVar(txd, stdp_field_body, config.myKey, 4);
  fiip_send(txd->frame, fiip_centerId, NULL);

  while (1) {
    wait(90);

    if (net->get_connection_status() == NSAPI_STATUS_GLOBAL_UP) {
      led_link = 0;
    } else {
      led_link = 1;
    }
    led_status = !led_status;

    stdp_setVar(txd, stdp_field_cmd, cmd_heart, 2);
    stdp_setVar(txd, stdp_field_body, config.myKey, 4);
    fiip_send(txd->frame, fiip_centerId, NULL);
  }
  return 0;
}
