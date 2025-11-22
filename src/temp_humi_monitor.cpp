#include "temp_humi_monitor.h"
#include "global.h"

DHT20 dht20;


void temp_humi_monitor(void *pvParameters){

    Wire.begin(11, 12);
    Serial.begin(115200);
    dht20.begin();

    

    while (1){

        // ===== Read DHT20 real sensor =====
        dht20.read();
        float temperature = dht20.getTemperature();
        float humidity    = dht20.getHumidity();
        lastTemp = temperature;
        lastHumi = humidity;

        // ===== Check for error =====
        if (isnan(temperature) || isnan(humidity)) {
            Serial.println("Failed to read from DHT20 sensor!");
            temperature = humidity = -1;
        }




         // =============================
        // Task 3: Send SensorPacket via Queue
        // =============================
        SensorPacket packet;
        packet.temperature = temperature;
        packet.humidity    = humidity;

        xQueueSend(sensorQueue, &packet, 0);

         // =============================
        // Task 3: Determine state (Option C)
        // =============================
        bool stateNormal   = (temperature < 30 && humidity < 70);
        bool stateWarning  = ((temperature >= 30 && temperature < 40) ||
                              (humidity >= 70 && humidity < 85));
        bool stateCritical = (temperature >= 40 || humidity >= 85);

        // Raise semaphore based on state
        if (stateNormal)
        {
            xSemaphoreGive(normalSemaphore);
            Serial.println("[SensorTask] NORMAL state triggered");
        }
        else if (stateWarning)
        {
            xSemaphoreGive(warningSemaphore);
            Serial.println("[SensorTask] WARNING state triggered");
        }
        else if (stateCritical)
        {
            xSemaphoreGive(criticalSemaphore);
            Serial.println("[SensorTask] CRITICAL state triggered");
        }

        // ===== Serial Debug =====
        Serial.print("[REAL SENSOR] Humi: ");
        Serial.print(humidity);
        Serial.print("%  Temp: ");
        Serial.println(temperature);


        // ===== Notify Tasks =====
        xSemaphoreGive(tempSemaphore);       // Task 1 (LED Temp)
        xSemaphoreGive(humiditySemaphore);   // Task 2 (NeoPixel Humi)

        vTaskDelay(pdMS_TO_TICKS(2000));     // Sensor read period
    }
}
