#ifndef MAIN_H
#define MAIN_H
#include "./config.h"
#include "./fiipListener.h"
#include "./lib/fiip-base/fiip.h"
#include "./lib/fiip-cloud/fiipCloud.h"
#include "./lib/fiip-link/linkSerial.h"
#include "./lib/fiip-link/linkUdp.h"
#include "./lib/fiip-protocol/stdp.h"
#include "./lib/riselib/flagHandle.h"
#include "./var.h"
#include "mbed.h"

extern InterruptIn btn_wps;
extern InterruptIn btn_cfg;
extern DigitalOut led_status;
extern DigitalOut led_link;
extern NetworkInterface* net;

#endif  // MAIN_H
