#include "./config.h"
#include "./lib/fiip-base/fiip.h"
#include "./lib/fiip-link/linkSerial.h"
#include "./lib/fiip-link/linkUdp.h"
#include "./lib/fiip-protocol/stdp.h"
#include "./lib/riselib/flagHandle.h"
#include "mbed.h"

#define handleFlag_resetStore 1
#define handleFlag_ledSwitch 2
#define handleFlag_enterBinding 3
#define handleFlag_resetWifi 4
static uint8_t cmd_body[64];
static uint8_t cmd_getId[] = {0x70, 0x11};
static uint8_t cmd_login[] = {0x70, 0x12};
static uint8_t cmd_heart[] = {0x70, 0x13};
static uint8_t cmd_enterBinding[] = {0x70, 0x16};
static uint8_t cmd_setVarCurrent[] = {0x70, 0x21};
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
void setVar(uint8_t key, uint8_t val) {
  if (key == 0x01) {
    led_link = val;
    stdp_setVar(txd, stdp_field_cmd, cmd_setVarCurrent, 2);
    memcpy(&cmd_body[0], config.myId, 8);
    memcpy(&cmd_body[8], config.myKey, 4);
    cmd_body[12] = 0x01;
    cmd_body[13] = key;
    cmd_body[14] = 0x01;
    cmd_body[15] = val;
    stdp_setVar(txd, stdp_field_body, cmd_body, 16);
    fiip_send(txd->frame, fiip_centerId, NULL);
  }
}

void funListener0xFFFF(Stdp_FieldStruct* msg) {
  if (msg->cmd[0] & 0x80) {
    if (msg->body[2] != 0x00) {
      printf("stdp error code: %02X.\n", msg->body[2]);
      msg->cmd[0] = 0x00;
      msg->cmd[1] = 0x00;
    }
  }
}
void funListener0xF011(Stdp_FieldStruct* msg) {
  config.setMyId(&msg->body[3]);
  config.setMyKey(&msg->body[11]);
  config.setMyStatus(1, 0x89);
  system_reset();
}
void funListener0xF012(Stdp_FieldStruct* msg) {
  printf("login success.\n");
}
void funListener0xF016(Stdp_FieldStruct* msg) {
  printf("binding code is: %02X%02X%02X%02X.\n", msg->body[3], msg->body[4],
         msg->body[5], msg->body[6]);
}
void funListener0x7811(Stdp_FieldStruct* msg) {
  printf("be bound: %02X%02X%02X%02X%02X%02X%02X%02X.\n", msg->body[0],
         msg->body[1], msg->body[2], msg->body[3], msg->body[4], msg->body[5],
         msg->body[6], msg->body[7]);
  msg->cmd[0] = 0xF8;
  stdp_setVar(txd, stdp_field_cmd, msg->cmd, 2);
  memcpy(&cmd_body[0], msg->index, 2);
  cmd_body[2] = 0x00;
  stdp_setVar(txd, stdp_field_body, cmd_body, 3);
  fiip_send(txd->frame, msg->srcAddr, NULL);
}
void funListener0x7812(Stdp_FieldStruct* msg) {
  printf("be unbound: %02X%02X%02X%02X%02X%02X%02X%02X.\n", msg->body[3],
         msg->body[4], msg->body[5], msg->body[6], msg->body[7], msg->body[8],
         msg->body[9], msg->body[10]);
  msg->cmd[0] = 0xF8;
  stdp_setVar(txd, stdp_field_cmd, msg->cmd, 2);
  memcpy(&cmd_body[0], msg->index, 2);
  cmd_body[2] = 0x00;
  stdp_setVar(txd, stdp_field_body, cmd_body, 3);
  fiip_send(txd->frame, msg->srcAddr, NULL);
}
void funListener0x7822(Stdp_FieldStruct* msg) {
  if (msg->body[1] == 0x01) {
    setVar(0x01, msg->body[3]);

    msg->cmd[0] = 0xF8;
    stdp_setVar(txd, stdp_field_cmd, msg->cmd, 2);
    memcpy(&cmd_body[0], msg->index, 2);
    cmd_body[2] = 0x00;
    stdp_setVar(txd, stdp_field_body, cmd_body, 3);
    fiip_send(txd->frame, msg->srcAddr, NULL);
  }
}

