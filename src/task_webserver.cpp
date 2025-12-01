#include "task_webserver.h"

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

bool webserver_isrunning = false;

void Webserver_sendata(String data)
{
    if (ws.count() > 0)
    {
        ws.textAll(data); // Gửi đến tất cả client đang kết nối
        Serial.println("📤 Đã gửi dữ liệu qua WebSocket: " + data);
    }
    else
    {
        Serial.println("⚠️ Không có client WebSocket nào đang kết nối!");
    }
}
String getSensorJSON() {
    float currentTemp = 0.0f;
    float currentHumi = 0.0f;

    // Lấy Mutex để truy cập dữ liệu an toàn
    if (xSemaphoreTake(sensorDataMutex, pdMS_TO_TICKS(10)) == pdTRUE) {
        currentTemp = latestData.temperature;
        currentHumi = latestData.humidity;
        xSemaphoreGive(sensorDataMutex);
    } else {
        // Trả về rỗng nếu không lấy được khóa (để tránh lỗi)
        Serial.println("⚠️ [WS] Không lấy được Mutex khi đọc sensor!");
        return ""; 
    }

    // Đóng gói JSON theo format Frontend mong đợi: 
    // {"page":"sensor", "value":{"temperature":X, "humidity":Y}}
    StaticJsonDocument<128> doc;
    doc["page"] = "sensor";
    doc["value"]["temperature"] = serialized(String(currentTemp, 1)); 
    doc["value"]["humidity"] = serialized(String(currentHumi, 1));
    
    String payload;
    serializeJson(doc, payload);
    return payload;
}

// ✅ TASK GỬI DỮ LIỆU ĐỊNH KỲ
void websocket_send_task(void *pvParameters) {
    Serial.println("🌐 [WS Task] Đang chờ kết nối Internet...");
    // Chặn Task cho đến khi Wifi kết nối thành công lần đầu
    // xSemaphoreTake(xBinarySemaphoreInternet, portMAX_DELAY);
    // xSemaphoreGive(xBinarySemaphoreInternet); 

    while (1) {
        // Chỉ gửi nếu có client và WebServer đang chạy
        // if (webserver_isrunning && ws.count() > 0) {
            String sensorData = getSensorJSON();
            if (sensorData.length() > 0) {
                 Webserver_sendata(sensorData);
            }
        // }
        vTaskDelay(pdMS_TO_TICKS(2000)); // Gửi mỗi 2 giây
    }
}
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
    if (type == WS_EVT_CONNECT)
    {
        Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
    }
    else if (type == WS_EVT_DISCONNECT)
    {
        Serial.printf("WebSocket client #%u disconnected\n", client->id());
    }
    else if (type == WS_EVT_DATA)
    {
        AwsFrameInfo *info = (AwsFrameInfo *)arg;

        if (info->opcode == WS_TEXT)
        {
            String message;
            message += String((char *)data).substring(0, len);
            // parseJson(message, true);
            handleWebSocketMessage(message);
        }
    }
}

void connnectWSV()
{
    ws.onEvent(onEvent);
    server.addHandler(&ws);
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(LittleFS, "/index.html", "text/html"); });
    server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(LittleFS, "/script.js", "application/javascript"); });
    server.on("/styles.css", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(LittleFS, "/styles.css", "text/css"); });
              

    server.on("/raphael.min.js", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(LittleFS, "/raphael.min.js", "application/javascript"); });

    server.on("/justgage.min.js", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(LittleFS, "/justgage.min.js", "application/javascript"); });
    server.begin();
    ElegantOTA.begin(&server);
    webserver_isrunning = true;
}

void Webserver_stop()
{
    ws.closeAll();
    server.end();
    webserver_isrunning = false;
}

void Webserver_reconnect()
{
    if (!webserver_isrunning)
    {
        connnectWSV();
    }
    ElegantOTA.loop();
}
