#include <IRremote.hpp> 

const int LDR_PIN = A0;
const int IR_SEND_PIN = 3; 

void setup() {
  Serial.begin(9600);

  IrSender.begin(IR_SEND_PIN, ENABLE_LED_FEEDBACK); 
}

void loop() {
  int lightValue = analogRead(LDR_PIN);
  Serial.print("LDR : ");
  Serial.println(lightValue);

  if (lightValue > 600) {
    IrSender.sendNEC(0x00, 0x69, 0); 
  } 
  else {
    IrSender.sendNEC(0x00, 0x96, 0);
  }
  delay(200);
}