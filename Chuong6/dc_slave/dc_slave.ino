#include <AFMotor.h>
#include <SoftwareSerial.h>


// Động cơ DC
// Khởi tạo Motor số 3 trên Motor Shield
AF_DCMotor motor(3); 

// Khởi tạo Serial ảo cho Bluetooth HC-05: RX=10, TX=11
SoftwareSerial BTSerial(10, 11); 

void setup() {
  // Thiết lập tốc độ motor (0-255)
  motor.setSpeed(200);
  motor.run(RELEASE); // Dừng motor lúc khởi động

  // Khởi động các cổng Serial
  // Lưu ý: Đổi thành 9600 nếu bạn đã thoát chế độ AT
  BTSerial.begin(38400); 
  Serial.begin(9600);

  Serial.println("--- SLAVE RECEIVER READY ---");
}

void loop() {
  // Kiểm tra xem có dữ liệu từ Bluetooth gửi đến không
  if (BTSerial.available()) {
    char cmd = BTSerial.read(); // Đọc ký tự lệnh
    Serial.print("Da nhan lenh: ");
    Serial.println(cmd);

    // Xử lý lệnh dựa trên ký tự nhận được
    if (cmd == '1') {
      // Khi nhận 'F' (Forward) từ Master
      motor.run(FORWARD);
      Serial.println("-> Motor 3: QUAY THUAN (TIEN)");
    } 
    else if (cmd == '0') {
      // Khi nhận 'R' (Reverse) từ Master
      motor.run(BACKWARD);
      Serial.println("-> Motor 3: QUAY NGUOC (LUI)");
    }
    else if (cmd == 'S') {
      // (Tùy chọn) Thêm lệnh S để dừng motor
      motor.run(RELEASE);
      Serial.println("-> Motor 3: DUNG");
    }
  }
}
