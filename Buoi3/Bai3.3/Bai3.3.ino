#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

// ---------------- CẤU HÌNH PHẦN CỨNG ----------------

// Loại module dùng: PAROLA_HW (phù hợp module 8x8 thông dụng)
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW

// Số lượng module (mỗi MAX7219 điều khiển 1 ma trận 8x8)
#define MAX_DEVICES 4

// Chân kết nối
#define CLK_PIN   13   // CLK
#define DATA_PIN  11   // DIN
#define CS_PIN    10   // LOAD

// Khởi tạo đối tượng Parola
MD_Parola P = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

// ---------------- THIẾT LẬP HIỂN THỊ ----------------

uint8_t scrollSpeed = 40;         // Tốc độ chạy
textEffect_t scrollEffect = PA_SCROLL_LEFT;  // Hiệu ứng chạy
textPosition_t scrollAlign = PA_CENTER;      // Căn giữa
uint16_t scrollPause = 0;         // Dừng giữa các lần lặp (ms)

// Bộ đệm tin nhắn
#define BUF_SIZE 100
char curMessage[BUF_SIZE] = { "" };
char newMessage[BUF_SIZE] = { "HELLO IOT!" };
bool newMessageAvailable = true;

// ---------------- HÀM ĐỌC DỮ LIỆU SERIAL ----------------
void readSerial(void)
{
  static char *cp = newMessage;

  while (Serial.available())
  {
    *cp = (char)Serial.read();
    if ((*cp == '\n') || (*cp == '\r') || (cp - newMessage >= BUF_SIZE - 2))
    {
      *cp = '\0'; // Kết thúc chuỗi
      cp = newMessage;
      newMessageAvailable = true;
    }
    else
      cp++;
  }
}

// ---------------- SETUP ----------------
void setup()
{
  Serial.begin(9600);
  Serial.println("[Parola LED Matrix]");
  Serial.println("Nhap chuoi va nhan Enter de hien thi...");

  P.begin();
  P.displayText(curMessage, scrollAlign, scrollSpeed, scrollPause, scrollEffect, scrollEffect);
}

// ---------------- LOOP ----------------
void loop()
{
  if (P.displayAnimate()) // Khi kết thúc 1 vòng cuộn
  {
    if (newMessageAvailable)
    {
      strcpy(curMessage, newMessage);
      newMessageAvailable = false;
    }
    P.displayReset(); // Hiển thị lại với dữ liệu mới
  }

  readSerial(); // Đọc dữ liệu nhập từ terminal
}
