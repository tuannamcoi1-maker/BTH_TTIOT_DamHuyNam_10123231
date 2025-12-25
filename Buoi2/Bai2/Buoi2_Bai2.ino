// --- ĐỊNH NGHĨA PHẦN CỨNG ---
const int buttonPin = 2; // Gán chân số 2 cho nút nhấn.
const int ledPin = 12;   // Gán chân số 12 cho đèn LED.


// --- BIẾN TOÀN CỤC ---
long tocDo = 9600; // Biến LƯU tốc độ Baud. Mặc định là 9600.
// (Lưu ý: Tốc độ PHẦN CỨNG sẽ luôn là 9600 trong code này)
enum CD { vanHanh, hieuChinh }; // Định nghĩa 2 trạng thái
CD currentState = vanHanh;   // Bắt đầu ở chế độ Vận hành

// --- BIẾN CHO LOGIC XỬ LÝ ---
unsigned long MocTimeKhiNutNhan = 0; // Lưu mốc thời gian khi nút bắt đầu được nhấn.
bool coChuyenCDHieuChinhTC = false;     // Cờ (flag) báo là hành động "nhấn giữ" đã được kích hoạt.
const long timeNhanGiu = 3000; // Thời gian nhấn giữ là 3000ms (3 giây).
unsigned long mocTimeLCGuiTN = 0; // Lưu mốc thời gian lần cuối gửi tin nhắn "Hello IoT".

// --- HÀM SETUP: CHỈ CHẠY 1 LẦN KHI KHỞI ĐỘNG ---
void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  
  // Tốc độ phần cứng BẮT ĐẦU VÀ LUÔN LUÔN là 9600
  Serial.begin(9600); 
  
  Serial.println("--- CHE DO VAN HANH ---");
  Serial.println("Nhan giu nut 3 giay de vao Cau Hinh.");
  Serial.println("");
}

// --- HÀM LOOP: CHẠY LẶP ĐI LẶP LẠI VÔ HẠN ---
void loop() {
  chuyenCheDo();
  if (currentState == vanHanh) {
    hamVanHanh();
  }
}

// --- HÀM KIỂM TRA NÚT NHẤN (ĐỂ CHUYỂN CHẾ ĐỘ) ---
void chuyenCheDo() {
  bool buttonDown = (digitalRead(buttonPin) == LOW);
  if (buttonDown) {
    if (MocTimeKhiNutNhan == 0) {
      MocTimeKhiNutNhan = millis();
    }
    if (millis() - MocTimeKhiNutNhan > timeNhanGiu && !coChuyenCDHieuChinhTC) {
      coChuyenCDHieuChinhTC = true;
      if (currentState == vanHanh) {
        cheDoHieuChinh();
      } else {
        enterOperationMode();
      }
    }
  } else {
    MocTimeKhiNutNhan = 0;
    coChuyenCDHieuChinhTC = false;
  }
}

// --- HÀM CHẠY KHI Ở CHẾ ĐỘ VẬN HÀNH ---
void hamVanHanh() {
  if (millis() - mocTimeLCGuiTN > 3000) {
    mocTimeLCGuiTN = millis();
    digitalWrite(ledPin, HIGH);
    delay(100);
    digitalWrite(ledPin, LOW);
    
    // NÓ SẼ IN RA GIÁ TRỊ CỦA BIẾN 'tocDo' (CÓ THỂ LÀ 115200)
    // NHƯNG NÓ VẪN ĐANG GỬI Ở 9600.
    Serial.print("Hello IoT. Toc do hien tai: ");
    Serial.println(tocDo);
  }
}

// --- HÀM KHI BƯỚC VÀO CHẾ ĐỘ CẤU HÌNH (QUAN TRỌNG) ---
void cheDoHieuChinh() {
  currentState = hieuChinh;
  Serial.flush(); 

  // 1. Nháy nhanh LED
  for (int i = 0; i < 10; i++) {
    digitalWrite(ledPin, !digitalRead(ledPin));
    delay(50);
  }
  digitalWrite(ledPin, LOW);

  // 2. GỬI HƯỚNG DẪN (Vẫn ở 9600)
  Serial.println(); 
  Serial.println("--- CHE DO HIEU CHINH ---");
  Serial.println("Moi chon che do: ");
  Serial.println(" 1 nhan : 9600 bps");
  Serial.println(" 2 nhan : 115200 bps");
  Serial.println("(Ban co 5 giay de chon...)");

  // 3. Bắt đầu đếm số lần nhấn nút
  int pressCount = 0;
  unsigned long configStartTime = millis();
  unsigned long configTimeout = 5000;
  
  while (millis() - configStartTime < configTimeout) {
    if (digitalRead(buttonPin) == LOW) {
      delay(50);
      pressCount++;
      while(digitalRead(buttonPin) == LOW);
      digitalWrite(ledPin, HIGH); 
      delay(50); 
      digitalWrite(ledPin, LOW);
    }
  }

  // 4. Kiểm tra kết quả đếm
  // (CHÚ Ý: Chỉ thay đổi biến 'tocDo', không thay đổi phần cứng)
  if (pressCount == 1) {
    tocDo = 9600; // THAY ĐỔI BIẾN
    digitalWrite(ledPin, HIGH); 
    delay(500); 
    digitalWrite(ledPin, LOW);
  } else if (pressCount >= 2) { 
    tocDo = 115200; // THAY ĐỔI BIẾN
    digitalWrite(ledPin, HIGH); 
    delay(250); 
    digitalWrite(ledPin, LOW); 
    delay(250);
    digitalWrite(ledPin, HIGH); 
    delay(250); 
    digitalWrite(ledPin, LOW);
  }

  // 5. Gửi thông báo xác nhận (VẪN Ở TỐC ĐỘ 9600)
  Serial.println("");
  Serial.print("Da chon toc do : ");
  Serial.println(tocDo); // Sẽ in "115200"
  Serial.println("Nhan giu nut 3s de quay ve van hanh.");
  delay(100); 

  // 6. --- [TRICK] ---
  // Chúng ta KHÔNG 'Serial.end()' và 'Serial.begin(tocDo)' nữa.
  // Chúng ta giữ nguyên tốc độ 9600 của phần cứng.
  
  MocTimeKhiNutNhan = 0;
  coChuyenCDHieuChinhTC = false;
}

// --- HÀM KHI QUAY VỀ CHẾ ĐỘ VẬN HÀNH ---
void enterOperationMode() {
  currentState = vanHanh;
  
  // --- [TRICK] ---
  // KHÔNG TẮT VÀ MỞ LẠI SERIAL.
  // Chỉ gửi thông báo (vẫn ở 9600).
  
  Serial.println();
  Serial.println("--- CHE DO VAN HANH ---");
  Serial.println("Nhan giu nut 3 giay de vao Cau Hinh.\n");
  Serial.println("");
  
  MocTimeKhiNutNhan = 0;
  coChuyenCDHieuChinhTC = false;
}