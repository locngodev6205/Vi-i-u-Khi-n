#include <SoftwareSerial.h>
#include <Servo.h>

Servo myServo;
const int SERVO_PIN = 4;
SoftwareSerial BTSerial(10, 11); 

char currentMode = 'S';

void setup() {
  Serial.begin(9600);
  BTSerial.begin(9600); 
  
  myServo.attach(SERVO_PIN);
  myServo.write(0);
}

void loop() {
  if (BTSerial.available()) {
    char newChar = BTSerial.read(); 
    
    if (newChar == 'A' || newChar == 'B' || newChar == 'S') {
      currentMode = newChar; 
      Serial.print("CMD : ");
      Serial.println(currentMode);
    }
    else {
      Serial.print("Canh bao: Ky tu khong hop le -> ");  Serial.println(newChar);
    }
  }

  if (currentMode == 'A') {
    myServo.write(90);
    delay(500);
    myServo.write(0);
    delay(500);
  } 
  else if (currentMode == 'B') {
    myServo.write(180);
    delay(500);
    myServo.write(0);
    delay(500);
  }
  else if (currentMode == 'S') {
    myServo.write(0);
  }
}