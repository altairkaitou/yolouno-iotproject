#include "led_blinky_temp.h"

// ======================= Pattern 1: Slow Blink =======================
// - LED bật 2s, tắt 2s
// - Lặp 5 lần để dễ quan sát
// - Đây là hành vi dùng khi nhiệt độ < 25°C
void slowBlink() {
    for (int i = 0; i < 5; i++) {
        digitalWrite(LED_GPIO, HIGH);
        vTaskDelay(pdMS_TO_TICKS(2000));
        digitalWrite(LED_GPIO, LOW);
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

// ======================= Pattern 2: Normal Blink ======================
// - LED bật 1s, tắt 1s
// - Lặp 5 lần để dễ quan sát
// - Áp dụng khi 25°C ≤ temperature < 30°C
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
// - LED bật/tắt nhanh (300ms) → hiệu ứng cảnh báo
// - Lặp 5 lần
// - Dùng khi nhiệt độ ≥ 30°C
void warningBlink() {
    for (int i = 0; i < 5; i++) {
        digitalWrite(LED_GPIO, HIGH);
        vTaskDelay(pdMS_TO_TICKS(300));
        digitalWrite(LED_GPIO, LOW);
        vTaskDelay(pdMS_TO_TICKS(300));
    }

    // stay ON for 1s as warning
    digitalWrite(LED_GPIO, HIGH);
    vTaskDelay(pdMS_TO_TICKS(1000));
    digitalWrite(LED_GPIO, LOW);
}

// ======================= LED Task for Task 1 ==========================
void led_blinky_temp(void *pvParameters) {

    pinMode(LED_GPIO, OUTPUT);
    digitalWrite(LED_GPIO, LOW);

    while (1) {

        // Wait for sensor data update
        if (xSemaphoreTake(tempSemaphore, portMAX_DELAY) == pdTRUE) {
            // LED task sẽ CHỜ (block) đến khi task cảm biến gọi xSemaphoreGive().
            // Điều này giúp LED không phải polling liên tục và chỉ chạy khi
            // có dữ liệu sensor mới.
            float t = glob_temperature;

            // Select blink pattern based on temperature
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
