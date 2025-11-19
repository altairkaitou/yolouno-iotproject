#include "temp_humi_monitor.h"
#include "global.h"

DHT20 dht20;
LiquidCrystal_I2C lcd(33,16,2);

void temp_humi_monitor(void *pvParameters){

    Wire.begin(11, 12);
    Serial.begin(115200);
    dht20.begin();

    

    while (1){

        // ===== Read DHT20 real sensor =====
        dht20.read();
        float temperature = dht20.getTemperature();
        float humidity    = dht20.getHumidity();

        // ===== Check for error =====
        if (isnan(temperature) || isnan(humidity)) {
            Serial.println("Failed to read from DHT20 sensor!");
            temperature = humidity = -1;
        }

        // ===== Update global variable for Task 1 and Task 2 =====
        glob_temperature = temperature;
        glob_humidity    = humidity;

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
