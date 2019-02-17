#ifndef HANDLE_H
#define HANDLE_H
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct {
  uint16_t index;
  void (*fun)(void);
} Handle_ListenerStruct;  //监听器结构体

void handle_addListener(uint16_t index, void (*fun)(void));
void handle_solve(void);
void handle_setFlag(uint16_t flag);
void handle_resetFlag(uint16_t flag);

#endif  // HANDLE_H
