#include <IRremote.hpp>
#include <Stepper.h>

const int stepPerRev = 2048; 
Stepper myStepper(stepPerRev, 8, 10, 9, 11);

const int IR_RECEIVE_PIN = 3;

// TIMEOUT
unsigned long lastReceiveTime = 0;  
const unsigned long TIMEOUT_MS = 500; 

void setup() {
  Serial.begin(9600);
  
  myStepper.setSpeed(15);
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
}

void loop() {
  if (IrReceiver.decode()) {
    uint16_t receivedAddress = IrReceiver.decodedIRData.address;
    unsigned long receivedCommand = IrReceiver.decodedIRData.command;
    
    Serial.print("CMD : ");
    Serial.println(receivedCommand, HEX);

    if (receivedAddress == 0x00) {
      if (receivedCommand == 0x69) {
        myStepper.step(stepPerRev / 4); 
        delay(100);

        lastReceiveTime = millis(); 
      }
      else if (receivedCommand == 0x96) {
        myStepper.step(-(stepPerRev / 8)); 
        delay(100);

        lastReceiveTime = millis(); 
      }
      
    } else {
      Serial.println("Canh bao: Sai dia chi!");
    }

    IrReceiver.resume(); 
  }
  if (millis() - lastReceiveTime > TIMEOUT_MS) {
    digitalWrite(8, LOW);
    digitalWrite(9, LOW);
    digitalWrite(10, LOW);
    digitalWrite(11, LOW);
  }
}