#include <WiFi.h>           // Thư viện WiFi cho ESP32
#include <PubSubClient.h>   // Thư viện MQTT
#include <ArduinoJson.h>    // Thư viện xử lý dữ liệu JSON

/* ====== CẤU HÌNH THÔNG SỐ (ĐÃ CẬP NHẬT) ====== */
const char* ssid = "Mạng ko free đâu";
const char* password = "dungbatnua";
const char* mqtt_server = "192.168.244.104"; // IP Home Assistant
const int mqtt_port = 1883;
const char* mqtt_user = "mqtt_user";      
const char* mqtt_pass = "123456";         

// Đã cập nhật mã số sinh viên của bạn vào topic
const char* mqtt_topic = "iot/lab1/10123231/sensor"; 
/* =========================================== */

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected!");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Tên định danh duy nhất cho mạch để tránh xung đột
    if (client.connect("ESP32_HuyNam_10123231", mqtt_user, mqtt_pass)) { 
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 2 seconds");
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Tạo dữ liệu giả lập nhiệt độ/độ ẩm theo định dạng JSON
  StaticJsonDocument<200> doc;
  doc["temp"] = random(250, 350) / 10.0; 
  doc["hum"] = random(400, 800) / 10.0;  

  char payload[128];
  serializeJson(doc, payload);

  // Gửi dữ liệu (Publish) lên Home Assistant
  Serial.print("Publish message: ");
  Serial.println(payload);
  client.publish(mqtt_topic, payload);

  delay(5000); // Gửi dữ liệu mỗi 5 giây
}