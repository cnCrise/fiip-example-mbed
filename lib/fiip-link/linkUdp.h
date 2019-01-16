#ifndef __LINK_UDP_H
#define __LINK_UDP_H
#ifdef __cplusplus
extern "C" {
#endif

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../fiip-base/fiip.h"
#include "../fiip-base/link/link.h"

#define linkType_udp 0x30

void startUdp(uint8_t* ip, uint32_t port);

#ifdef __cplusplus
}
#endif
#endif  //__LINK_UDP_H
