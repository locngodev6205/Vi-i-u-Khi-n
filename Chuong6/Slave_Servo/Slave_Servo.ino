#include <SoftwareSerial.h>
#include <Servo.h>

Servo myServo;
const int SERVO_PIN = 6;
SoftwareSerial BTSerial(10, 11); 

char currentMode = 'S';

void setup() {
  Serial.begin(9600);
  BTSerial.begin(38400); 
  
  myServo.attach(SERVO_PIN);
  myServo.write(0);
}

void loop() {
  if (BTSerial.available()) {
    char newChar = BTSerial.read(); 
    
    if (newChar == '1' || newChar == '0' || newChar == 'S') {
      currentMode = newChar; 
      Serial.print("CMD : ");
      Serial.println(currentMode);
    }
    else {
      Serial.print("Canh bao: Ky tu khong hop le -> ");  Serial.println(newChar);
    }
  }

  if (currentMode == '1') {
    myServo.write(60);
    delay(500);
    myServo.write(0);
    delay(500);
  } 
  else if (currentMode == '0') {
    myServo.write(120);
    delay(500);
    myServo.write(0);
    delay(500);
  }
  else if (currentMode == 'S') {
    myServo.write(0);
  }
}
