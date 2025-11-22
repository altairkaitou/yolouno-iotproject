#ifndef __LCD_DISPLAY_TASK_H__
#define __LCD_DISPLAY_TASK_H__

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include "global.h"   

// ============================================================================
// Task 3: LCD Display Task
//
// This task receives SensorPacket (temperature + humidity) via Queue,
// and displays different LCD screens depending on which semaphore
// (normal / warning / critical) is released by the sensor task.
// ============================================================================


void lcd_display_task(void *pvParameters);

#endif
