#include "temp_humi_monitor.h"
#include "global.h"

void temp_humi_monitor(void *pvParameters){

    Serial.begin(115200);

    static float fakeTemp = 20;   
    static float fakeHumi = 30;   // start at low humidity

    while (1){

        // ===== FAKE SENSOR MODE =====

        // Fake Temperature: 20 → 40 → reset
        fakeTemp += 1;
        if (fakeTemp > 40) fakeTemp = 20;

        // Fake Humidity: 30 → 80 → reset
        fakeHumi += 10;
        if (fakeHumi > 80) fakeHumi = 30;

        float temperature = fakeTemp;
        float humidity = fakeHumi;

        // ===== Update global variables =====
        glob_temperature = temperature;
        glob_humidity = humidity;

        // ===== Serial log =====
        Serial.print("[FAKE SENSOR] Humi: ");
        Serial.print(humidity);
        Serial.print("%  Temp: ");
        Serial.println(temperature);

        // ===== Notify LED + NeoPixel Tasks =====
        xSemaphoreGive(tempSemaphore);       // Task 1 (Temp)
        xSemaphoreGive(humiditySemaphore);   // Task 2 (Humidity)

        vTaskDelay(5000);   // update every 5s
    }
}
