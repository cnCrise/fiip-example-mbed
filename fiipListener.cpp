#include "./fiipListener.h"

void onError(uint8_t err) {
  printf("stdp error code: %02X.\n", err);
}
void onGetId(uint8_t* id, uint8_t* key) {
  config.setMyId(id);
  config.setMyKey(key);
  config.setMyStatus(1, 0x89);
  system_reset();
}
void onLogin() {
  printf("login success.\n");

  setVar(0x01, 0);
  fiipCloud_subActualAllVar(config.myId, config.myKey);
  // fiipCloud_subTargetAllVar(config.myId, config.myKey);
}
void onEnterBinding(uint8_t* key) {
  printf("binding code is: %02X%02X%02X%02X.\n", key[0], key[1], key[2],
         key[3]);
}
void onBeBound(uint8_t* id, uint8_t* key) {
  printf("be bound: %02X%02X%02X%02X%02X%02X%02X%02X.\n", id[0], id[1], id[2],
         id[3], id[4], id[5], id[6], id[7]);
}
void onBeUnbound(uint8_t* id) {
  printf("be unbound: %02X%02X%02X%02X%02X%02X%02X%02X.\n", id[0], id[1], id[2],
         id[3], id[4], id[5], id[6], id[7]);
}

void onTargetChange(uint8_t* id, CloudVarStruct* var) {
  printf("cloud target: %d change %d.\n", var->keyVal[0], var->targetVal[0]);
  setVar(0x01, var->targetVal[0]);
}

void fiipListener_start() {
  fiipCloud_on(CloudEvent_error, onError);
  fiipCloud_on(cloudEvent_getId, onGetId);
  fiipCloud_on(cloudEvent_login, onLogin);
  fiipCloud_on(cloudEvent_enterBinding, onEnterBinding);
  fiipCloud_on(cloudEvent_beBound, onBeBound);
  fiipCloud_on(cloudEvent_beUnbound, onBeUnbound);
  fiipCloudVar_onTargetChange(config.myId, kVal_led, kLen_led, onTargetChange);
}
