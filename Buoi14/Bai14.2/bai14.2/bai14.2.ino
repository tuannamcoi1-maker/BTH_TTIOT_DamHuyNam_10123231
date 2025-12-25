#include <WiFi.h>               
#include <PubSubClient.h>       
#include <ArduinoJson.h>

/* ====== CẤU HÌNH ====== */
const char* ssid = "Mạng ko free đâu";           
const char* password = "dungbatnua";     

const char* mqtt_server = "192.168.244.104"; 
const int mqtt_port = 1883;         // Cổng không mã hóa
const char* mqtt_user = "mqtt-user";
const char* mqtt_pass = "123456";
const char* mqtt_topic = "iot/lab14/sensor";

// FIX LỖI 1: Dùng WiFiClient thường cho cổng 1883
WiFiClient espClient; 
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  Serial.println("\nConnecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection (1883)...");
    
    // Tạo ID ngẫu nhiên cho Client
    String id = "ESP32-Client-" + String(random(0xffff), HEX);
    
    // FIX LỖI 2: Truyền đầy đủ User và Password vào lệnh connect
    if (client.connect(id.c_str(), mqtt_user, mqtt_pass)) { 
      Serial.println("connected");
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
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  StaticJsonDocument<200> doc;
  doc["temp"] = random(250, 350) / 10.0;
  doc["hum"] = random(400, 800) / 10.0;
  
  char payload[128];
  serializeJson(doc, payload);

  Serial.print("Publishing Plaintext: ");
  Serial.println(payload);
  
  client.publish(mqtt_topic, payload);
  
  delay(5000);
}