#include "neo_blinky_humidity.h"
#include "global.h"

Adafruit_NeoPixel strip(LED_COUNT, NEO_PIN, NEO_GRB + NEO_KHZ800);

void neo_blinky_humidity(void *pvParameters) {

    strip.begin();
    strip.clear();
    strip.show();

    SensorPacket packet;

    while (1) {
        // Wait for new humidity data from sensor task
        if (xSemaphoreTake(humiditySemaphore, portMAX_DELAY) == pdTRUE) {

            // Get the latest humidity packet
            xQueueReceive(sensorQueue, &packet, portMAX_DELAY);

            float hum = packet.humidity;

            Serial.print("[Neo] Humidity = ");
            Serial.println(hum);

            // Humidity → Color mapping (aligned with Task 3 states)
            if (hum < 70) {
                strip.setPixelColor(0, strip.Color(0, 255, 0));  // GREEN
            }
            else if (hum < 85) {
                strip.setPixelColor(0, strip.Color(255, 180, 0)); // YELLOW
            }
            else {
                strip.setPixelColor(0, strip.Color(255, 0, 0));  // RED
            }

            strip.show();
        }
    }
}
