#include <AFMotor.h>

AF_DCMotor motor(3);

// Chân cảm biến ánh sáng (LDR)
const int ldrPin = A1;  

void setup() {
  motor.setSpeed(150);
  Serial.begin(9600);
}

void loop() {
  // Đọc giá trị ánh sáng (0 - 1023)
  int lightValue = analogRead(ldrPin);

  Serial.print("Light: ");
  Serial.println(lightValue);

  // Ngưỡng (bạn có thể chỉnh lại)
  int threshold = 500;

  if (lightValue > threshold) {
    // sáng → chạy thuận
    motor.run(FORWARD);
  } else {
    // tối → chạy ngược
    motor.run(BACKWARD);
  }

  delay(200);
}
