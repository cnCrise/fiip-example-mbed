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

#define serialType_usb "usb"
#define serialType_uart1 "uart1"
#define serialType_uart2 "uart2"
#define serialType_uart3 "uart3"

void startSerial(uint8_t* dev, uint32_t speed);

#ifdef __cplusplus
}
#endif
#endif  //__LINK_SERIAL_H
