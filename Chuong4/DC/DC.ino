#include <AFMotor.h>

AF_DCMotor motor(3);
int lightSensor = A0;
int threshold = 500;

void setup() {
  motor.setSpeed(150);
  Serial.begin(9600);
}

void loop() {
  int lightValue = analogRead(lightSensor);
  Serial.println(lightValue);

  if (lightValue > threshold) {
    motor.run(FORWARD);   // sáng
  } else {
    motor.run(BACKWARD);  // tối
  }

  delay(200);
}