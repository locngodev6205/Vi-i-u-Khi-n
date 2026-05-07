#include <AFMotor.h>

AF_DCMotor motor(3);

// HC-SR04 pins
// const int trigPin = 9;
// const int echoPin = 10;

const int trigPin = A1;
const int echoPin = A2;

void setup() {
  motor.setSpeed(150);
  Serial.begin(9600);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {
  // Gửi tín hiệu trigger
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Đọc thời gian phản hồi
  long duration = pulseIn(echoPin, HIGH);

  // Tính khoảng cách (cm)
  int distance = duration * 0.034 / 2;

  Serial.print("Distance: ");
  Serial.println(distance);

  if (distance > 20) {
    motor.run(FORWARD);   // xa → chạy thuận
  } else {
    motor.run(BACKWARD);  // gần → chạy nghịch
  }

  delay(200);
}