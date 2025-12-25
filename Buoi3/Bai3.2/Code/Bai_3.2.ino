#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>

RTC_DS1307 rtc;
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Các chân nút nhấn
#define btnSet     2
#define btnMove    3
#define btnUp      4
#define btnDown    5
#define buzzer     6

int setMode = 0;
int viTriChon = 0; // 0: giờ, 1: phút
int gioBao = 6, phutBao = 0;
bool baoThucBat = false;

// === BIẾN MỚI ĐỂ CHỐNG CHẶN ===
bool dangKeu = false; // Trạng thái cho biết còi có đang kêu hay không
unsigned long thoiDiemBatDauKeu = 0; // Mốc thời gian bắt đầu kêu
// =============================

void setup() {
  pinMode(btnSet, INPUT_PULLUP);
  pinMode(btnMove, INPUT_PULLUP);
  pinMode(btnUp, INPUT_PULLUP);
  pinMode(btnDown, INPUT_PULLUP);
  pinMode(buzzer, OUTPUT);

  lcd.init();
  lcd.backlight();
  rtc.begin();

  if (!rtc.isrunning()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  lcd.setCursor(0, 0);
  lcd.print("DONG HO BAO THUC");
  delay(1000);
  lcd.clear();
}

void loop() {
  DateTime now = rtc.now();

  // Luôn kiểm tra báo thức, dù ở bất kỳ chế độ nào
  // Hàm này KHÔNG còn "chặn" nữa
  kiemTraBaoThuc(now); 

  // Nhấn nút Cài đặt (Logic này có thể bị ảnh hưởng nếu nhấn lúc đang kêu)
  // Nhưng vì kiemTraBaoThuc cũng kiểm tra nút Set nên sẽ ổn
  if (digitalRead(btnSet) == LOW && !dangKeu) { // Thêm kiểm tra "dangKeu"
    delay(300);
    setMode++;
    if (setMode > 2) setMode = 0;
  }

  // Chỉ hiển thị và cài đặt nếu còi không kêu
  // (Ưu tiên xử lý báo thức)
  if (!dangKeu) {
    if (setMode == 0) {
      hienThiGiaoDien(now);
    } else if (setMode == 1) {
      caiDatBaoThuc();
    } else if (setMode == 2) {
      baoThucBat = !baoThucBat;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Bao thuc: ");
      lcd.print(baoThucBat ? "BAT" : "TAT");
      delay(1000);
      setMode = 0;
    }
  } else {
    // Nếu đang kêu, ưu tiên hiển thị màn hình báo thức
    lcd.setCursor(0, 0);
    lcd.print("!!! BAO THUC !!!");
    lcd.setCursor(0, 1);
    lcd.print("Nhan nut de tat");
  }
}


void hienThiGiaoDien(DateTime now) {
  lcd.setCursor(0, 0);
  lcd.print("Time: ");
  hienThi2ChuSo(now.hour());
  lcd.print(":");
  hienThi2ChuSo(now.minute());
  lcd.print(":");
  hienThi2ChuSo(now.second());

  lcd.setCursor(0, 1);
  lcd.print("Alarm ");
  hienThi2ChuSo(gioBao);
  lcd.print(":");
  hienThi2ChuSo(phutBao);
  lcd.print(baoThucBat ? " ON " : " OFF");
}

// === HÀM KIỂM TRA BÁO THỨC ĐÃ SỬA (KHÔNG CHẶN) ===
void kiemTraBaoThuc(DateTime now) {
  // Dùng 'static' để biến này giữ giá trị, chỉ reset khi qua phút mới
  static bool daKeuTrongPhutNay = false;

  // ----- 1. KIỂM TRA ĐỂ BẮT ĐẦU KÊU -----
  // Điều kiện: Báo thức BẬT, CHƯA kêu, VÀ giờ/phút khớp
  if (baoThucBat && !dangKeu && !daKeuTrongPhutNay &&
      now.hour() == gioBao && now.minute() == phutBao)
  {
    dangKeu = true;           // Bật trạng thái "đang kêu"
    daKeuTrongPhutNay = true; // Đánh dấu là đã kêu (để không kêu lặp lại)
    thoiDiemBatDauKeu = millis(); // Ghi lại mốc thời gian
    tone(buzzer, 1000);       // Bật còi (chỉ 1 lần)
    lcd.clear();              // Xóa màn hình để chuẩn bị hiển thị cảnh báo
  }

  // ----- 2. KIỂM TRA ĐỂ TẮT (NẾU CÒI ĐANG KÊU) -----
  if (dangKeu) {
    // 2a. Tắt do đã kêu đủ 60 giây
    if (millis() - thoiDiemBatDauKeu > 2000) {
      dangKeu = false;
      noTone(buzzer); // Tắt còi
      lcd.clear();    // Xóa màn hình cảnh báo
      setMode = 0;    // Quay về màn hình chính
    }

    // 2b. Tắt do người dùng nhấn nút bất kỳ
    if (digitalRead(btnSet) == LOW || digitalRead(btnMove) == LOW ||
        digitalRead(btnUp) == LOW || digitalRead(btnDown) == LOW)
    {
      dangKeu = false;
      noTone(buzzer); // Tắt còi
      baoThucBat = false; // Tắt luôn chế độ báo thức
      lcd.clear();    // Xóa màn hình cảnh báo
      setMode = 0;    // Quay về màn hình chính

      // Chờ một chút để tránh việc nhấn 1 lần mà bị nhận nhiều lần
      delay(300);
    }
  }

  // ----- 3. RESET CỜ ĐỂ CHUẨN BỊ CHO PHÚT SAU -----
  // Khi thời gian đã qua phút báo thức, reset cờ 'daKeu'
  if (now.minute() != phutBao) {
    daKeuTrongPhutNay = false;
  }
}
// ===============================================


void caiDatBaoThuc() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("CAI DAT BAO THUC");

  while (true) { // Vòng lặp này vẫn "chặn", nhưng chấp nhận được
    lcd.setCursor(0, 1);
    hienThi2ChuSo(gioBao);
    lcd.print(":");
    hienThi2ChuSo(phutBao);
    lcd.print("      "); // Xóa phần còn lại của dòng

    // Chớp vị trí đang chọn
    if (viTriChon == 0) lcd.setCursor(0, 1);
    else lcd.setCursor(3, 1);
    lcd.blink();

    // Nút di chuyển (qua lại giữa giờ và phút)
    if (digitalRead(btnMove) == LOW) {
      delay(250);
      viTriChon = !viTriChon;
    }

    // Nút tăng
    if (digitalRead(btnUp) == LOW) {
      delay(200);
      if (viTriChon == 0) {
        gioBao++;
        if (gioBao > 23) gioBao = 0;
      } else {
        phutBao++;
        if (phutBao > 59) phutBao = 0;
      }
    }

    // Nút giảm
    if (digitalRead(btnDown) == LOW) {
      delay(200);
      if (viTriChon == 0) {
        gioBao--;
        if (gioBao < 0) gioBao = 23;
      } else {
        phutBao--;
        if (phutBao < 0) phutBao = 59;
      }
    }

    // Nhấn nút Cài đặt lần nữa để lưu và thoát
    if (digitalRead(btnSet) == LOW) {
      delay(300);
      lcd.noBlink();
      lcd.clear();
      baoThucBat = true; // Tự động bật báo thức sau khi cài
      setMode = 0;
      return; // Thoát khỏi hàm
    }
  }
}

void hienThi2ChuSo(int x) {
  if (x < 10) lcd.print("0");
  lcd.print(x);
}