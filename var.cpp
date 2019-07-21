#include "./var.h"
#include "./main.h"

uint8_t cmd_body[64];
uint8_t kLen_led = 1;
uint8_t kVal_led[1] = {0x01};
uint8_t vLen_led = 1;
uint8_t vVal_led[1] = {0x00};

void setVar(uint8_t key, uint8_t val) {
  if (key == 0x01) {
    led_status = !val;

    vVal_led[0] = val;
    fiipCloud_setActualVar(config.myId, config.myKey, kVal_led, kLen_led,
                           vVal_led, vLen_led);
  }
}