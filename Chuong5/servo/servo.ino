#include <IRremote.hpp>
#include <Servo.h>

Servo myServo;
const int SERVO_PIN = 6;
const int IR_RECEIVE_PIN = 2;

// TIMEOUT
unsigned long lastReceiveTime = 0;  
const unsigned long TIMEOUT_MS = 600; 

const int MY_ADDRESS = 0x03;
const int CMD_1      = 0x01;  // Servo quay 1-50 độ
const int CMD_2      = 0x02;  // Servo quay 50 độ

void setup() {
  Serial.begin(9600);
  
  myServo.attach(SERVO_PIN);
  myServo.write(0);

  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
}

void loop() {
  if (IrReceiver.decode()) {
    uint16_t receivedAddress = IrReceiver.decodedIRData.address;
    unsigned long receivedCommand = IrReceiver.decodedIRData.command;
    
    Serial.print("CMD : ");
    Serial.println(receivedCommand, HEX);

    if (receivedAddress == MY_ADDRESS) {
      
      if (receivedCommand == CMD_1) {
        
        myServo.write(45);
        delay(500);
        myServo.write(0);
        delay(500);
        lastReceiveTime = millis(); 
        
      } 
      else if (receivedCommand == CMD_2) {
        
        myServo.write(90);
        delay(500);
        myServo.write(0);
        delay(500);
        lastReceiveTime = millis();
         
      }
      else  {
        myServo.write(0);

        lastReceiveTime = millis(); 
      }

      
    }

    IrReceiver.resume(); 
  }
  if (millis() - lastReceiveTime > TIMEOUT_MS) {
    myServo.write(0);
  }
}
