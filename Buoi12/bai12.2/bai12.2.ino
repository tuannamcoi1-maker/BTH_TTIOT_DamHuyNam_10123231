#include <WiFi.h>           // Dùng cho ESP32
#include <PubSubClient.h>

/* ===== CẤU HÌNH (GIỮ NGUYÊN TỪ BÀI 12.1) ===== */
const char* ssid = "Mạng ko free đâu";
const char* password = "dungbatnua";
const char* mqtt_server = "192.168.244.104";
const int mqtt_port = 1883;
const char* mqtt_user = "mqtt_user";
const char* mqtt_pass = "123456";

/* TOPIC THEO MÃ SV CỦA BẠN */
const char* student_id = "10123231";
const char* topic_cmd = "iot/lab2/10123231/cmd";
const char* topic_state = "iot/lab2/10123231/state";

WiFiClient espClient;
PubSubClient client(espClient);
const int LED_PIN = 2; // Chân LED onboard của ESP32

/* === HÀM XỬ LÝ KHI CÓ LỆNH TỪ HA GỬI XUỐNG === */
void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  
  Serial.println("Lệnh nhận được: " + message);

  if (message == "ON") {
    digitalWrite(LED_PIN, HIGH);
    client.publish(topic_state, "ON"); // Báo lại cho HA là đèn đã bật
  } 
  else if (message == "OFF") {
    digitalWrite(LED_PIN, LOW);
    client.publish(topic_state, "OFF"); // Báo lại cho HA là đèn đã tắt
  }
}

void setup_wifi() {
  delay(10);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println("\nWiFi connected");
}

void reconnect() {
  while (!client.connected()) {
    String clientId = "ESP32-Lab2-10123231";
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_pass)) {
      Serial.println("MQTT connected");
      client.subscribe(topic_cmd); // Đăng ký nghe lệnh từ topic CMD
    } else {
      delay(2000);
    }
  }
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback); // Kích hoạt hàm đợi lệnh
}

void loop() {
  if (!client.connected()) { reconnect(); }
  client.loop();
}