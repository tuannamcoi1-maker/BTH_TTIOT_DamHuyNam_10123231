#include <WiFi.h>
#include <PubSubClient.h>

/* ===== CẤU HÌNH (GIỮ NGUYÊN) ===== */
const char* ssid = "Mạng ko free đâu";
const char* password = "dungbatnua";
const char* mqtt_server = "192.168.244.104";
const int mqtt_port = 1883;
const char* mqtt_user = "mqtt_user";
const char* mqtt_pass = "123456";
const char* student_id = "10123231";

WiFiClient espClient;
PubSubClient client(espClient);

/* ===== TOPICS TỰ ĐỘNG ===== */
String topic_cmd = "iot/lab3/10123231/cmd";
String topic_state = "iot/lab3/10123231/state";
String topic_lwt = "iot/lab3/10123231/lwt";

const int LED_PIN = 2;
bool ledState = false;

void applyLed(bool on) {
  ledState = on;
  digitalWrite(LED_PIN, on ? HIGH : LOW);
  // Gửi trạng thái kèm RETAIN = true để Broker ghi nhớ
  client.publish(topic_state.c_str(), ledState ? "ON" : "OFF", true);
}

void callback(char* topic, byte* payload, unsigned int length) {
  String msg;
  for (unsigned int i = 0; i < length; i++) msg += (char)payload[i];
  msg.trim();
  Serial.println("Lệnh nhận: " + msg);

  if (msg == "ON") applyLed(true);
  else if (msg == "OFF") applyLed(false);
  else if (msg == "TOGGLE") applyLed(!ledState);
}

void reconnect() {
  while (!client.connected()) {
    String clientId = "ESP32-Lab3-10123231";
    // Kết nối kèm LWT: Nếu ESP32 mất kết nối, Broker tự gửi "OFFLINE"
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_pass, topic_lwt.c_str(), 0, true, "OFFLINE")) {
      Serial.println("Connected!");
      client.subscribe(topic_cmd.c_str());
      client.publish(topic_lwt.c_str(), "ONLINE", true); // Báo đang Online
    } else {
      delay(2000);
    }
  }
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();
}