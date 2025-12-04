#include "coreiot.h"

// ----------- CONFIGURE THESE! -----------
const char* coreIOT_Server = "app.coreiot.io";  
const char* coreIOT_Token = "tranmanhtai";   // Device Access Token
const int   mqttPort = 1883;
// ----------------------------------------

WiFiClient espClient;
PubSubClient client(espClient);


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect (username=token, password=empty)
    client.connect("ESP32Client", coreIOT_Token, NULL);
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {
        
      Serial.println("connected to CoreIOT Server!");
      client.subscribe("v1/devices/me/rpc/request/+");
      Serial.println("Subscribed to v1/devices/me/rpc/request/+");

    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}


void callback(char* topic, byte* payload, unsigned int length) {
  String topicStr = String(topic);
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.println("] ");
  pinMode(48, OUTPUT); //Mở pin điều khiển LED
  Adafruit_NeoPixel strip(1, 45, NEO_GRB + NEO_KHZ800); //Mở điều khiển đèn NEO
  strip.begin();

  // Allocate a temporary buffer for the message
  char message[length + 1];
  memcpy(message, payload, length);
  message[length] = '\0';
  Serial.print("Payload: ");
  Serial.println(message);

  // Parse JSON
  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, message);

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }

  const char* method = doc["method"];
  String methodName = String(method);

  //Xử lý các lệnh được gửi từ server
  
  // Lệnh lấy trạng thái LED
  if (methodName == "getLEDState") {
    Serial.println("Server asking for LED state...");
    
    String requestId = topicStr.substring(26); 
    
    String responseTopic = "v1/devices/me/rpc/response/" + requestId;
    
    String responsePayload = LEDState ? "true" : "false"; 

    lastSentLEDState = LEDState;  // Cập nhật trạng thái đã gửi

    client.publish(responseTopic.c_str(), responsePayload.c_str());
    Serial.println("Replying to " + responseTopic + " with " + responsePayload);
  }
  // Lệnh điều khiển LED
  else if (strcmp(method, "setLEDState") == 0) {
    
    bool params = doc["params"]; 

    if (params == true) {
      Serial.println(">> COMMAND: Device LED turned ON");
      digitalWrite(48, HIGH);
      LEDState = true;
      
      
      client.publish("v1/devices/me/attributes", "{\"led\": true}");

    } else {   
      Serial.println(">> COMMAND: Device LED turned OFF");
      digitalWrite(48, LOW);
      LEDState = false;
      
      client.publish("v1/devices/me/attributes", "{\"led\": false}");
    }
  } 
  // Lệnh lấy trạng thái đèn NEO
  else if (methodName == "getNEOState") {
    Serial.println("Server asking for NEO state...");
    
    
    String requestId = topicStr.substring(26); 
    
    
    String responseTopic = "v1/devices/me/rpc/response/" + requestId;
    
    
    String responsePayload = NEOState ? "true" : "false"; 
    lastSentNEOState = NEOState;  // Cập nhật trạng thái đã gửi

  
    client.publish(responseTopic.c_str(), responsePayload.c_str());
    Serial.println("Replying to " + responseTopic + " with " + responsePayload);
  }
  // Lệnh điều khiển đèn NEO
  else if (strcmp(method, "setNEOState") == 0) {
    
    bool params = doc["params"]; 

    if (params == true) {
      Serial.println(">> COMMAND: Device NEO turned ON");
      strip.setPixelColor(0, strip.Color(255, 0, 0));
      strip.show();
      NEOState = true;
      
      client.publish("v1/devices/me/attributes", "{\"neo\": true}");

    } else {   
      Serial.println(">> COMMAND: Device NEO turned OFF");
      strip.setPixelColor(0, strip.Color(0, 0, 0));
      strip.show();
      NEOState = false;
    
      client.publish("v1/devices/me/attributes", "{\"neo\": false}");
    }
  } 
   else {
    Serial.print("Unknown method: ");
    Serial.println(method);
  }
}


void setup_coreiot(){

  //Serial.print("Connecting to WiFi...");
  //WiFi.begin(wifi_ssid, wifi_password);
  //while (WiFi.status() != WL_CONNECTED) {
  
  // while (isWifiConnected == false) {
  //   delay(500);
  //   Serial.print(".");
  // }

  while(1){
    if (xSemaphoreTake(xBinarySemaphoreInternet, portMAX_DELAY)) {
      break;
    }
    delay(500);
    Serial.print(".");
  }


  Serial.println(" Connected!");

  client.setServer(CORE_IOT_SERVER.c_str(), CORE_IOT_PORT.toInt());
  client.setCallback(callback);

}

void coreiot_task(void *pvParameters){

    setup_coreiot();

    while(1){

        if (!client.connected()) {
            reconnect();
        }
        client.loop();
        float currentTemp = 0.0;
        float currentHumi = 0.0;
        
        if (lastSentLEDState != LEDState) {
            // Gửi cập nhật trạng thái LED nếu có thay đổi
            if (LEDState) {
                client.publish("v1/devices/me/attributes", "{\"value\": true}");
            } else {
                client.publish("v1/devices/me/attributes", "{\"value\": false}");
            }
            lastSentLEDState = LEDState;  // Cập nhật trạng thái đã gửi
            Serial.println("Published LED state: " + String(LEDState));
        }

        if (xSemaphoreTake(sensorDataMutex, pdMS_TO_TICKS(10)) == pdTRUE) {
        currentTemp = latestData.temperature;
        currentHumi = latestData.humidity;
        xSemaphoreGive(sensorDataMutex);
        }
        // Sample payload, publish to 'v1/devices/me/telemetry'
        String payload = "{\"temperature\":" + String(currentTemp) +  ",\"humidity\":" + String(currentHumi) + "}";
        
        client.publish("v1/devices/me/telemetry", payload.c_str());


        
        //Serial.println("Published payload: " + payload);
        vTaskDelay(1000);  // Publish every 10 seconds
    }
}