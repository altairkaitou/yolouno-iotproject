#include "global.h"

#include "led_blinky.h"
#include "neo_blinky.h"
#include "temp_humi_monitor.h"
// #include "mainserver.h"
// #include "tinyml.h"
#include "coreiot.h"

// include task
#include "task_check_info.h"
#include "task_toogle_boot.h"
#include "task_wifi.h"
#include "task_webserver.h"
#include "task_core_iot.h"
#include "led_blinky_temp.h"
#include "neo_blinky_humidity.h"
#include "lcd_display_task.h"

void setup()
{
  Serial.begin(115200);
  check_info_File(0);

  Wire.begin(11, 12);


  // ============== Create Semaphore ========= TASK 1 //
   tempSemaphore = xSemaphoreCreateBinary();
  if (tempSemaphore == NULL) {
      Serial.println("Failed to create tempSemaphore!");
      while(1) { delay(1000); }
  }


  // ============== Create Semaphore TASK 2 ======= //
  humiditySemaphore = xSemaphoreCreateBinary();
if (humiditySemaphore == NULL) {
    Serial.println("Failed to create humiditySemaphore!");
    while(1) { delay(1000); }
}


  // Task 3: Create Queue
  // Queue length = 5 packets, each packet = SensorPacket struct
  // ============================
  sensorQueue = xQueueCreate(5, sizeof(SensorPacket));
  if (sensorQueue == NULL)
  {
      Serial.println("Failed to create sensorQueue!");
      while(1) { delay(1000); }
  }

  // Task 3: Create 3 Semaphores

  normalSemaphore   = xSemaphoreCreateBinary();
  warningSemaphore  = xSemaphoreCreateBinary();
  criticalSemaphore = xSemaphoreCreateBinary();

  if (!normalSemaphore || !warningSemaphore || !criticalSemaphore)
  {
      Serial.println("Failed to create LCD State Semaphores!");
      while(1) { delay(1000); }
  }

  //xTaskCreate(led_blinky, "Task LED Blink", 2048, NULL, 2, NULL);
  //xTaskCreate(neo_blinky, "Task NEO Blink", 2048, NULL, 2, NULL);
  xTaskCreate(neo_blinky_humidity, "Neo Humidity", 4096, NULL, 2, NULL);
  xTaskCreate(led_blinky_temp, "Task LED Temp", 4096, NULL, 2, NULL);
  xTaskCreate(lcd_display_task, "LCD Display", 4096, NULL, 2, NULL);
  xTaskCreate(temp_humi_monitor, "Task TEMP HUMI Monitor", 2048, NULL, 2, NULL);
  // xTaskCreate(main_server_task, "Task Main Server" ,8192  ,NULL  ,2 , NULL);
  // xTaskCreate( tiny_ml_task, "Tiny ML Task" ,2048  ,NULL  ,2 , NULL);
  xTaskCreate(coreiot_task, "CoreIOT Task" ,4096  ,NULL  ,2 , NULL);
  // xTaskCreate(Task_Toogle_BOOT, "Task_Toogle_BOOT", 4096, NULL, 2, NULL);

}

void loop()
{
  if (check_info_File(1))
  {
    if (!Wifi_reconnect())
    {
      Webserver_stop();
    }
    else
    {
      //CORE_IOT_reconnect();
    }
  }
  Webserver_reconnect();
}