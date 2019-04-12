#include "./main.h"

#define handleFlag_resetStore 1
#define handleFlag_ledSwitch 2
#define handleFlag_enterBinding 3
#define handleFlag_resetWifi 4
InterruptIn btn_wps(PB_3);
InterruptIn btn_cfg(PC_5);
DigitalOut led_status(PA_3, 1);
DigitalOut led_link(PA_5, 1);
NetworkInterface* net;

void fiip_connectCloud() {
  char* hostname = "fiip.crise.cn";
  SocketAddress hostAddress;

  if (net->gethostbyname(hostname, &hostAddress, NSAPI_IPv4) != 0) {
    perror("DNS error");
  } else {
    LinkCfgStruct* link =
        linkUdp_startClient(hostAddress.get_addr().bytes, 16464);
    fiip_connect(fiip_centerId, link);
    printf("cloud is:%d.%d.%d.%d,%d.\n", link->address[0], link->address[1],
           link->address[2], link->address[3], link->port);
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
  setVar(0x01, !led_status);
}
void handle_enterBinding() {
  uint8_t bindingCode[4] = {0x30, 0x30, 0x30, 0x30};
  fiipCloud_enterBinding(bindingCode);
}
void handle_resetWifi() {
  net->disconnect();
  net->connect();
}

int main(int argc, char* argv[]) {
  static uint8_t* dev = serialType_usb;
  printf("\n--- system is running ---\n");
  net = NetworkInterface::get_default_instance();
  net->attach(on_net_change);
  net->connect();
  config.init();
  btn_wps.fall(on_btn_wps_fall);
  btn_cfg.fall(on_btn_cfg_fall);
  wait(1);

  mempool_init(0x1000);
  flagHandle_init();
  flagHandle_addListener(handleFlag_resetStore, handle_resetStore);
  flagHandle_addListener(handleFlag_ledSwitch, handle_ledSwitch);
  flagHandle_addListener(handleFlag_enterBinding, handle_enterBinding);
  flagHandle_addListener(handleFlag_resetWifi, handle_resetWifi);

  fiip_init();
  fiip_setId(config.myId);
  fiipListener_start();
  // linkSerial_startServer(dev, 9600);
  fiip_connectCloud();
  fiipCloud_init(config.myId, config.myKey);

  wait(1);
  if (config.myStatus[1] != 0x89) {
    fiipCloud_getId(config.myTypeId, config.myTypeKey);
    while (1) {
      wait(90);
    }
  }
  fiipCloud_login();

  while (1) {
    static uint16_t rtime = 0;
    wait(1);
    if (++rtime == 30030) {
      rtime = 0;
    }

    if (1) {
      flagHandle_solve();
    }
    if (rtime % 90 == 0) {
      int8_t rssi = net->wifiInterface()->get_rssi();
      printf("net RSSI: %d.\n", rssi);
      if (rssi == 0) {
        net->disconnect();
      }

      fiipCloud_heart();
    }
  }
  return 0;
}
