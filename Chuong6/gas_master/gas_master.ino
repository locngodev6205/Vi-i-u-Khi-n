#include <SoftwareSerial.h>

// RX=10, TX=11 (Nối với TX, RX của HC-05)
SoftwareSerial BTSerial(10, 11);

const int gasSensor = A0; // Chân analog nối với cảm biến khí Gas (MQ sensor)
int threshold = 400;      // Ngưỡng phát hiện gas (tùy chỉnh tùy môi trường)
int lastState = -1;       // Biến lưu trạng thái trước đó

void setup() {
  // QUAN TRỌNG: Đổi thành 9600 nếu đã cấu hình xong HC-05 để truyền dữ liệu
  BTSerial.begin(38400); 
  Serial.begin(9600);
  
  Serial.println("--- MASTER GAS SENSOR READY ---");
  Serial.println("Dang khoi dong cam bien (Can khoang 20-60s de lam nong)...");
}

void loop() {
  int gasValue = analogRead(gasSensor); // Đọc giá trị từ cảm biến gas
  int currentStatus;

  // Logic phân loại: Nếu nồng độ gas vượt ngưỡng
  if (gasValue < threshold) {
    currentStatus = 0; // Trạng thái AN TOÀN (Bình thường)
  } else {
    currentStatus = 1; // Trạng thái NGUY HIỂM (Phát hiện Gas/Khói)
  }

  // Chỉ gửi lệnh khi trạng thái thay đổi
  if (currentStatus != lastState) {
    
    if (currentStatus == 0) {
      BTSerial.print('1'); // Gửi 'F' khi An toàn
      Serial.print("Gia tri Gas: "); Serial.print(gasValue);
      Serial.println(" -> AN TOAN -> Gui 'F'");
    } 
    else {
      BTSerial.print('0'); // Gửi 'R' khi có Gas
      Serial.print("Gia tri Gas: "); Serial.print(gasValue);
      Serial.println(" -> PHAT HIEN GAS! -> Gui 'R'");
    }
    
    lastState = currentStatus; 
  }

  delay(100); // Tăng delay một chút để đọc giá trị ổn định hơn
}
