#include "neo_blinky_humidity.h"


Adafruit_NeoPixel strip(LED_COUNT, NEO_PIN, NEO_GRB + NEO_KHZ800);

/* ============================================================
   HUMIDITY → COLOR MAPPING (Task 2 - Option A)

   Humidity < 40%        → RED
   Humidity 40% – 70%    → GREEN
   Humidity > 70%        → BLUE

   - RED    = Mức ẩm thấp (không khí khô)
   - GREEN  = Mức ẩm bình thường / tối ưu
   - BLUE   = Mức ẩm cao (cảnh báo ẩm)
   ============================================================ */
void neo_blinky_humidity(void *pvParameters) {

    strip.begin();
    strip.clear();
    strip.show();

    while (1) {

        // ---------------------------------------------------------------
        // CHỜ tín hiệu từ temp_humi_monitor
        // humiditySemaphore sẽ được "give" mỗi khi có dữ liệu mới
        // ---------------------------------------------------------------
        if (xSemaphoreTake(humiditySemaphore, portMAX_DELAY) == pdTRUE) {

            float h = glob_humidity;

            // Debug ra serial để dễ theo dõi
            Serial.print("Humidity received: ");
            Serial.println(h);

            

            if (h < 40.0f) {
                strip.setPixelColor(0, strip.Color(255, 0, 0));  // RED
                Serial.println("NeoPixel: RED (Low Humidity)");
            }
            else if (h <= 70.0f) {
                strip.setPixelColor(0, strip.Color(0, 255, 0));  // GREEN
                Serial.println("NeoPixel: GREEN (Normal Humidity)");
            }
            else {
                strip.setPixelColor(0, strip.Color(0, 0, 255));  // BLUE
                Serial.println("NeoPixel: BLUE (High Humidity)");
            }

            strip.show();

        } // end semaphore take
    } // end while(1)
}
