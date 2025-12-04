
#ifndef __TASK_WEBSERVER_H__
#define __TASK_WEBSERVER_H__

#include <ESPAsyncWebServer.h>
#include "LittleFS.h"
#include <AsyncTCP.h>
#include <ArduinoJson.h>
#include <ElegantOTA.h>
#include <task_handler.h>

extern AsyncWebServer server;
extern AsyncWebSocket ws;
extern SemaphoreHandle_t xBinarySemaphoreInternet; 
extern LatestSensorData latestData;
extern SemaphoreHandle_t sensorDataMutex; // Biến Mutex bảo vệ data
void Webserver_stop();
void Webserver_reconnect();
void Webserver_sendata(String data);
void websocket_send_task(void *pvParameters);
#endif