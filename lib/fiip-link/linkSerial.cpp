#include "./linkSerial.h"
void linkSerial_send(uint8_t* data, uint16_t dataLen, LinkCfgStruct* link);

static Thread thread;
static void thread_recvData(void* x);

LinkCfgStruct* linkSerial_startServer(uint8_t* dev, uint32_t speed) {
  LinkCfgStruct* link = link_new(NULL);
  link->type = linkType_serial;
  link->status = linkStatus_server;
  mem_cpy(link->address, dev, strlen((char*)dev));
  link->port = speed;
  if (dev == serialType_usb) {
    link->fd = new Serial(USBTX, USBRX);
    /*
  } else if (dev == serialType_uart1) {
    link->fd = new Serial(UART1_TX, UART1_RX);
  } else if (dev == serialType_uart2) {
    link->fd = new Serial(UART2_TX, UART2_RX);
  } else if (dev == serialType_uart3) {
    link->fd = new Serial(UART3_TX, UART3_RX);
    */
  } else {
    perror("LinkSerial dev error.");
  }
  link->send = linkSerial_send;
  link->recv = fiip_recv;

  thread.start(callback(thread_recvData, link));
  return link;
}
LinkCfgStruct* linkSerial_startClient(uint8_t* dev, uint32_t speed) {
  LinkCfgStruct* link = link_new(NULL);
  link->type = linkType_serial;
  link->status = linkStatus_client;
  mem_cpy(link->address, dev, strlen((char*)dev));
  link->port = speed;
  if (dev == serialType_usb) {
    link->fd = new Serial(USBTX, USBRX);
    /*
  } else if (dev == serialType_uart1) {
    link->fd = new Serial(UART1_TX, UART1_RX);
  } else if (dev == serialType_uart2) {
    link->fd = new Serial(UART2_TX, UART2_RX);
  } else if (dev == serialType_uart3) {
    link->fd = new Serial(UART3_TX, UART3_RX);
    */
  } else {
    perror("LinkSerial dev error.");
  }
  link->send = linkSerial_send;
  link->recv = fiip_recv;

  thread.start(callback(thread_recvData, link));
  return link;
}

void linkSerial_send(uint8_t* data, uint16_t dataLen, LinkCfgStruct* rlink) {
  uint16_t num = dataLen;
  while (num) {
    ((Serial*)rlink->fd)->putc(data[dataLen - num]);
    num--;
  }
}

void thread_recvData(void* x) {
  LinkCfgStruct* self = (LinkCfgStruct*)x;
  Serial* fd = (Serial*)self->fd;
  uint8_t recvBuf[256];
  uint16_t recvNum;

  while (1) {
    if (fd->readable()) {
      recvNum = 1;
      recvBuf[0] = fd->getc();
      self->recv(recvBuf, recvNum, self);
    }
  }
  perror("LinkSerial stop");
}
