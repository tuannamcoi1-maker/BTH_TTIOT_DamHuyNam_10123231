#include <WiFi.h>
#include <WiFiUdp.h>
#include <coap-simple.h>

// Ép kiểu về COAP_TYPE để thư viện chấp nhận
#define MY_NON (COAP_TYPE)1 

/* ===== CẤU HÌNH CỦA BẠN ===== */
const char* WIFI_SSID = "Mạng ko free đâu";
const char* WIFI_PASS = "dungbatnua";

IPAddress coap_server_ip(192, 168, 244, 241); 
const int coap_port = 5683;
const char* coap_resource = "sensor/temp";

WiFiUDP udp;
Coap coap(udp);
unsigned long lastSend = 0;

void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println("\nWiFi Connected!");
  coap.start();
}

void loop() {
  if (millis() - lastSend >= 10000) { 
    lastSend = millis();
    String payload = "temp=30.5_MSV_10123231"; 

    Serial.println("Dang gui goi tin NON...");
    
    coap.send(
      coap_server_ip,            
      coap_port,                 
      coap_resource,             
      MY_NON,                    // Sử dụng MY_NON đã được ép kiểu COAP_TYPE
      COAP_POST,                 
      NULL,                      
      0,                         
      (uint8_t*)payload.c_str(), 
      payload.length(),          
      COAP_TEXT_PLAIN            
    );
  }
  coap.loop();
}