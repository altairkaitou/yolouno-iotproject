#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

//extern float glob_temperature;
//extern float glob_humidity;
extern SemaphoreHandle_t tempSemaphore;
extern SemaphoreHandle_t humiditySemaphore;
extern float lastTemp;
extern float lastHumi;

extern String WIFI_SSID;
extern String WIFI_PASS;
extern String CORE_IOT_TOKEN;
extern String CORE_IOT_SERVER;
extern String CORE_IOT_PORT;

extern boolean isWifiConnected;
extern SemaphoreHandle_t xBinarySemaphoreInternet;

#define LED_GPIO 48     


// ======================= Task 3: Sensor → LCD (no global data) ============
// This struct is used to send sensor data through a Queue instead of globals.
typedef struct {
    float temperature;
    float humidity;
} SensorPacket;

// Queue for transferring SensorPacket from sensor task to LCD task
extern QueueHandle_t sensorQueue;

// Three semaphores for LCD display states:
// - normalSemaphore   → NORMAL state
// - warningSemaphore  → WARNING state
// - criticalSemaphore → CRITICAL state
extern SemaphoreHandle_t normalSemaphore;
extern SemaphoreHandle_t warningSemaphore;
extern SemaphoreHandle_t criticalSemaphore;

#endif
