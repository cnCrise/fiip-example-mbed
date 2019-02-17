#include "handle.h"

static uint32_t handle_flags;
#define LISTENERS_LEN_MAX 64
static Handle_ListenerStruct* Listeners[LISTENERS_LEN_MAX];
static uint16_t ListenersLen;

void handle_addListener(uint16_t index, void (*fun)(void)) {
  if (ListenersLen < LISTENERS_LEN_MAX) {
    Listeners[ListenersLen] =
        (Handle_ListenerStruct*)malloc(sizeof(Handle_ListenerStruct));
    Listeners[ListenersLen]->index = index;
    Listeners[ListenersLen]->fun = fun;
    ListenersLen++;
  }
}
void handle_solve(void) {
  for (uint16_t i = 0; i < ListenersLen; i++) {
    if ((1 << Listeners[i]->index) & handle_flags) {
      Listeners[i]->fun();
      handle_resetFlag(Listeners[i]->index);
    }
  }
}
void handle_setFlag(uint16_t flag) {
  handle_flags |= (1 << flag);
}
void handle_resetFlag(uint16_t flag) {
  handle_flags &= ~(1 << flag);
}

