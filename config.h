#ifndef CONFIG_H
#define CONFIG_H
#include "mbed.h"
#include "nvstore.h"

class Config {
#define myStatusLen 2
#define TypeIdLen 4
#define IdLen 8
#define KeyLen 4
 public:
  uint8_t myStatus[myStatusLen];
  uint8_t myTypeId[TypeIdLen];
  uint8_t myTypeKey[KeyLen];
  uint8_t myId[IdLen];
  uint8_t myKey[KeyLen];

  Config(){};
  void init();
  void reset();
  void setMyStatus(uint8_t index, uint8_t status);
  void setMyId(uint8_t* id);
  void setMyKey(uint8_t* key);
};
extern Config config;
#endif  // CONFIG_H
