#include<Servo.h>
Servo servo;

int VRx = A0;
int VRy = A1;
int SW  = 2;

int servoPin = 9;

void setup() {
  servo.attach(servoPin);
  servo.write(0);

  pinMode(SW, INPUT_PULLUP);
}

void loop() {
  int xValue = analogRead(VRx);
  int yValue = analogRead(VRy);
  int button = digitalRead(SW);

  // trái
  if (xValue < 300) {
    servo.write(180);
  }

  // phải
  if (xValue > 700) {
    servo.write(0);
  }

  // lên
  if (yValue > 700) {
    servo.write(90);
  }

  // xuống
  if (yValue < 300) {
    servo.write(90);
  }

  // nhấn joystick
  if (button == LOW) {

    servo.write(0);
    delay(300);
    servo.write(180);
    delay(300);
    servo.write(0);
  }

  delay(100);

}
