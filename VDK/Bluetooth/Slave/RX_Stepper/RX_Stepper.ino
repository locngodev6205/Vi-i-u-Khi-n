#include <SoftwareSerial.h>
#include <Stepper.h>

const int stepPerRev = 2048; 
Stepper myStepper(stepPerRev, 2, 4, 3, 5);
SoftwareSerial BTSerial(10, 11); 

char currentMode = 'S';

void setup() {
  Serial.begin(9600);

  myStepper.setSpeed(15);
  BTSerial.begin(9600); 
}

void loop() {
  if (BTSerial.available()) {
    char newChar = BTSerial.read();
    if (newChar == 'A' || newChar == 'B' || newChar == 'S') {
      currentMode = newChar; 
      Serial.print("CMD : ");
      Serial.println(currentMode);
    }
  }

  if (currentMode == 'A') {
    myStepper.step(stepPerRev / 4); 
    delay(100);
  } 
  else if (currentMode == 'B') {
    myStepper.step(-(stepPerRev / 8)); 
    delay(100);
  }
  else if (currentMode == 'S'){
    
  }
}