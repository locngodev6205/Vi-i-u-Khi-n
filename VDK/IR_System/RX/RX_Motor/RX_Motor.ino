#include <IRremote.hpp>

// L298N
const int IN1 = 8;
const int IN2 = 7;
const int ENA = 9;

const int IR_RECEIVE_PIN = 3;

// TIMEOUT
unsigned long lastReceiveTime = 0;  
const unsigned long TIMEOUT_MS = 500; 

void setup() {
  Serial.begin(9600);
  
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);

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
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
        analogWrite(ENA, 200); 

        lastReceiveTime = millis(); 
      } 
      else if (receivedCommand == 0x96) {
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, HIGH);
        analogWrite(ENA, 200);

        lastReceiveTime = millis(); 
      }
      
    } else {
      Serial.println("Canh bao: Sai dia chi!");
    }

    IrReceiver.resume(); 
  }
  if (millis() - lastReceiveTime > TIMEOUT_MS) {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, 0);
  }
}