#ifndef __LED_BLINKY_TEMP_H__
#define __LED_BLINKY_TEMP_H__

#include <Arduino.h>
#include "global.h"

// ===== Pattern functions for 3 temperature levels =====
void slowBlink();
void normalBlink();
void warningBlink();

// ===== Task for LED blink based on temperature =====
void led_blinky_temp(void *pvParameters);

#endif
