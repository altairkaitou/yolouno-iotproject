#include "lcd_display_task.h"


LiquidCrystal_I2C lcd(33, 16, 2);


// ============================================================================
// LCD DISPLAY TASK
// - Nhận SensorPacket từ Queue (temperature + humidity)
// - Chờ semaphore trạng thái NORMAL / WARNING / CRITICAL
// - Hiển thị
// ============================================================================

void lcd_display_task(void *pvParameters)
{
    lcd.begin();
    lcd.backlight();

    SensorPacket data;   // buffer chứa temperature + humidity

    while (1)
    {
        // --------------------------------------------------------------------
        // 1) Nhận dữ liệu từ Queue 
        // --------------------------------------------------------------------
        if (xQueueReceive(sensorQueue, &data, portMAX_DELAY) == pdTRUE)
        {
            float temp = data.temperature;
            float humi = data.humidity;

            // ----------------------------------------------------------------
            // 2) Kiểm tra semaphore để biết trạng thái nào cần hiển thị
            //    (chỉ 1 semaphore được give mỗi chu kỳ sensor)
            // ----------------------------------------------------------------

            // ===== NORMAL STATE =====
            if (xSemaphoreTake(normalSemaphore, 0) == pdTRUE)
            {
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Temp: ");
                lcd.print(temp, 1);
                lcd.print("C");

                lcd.setCursor(0, 1);
                lcd.print("Humi: ");
                lcd.print(humi, 1);
                lcd.print("%  OK");

                Serial.println("[LCD] NORMAL state displayed");
            }

            // ===== WARNING STATE =====
            else if (xSemaphoreTake(warningSemaphore, 0) == pdTRUE)
            {
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Temp: ");
                lcd.print(temp, 1);
                lcd.print("C");

                lcd.setCursor(0, 1);
                lcd.print("Humi: ");
                lcd.print(humi, 1);
                lcd.print("% WARN");

                Serial.println("[LCD] WARNING state displayed");
            }

            // ===== CRITICAL STATE =====
            else if (xSemaphoreTake(criticalSemaphore, 0) == pdTRUE)
            {
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Temp: ");
                lcd.print(temp, 1);
                lcd.print("C");

                lcd.setCursor(0, 1);
                lcd.print("CRITICAL !!!");

                Serial.println("[LCD] CRITICAL state displayed");
            }

            // Nếu không semaphore nào → không đổi LCD
        }
    }
}
