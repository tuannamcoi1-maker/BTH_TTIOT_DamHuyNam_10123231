/**
 * DHT11 Data Sender (Optimized for Node.js Serial)
 * Định dạng: T:XX H:YY\n (Ví dụ: T:28 H:55)
 */

#include <DHT11.h>

// Kết nối cảm biến DHT11 ở chân Digital 2 (Giả định theo code cũ)
// Lưu ý: Sơ đồ Proteus (image_55da59.png) cắm DHT11 vào chân D4. 
// Tôi sẽ giữ chân D2 để phù hợp với code cũ, nhưng bạn nên kiểm tra lại.
DHT11 dht11(2); 

void setup() {
    // Khởi tạo Serial ở Baud Rate 9600 bps
    Serial.begin(9600);
    Serial.println("DHT11 Ready..."); 
}

void loop() {
    int temperature = dht11.readTemperature();
    int humidity = dht11.readHumidity(); 
    
    // Kiểm tra kết quả đọc
    if (temperature != DHT11::ERROR_CHECKSUM && temperature != DHT11::ERROR_TIMEOUT) {
        // Gửi dữ liệu ở định dạng: T:Nhiệt độ H:Độ ẩm\n
        Serial.print("T:");
        Serial.print(temperature);
        Serial.print(" H:");
        Serial.println(humidity); // println sẽ thêm \n ở cuối
    }
    
    // Đặt độ trễ giữa các lần đọc
    delay(2000); 
}