#ifndef __NEO_BLINKY_HUMIDITY_H__
#define __NEO_BLINKY_HUMIDITY_H__

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "global.h"
#include "neo_blinky.h"

// ===== Color Mapping for Humidity (Option A) =====
// <40%  → RED
// 40–70% → GREEN
// >70%  → BLUE

void neo_blinky_humidity(void *pvParameters);

#endif
