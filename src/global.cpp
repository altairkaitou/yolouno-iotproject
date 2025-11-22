#include "global.h"
//float glob_temperature = 0;
//float glob_humidity = 0;

SemaphoreHandle_t tempSemaphore = NULL;
SemaphoreHandle_t humiditySemaphore = NULL;

float lastTemp = 0;
float lastHumi = 0;


String WIFI_SSID;
String WIFI_PASS;
String CORE_IOT_TOKEN;
String CORE_IOT_SERVER;
String CORE_IOT_PORT;

String ssid = "ESP32-YOUR NETWORK HERE!!!";
String password = "12345678";
String wifi_ssid = "abcde";
String wifi_password = "123456789";
boolean isWifiConnected = false;
SemaphoreHandle_t xBinarySemaphoreInternet = xSemaphoreCreateBinary();


// ==================== Task 3: Sensor → LCD (Queue + 3 Semaphore) ============

// Queue for sending SensorPacket {temperature, humidity} from sensor task
QueueHandle_t sensorQueue = NULL;

// Three semaphores for display-state control
SemaphoreHandle_t normalSemaphore   = NULL;
SemaphoreHandle_t warningSemaphore  = NULL;
SemaphoreHandle_t criticalSemaphore = NULL;