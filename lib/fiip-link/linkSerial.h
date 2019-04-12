#ifndef __LINK_SERIAL_H
#define __LINK_SERIAL_H
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../fiip-base/fiip.h"
#include "../fiip-base/link/link.h"
#include "mbed.h"

#ifdef __cplusplus
extern "C" {
#endif

#define serialType_usb (uint8_t*)"usb"
#define serialType_uart1 (uint8_t*)"uart1"
#define serialType_uart2 (uint8_t*)"uart2"
#define serialType_uart3 (uint8_t*)"uart3"

LinkCfgStruct* linkSerial_startServer(uint8_t* dev, uint32_t speed);
LinkCfgStruct* linkSerial_startClient(uint8_t* dev, uint32_t speed);

#ifdef __cplusplus
}
#endif
#endif  //__LINK_SERIAL_H
