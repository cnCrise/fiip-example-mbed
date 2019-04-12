#ifndef VAR_H
#define VAR_H
#include <stdint.h>

extern uint8_t cmd_body[64];
extern uint8_t kLen_led;
extern uint8_t kVal_led[1];
extern uint8_t vLen_led;
extern uint8_t vVal_led[1];

void setVar(uint8_t key, uint8_t val);

#endif  // VAR_H
