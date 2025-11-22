#include "led_blinky_temp.h"
#include "global.h"

// ======================= Pattern 1: Slow Blink =======================
void slowBlink() {
    for (int i = 0; i < 5; i++) {
        digitalWrite(LED_GPIO, HIGH);
        vTaskDelay(pdMS_TO_TICKS(2000));
        digitalWrite(LED_GPIO, LOW);
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

// ======================= Pattern 2: Normal Blink ======================
void normalBlink() {
    for (int i = 0; i < 5; i++) {
        digitalWrite(LED_GPIO, HIGH);
        vTaskDelay(pdMS_TO_TICKS(1000));
        digitalWrite(LED_GPIO, LOW);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    vTaskDelay(pdMS_TO_TICKS(600));
}

// ======================= Pattern 3: Warning Blink =====================
void warningBlink() {
    for (int i = 0; i < 5; i++) {
        digitalWrite(LED_GPIO, HIGH);
        vTaskDelay(pdMS_TO_TICKS(300));
        digitalWrite(LED_GPIO, LOW);
        vTaskDelay(pdMS_TO_TICKS(300));
    }
    digitalWrite(LED_GPIO, HIGH);
    vTaskDelay(pdMS_TO_TICKS(1000));
    digitalWrite(LED_GPIO, LOW);
}

// ======================= LED Task (Task 1) ============================
void led_blinky_temp(void *pvParameters) {

    pinMode(LED_GPIO, OUTPUT);
    digitalWrite(LED_GPIO, LOW);

    while (1) {

        // Chờ tín hiệu
        if (xSemaphoreTake(tempSemaphore, portMAX_DELAY) == pdTRUE) {

            float t = lastTemp;   // lấy từ cache (không dùng queue)

            if (t < 25.0f) {
                slowBlink();
            }
            else if (t < 30.0f) {
                normalBlink();
            }
            else {
                warningBlink();
            }
        }
    }
}



