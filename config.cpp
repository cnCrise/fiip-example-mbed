#include "config.h"
Config config;
static NVStore& nvstore = NVStore::get_instance();

void Config::init() {
  uint16_t actual_len_bytes = 0;
  static uint8_t myTypeId[TypeIdLen] = {0x00, 0x00, 0x02, 0x01};  //智能模块
  static uint8_t myTypeKey[KeyLen] = {0x2A, 0x64, 0xE8, 0xDA};

  if (nvstore.init() != NVSTORE_SUCCESS) {
    perror("nvstore init");
  }
  nvstore.get(0, myStatusLen, this->myStatus, actual_len_bytes);
  if (this->myStatus[0] != 0x9A) {
    this->reset();
  }
  memcpy(this->myTypeId, myTypeId, TypeIdLen);
  memcpy(this->myTypeKey, myTypeKey, KeyLen);
  if (nvstore.get(1, IdLen, this->myId, actual_len_bytes) != NVSTORE_SUCCESS) {
    printf("myId is none.\n");
    uint8_t* uid = (uint8_t*)malloc(1);
    this->myId[0] = (((int)uid) >> 24) | 0x80;
    this->myId[1] = ((int)uid) >> 16;
    this->myId[2] = ((int)uid) >> 8;
    this->myId[3] = ((int)uid) >> 0;
    this->myId[4] = ((int)&uid) >> 24;
    this->myId[5] = ((int)&uid) >> 16;
    this->myId[6] = ((int)&uid) >> 8;
    this->myId[7] = ((int)&uid) >> 0;
    free(uid);
  }
  nvstore.get(2, KeyLen, this->myKey, actual_len_bytes);

  printf("myId:%02X%02X%02X%02X%02X%02X%02X%02X.\n", this->myId[0],
         this->myId[1], this->myId[2], this->myId[3], this->myId[4],
         this->myId[5], this->myId[6], this->myId[7]);
  printf("myKey:%02X%02X%02X%02X.\n", this->myKey[0], this->myKey[1],
         this->myKey[2], this->myKey[3]);
}
void Config::reset() {
  this->setMyStatus(0, 0x00);
  this->setMyStatus(1, 0x00);
  if (nvstore.reset() != NVSTORE_SUCCESS) {
    perror("nvstore reset");
  }else{
    printf("nvstore reset success.\n");
  }
  this->setMyStatus(0, 0x9A);
}

void Config::setMyStatus(uint8_t index, uint8_t status) {
  this->myStatus[index] = status;
  if (nvstore.set(0, myStatusLen, this->myStatus) != NVSTORE_SUCCESS) {
    perror("nvstore setMyStatus");
  }
}
void Config::setMyId(uint8_t* id) {
  memcpy(this->myId, id, IdLen);
  if (nvstore.set(1, IdLen, this->myId) != NVSTORE_SUCCESS) {
    perror("nvstore setMyId");
  }
}
void Config::setMyKey(uint8_t* key) {
  memcpy(this->myKey, key, KeyLen);
  if (nvstore.set(2, KeyLen, this->myKey) != NVSTORE_SUCCESS) {
    perror("nvstore setMyKey");
  }
}
