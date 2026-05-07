#include<Servo.h>
Servo servo;

int servoPin = 9;

void setup() {
  servo.attach(servoPin);
  servo.write(0);
}

void loop() {
  servo.write(180);
  delay(2000);
  servo.write(0);
  delay(2000);
}
