#include <IRremote.hpp>

const int JOYSTICK_X_PIN = A0; 
const int JOYSTICK_Y_PIN = A1;      
const int IR_SEND_PIN = 3;   

void setup() {
  Serial.begin(9600);
  
  IrSender.begin(IR_SEND_PIN, ENABLE_LED_FEEDBACK); 
}

void loop() {
  int xValue = analogRead(JOYSTICK_X_PIN); 
  int yValue = analogRead(JOYSTICK_Y_PIN);
  
  Serial.print("X: "); Serial.print(xValue);
  Serial.print(" | Y: "); Serial.println(yValue);

  if (xValue > 700 || yValue > 700) {
    IrSender.sendNEC(0x00, 0x69, 0); 
  } 
  else if (xValue < 300 || yValue < 300) {
    IrSender.sendNEC(0x00, 0x96, 0);
  }
  else {
   chat IrSender.sendNEC(0x00, 0x44, 0);
  }
  
  delay(200);
}