void on_net_change(nsapi_event_t status, intptr_t param) {
  if (status == NSAPI_EVENT_CONNECTION_STATUS_CHANGE) {
    switch (param) {
      case NSAPI_STATUS_LOCAL_UP:
        printf("net change: connected local.\n");
        break;
      case NSAPI_STATUS_GLOBAL_UP:
        printf("net change: connected global.\n");
        led_link = 0;
        break;
      case NSAPI_STATUS_CONNECTING:
        printf("net change: connecting...\n");
        break;
      default:
        printf("net change: unconnected:%d.\n", param);
        led_link = 1;
        system_reset();
        break;
    }
  } else {
    printf("net change: unknow:%d, %d.\n", status, param);
  }
}
void on_btn_wps_fall() {
  wait(1);
  if (btn_wps == 1) {
    flagHandle_setFlag(handleFlag_ledSwitch);
  } else {
    flagHandle_setFlag(handleFlag_resetWifi);
  }
}
void on_btn_cfg_fall() {
  wait(1);
  if (btn_cfg == 1) {
    flagHandle_setFlag(handleFlag_enterBinding);
  } else {
    flagHandle_setFlag(handleFlag_resetStore);
  }
}

void handle_resetStore() {
  config.reset();
  system_reset();
}
void handle_ledSwitch() {
  setVar(0x01, !led_link);
}
void handle_enterBinding() {
  uint8_t bindingCode[4] = {0x30, 0x30, 0x30, 0x30};
  stdp_setVar(txd, stdp_field_cmd, cmd_enterBinding, 2);
  stdp_setVar(txd, stdp_field_body, bindingCode, 4);
  fiip_send(txd->frame, fiip_centerId, NULL);
}
void handle_resetWifi() {
  net->disconnect();
  net->connect();
}

int main(int argc, char* argv[]) {
  static uint8_t myIP[4] = {0x00, 0x00, 0x00, 0x00};
  static uint8_t dev[] = serialType_usb;
  printf("\n--- system is running ---\n");
  mempool_init(0x1000);
  txd = stdp_new();
  net = NetworkInterface::get_default_instance();
  net->attach(on_net_change);
  net->connect();
  config.init();
  btn_wps.fall(on_btn_wps_fall);
  btn_cfg.fall(on_btn_cfg_fall);
  flagHandle_init();
  flagHandle_addListener(handleFlag_resetStore, handle_resetStore);
  flagHandle_addListener(handleFlag_ledSwitch, handle_ledSwitch);
  flagHandle_addListener(handleFlag_enterBinding, handle_enterBinding);
  flagHandle_addListener(handleFlag_resetWifi, handle_resetWifi);
  wait(1);

  fiip_init();
  fiip_setId(config.myId);
  // startSerial(dev, 9600);
  startUdp(myIP, 16464);
  fiip_connectCloud();

  stdp_start();
  stdp_addListener(0xFFFF, funListener0xFFFF);
  stdp_addListener(0xF011, funListener0xF011);
  stdp_addListener(0xF012, funListener0xF012);
  stdp_addListener(0xF016, funListener0xF016);
  stdp_addListener(0x7811, funListener0x7811);
  stdp_addListener(0x7812, funListener0x7812);
  stdp_addListener(0x7822, funListener0x7822);

  wait(1);
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
    static uint16_t rtime = 0;
    wait(1);
    if (++rtime == 30030) {
      rtime = 0;
    }

    if (1) {
      led_status = !led_status;

      flagHandle_solve();
    }
    if (rtime % 90 == 0) {
      int8_t rssi = net->wifiInterface()->get_rssi();
      printf("net RSSI: %d.\n", rssi);
      if (rssi == 0) {
        net->disconnect();
      }

      stdp_setVar(txd, stdp_field_cmd, cmd_heart, 2);
      stdp_setVar(txd, stdp_field_body, config.myKey, 4);
      fiip_send(txd->frame, fiip_centerId, NULL);
    }
  }
  return 0;
}
