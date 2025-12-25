#include <WiFi.h>               
#include <WiFiClientSecure.h>   
#include <PubSubClient.h>
#include <ArduinoJson.h>

/* ====== CẤU HÌNH ====== */
const char* ssid = "LAPTOP-4FB6A1P1 0840";           
const char* password = "hello moi nguoi";     

// IP Home Assistant (Broker)
const char* mqtt_server = "test.mosquitto.org"; 
const int mqtt_port = 8883;                 
const char* mqtt_user = "mqtt-user";
const char* mqtt_pass = "123456";
const char* mqtt_topic = "iot/lab14/sensor";

// Khởi tạo Client bảo mật
WiFiClientSecure espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");

  // QUAN TRỌNG: Bỏ qua kiểm tra chứng chỉ cho ESP32
  espClient.setInsecure(); 
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTTS connection...");
    
    // Sửa lỗi tạo clientId cho ESP32
    String id = "ESP32-Secure-" + String(random(0xffff), HEX);
    const char* clientId = id.c_str();
    
    if (client.connect(clientId)) {
      Serial.println("connected (Encrypted)");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  
  // Thiết lập Server và Port 8883
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Tạo và gửi dữ liệu JSON
  StaticJsonDocument<200> doc;
  doc["temp"] = random(250, 350) / 10.0;
  doc["hum"] = random(400, 800) / 10.0;
  
  char payload[128];
  serializeJson(doc, payload);

  Serial.print("Publishing Secure message: ");
  Serial.println(payload);
  
  // Gửi dữ liệu qua kênh đã mã hóa TLS
  client.publish(mqtt_topic, payload);
  
  delay(5000);
}