#include "./linkSerial.h"
void linkSerial_start(LinkServerStruct* self);
void linkSerial_send(LinkServerStruct* self,
                     uint8_t* data,
                     uint16_t dataLen,
                     LinkCfgStruct* link);

static void thread_recvData(void* x);

Serial* serial;

void startSerial(uint8_t* dev, uint32_t speed) {
  LinkCfgStruct* link = (LinkCfgStruct*)malloc(sizeof(LinkCfgStruct));
  LinkServerStruct* linkServer =
      (LinkServerStruct*)malloc(sizeof(LinkServerStruct));
  link->type = linkType_serial;
  link->address = (uint8_t*)malloc(strlen((char*)dev));
  memcpy(link->address, dev, strlen((char*)dev));
  link->port = speed;
  if (dev == serialType_usb) {
    serial = new Serial(USBTX, USBRX);
    /*
  } else if (dev == serialType_uart1) {
    serial = new Serial(UART1_TX, UART1_RX);
  } else if (dev == serialType_uart2) {
    serial = new Serial(UART2_TX, UART2_RX);
  } else if (dev == serialType_uart3) {
    serial = new Serial(UART3_TX, UART3_RX);
    */
  } else {
    perror("LinkSerial dev error.");
  }

  linkServer->link = link;
  linkServer->start = linkSerial_start;
  linkServer->send = linkSerial_send;
  linkServer->recv = fiip_recv;

  fiip_addServer(linkType_serial, linkServer);
  linkServer->start(linkServer);
}

void linkSerial_start(LinkServerStruct* self) {
  Thread thread;
  thread.start(callback(thread_recvData, self));
}

void linkSerial_send(LinkServerStruct* self,
                     uint8_t* data,
                     uint16_t dataLen,
                     LinkCfgStruct* dstLink) {
  uint16_t num = dataLen;
  while (dataLen) {
    serial->putc(data[num - dataLen]);
    dataLen--;
  }
}

void thread_recvData(void* x) {
  LinkServerStruct* self = (LinkServerStruct*)x;
  uint8_t recvBuf[256];
  uint16_t recvNum;

  while (1) {
    if (serial->readable()) {
      recvNum = 1;
      recvBuf[0] = serial->getc();
      LinkCfgStruct* srcLink = (LinkCfgStruct*)malloc(sizeof(LinkCfgStruct));
      srcLink->type = linkType_serial;
      srcLink->address = (uint8_t*)malloc(strlen((char*)self->link->address));
      memcpy(srcLink, self->link->address, strlen((char*)self->link->address));
      srcLink->port = self->link->port;
      self->recv(recvBuf, recvNum, srcLink);
    }
  }
  perror("LinkSerial stop");
}
