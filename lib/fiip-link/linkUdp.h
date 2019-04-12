#ifndef __LINK_UDP_H
#define __LINK_UDP_H
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../fiip-base/fiip.h"
#include "../fiip-base/link/link.h"
#include "EthernetInterface.h"
#include "mbed.h"

#ifdef __cplusplus
extern "C" {
#endif

LinkCfgStruct* linkUdp_startServer(uint8_t* ip, uint32_t port);
LinkCfgStruct* linkUdp_startClient(uint8_t* ip, uint32_t port);

#ifdef __cplusplus
}
#endif
#endif  //__LINK_UDP_H
