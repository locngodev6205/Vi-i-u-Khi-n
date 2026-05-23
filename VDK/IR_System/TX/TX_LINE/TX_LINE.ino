#include <IRremote.hpp> 

const int LINE_SENSOR_PIN = 2;
const int IR_SEND_PIN = 3;    

void setup() {
  Serial.begin(9600);

  pinMode(LINE_SENSOR_PIN, INPUT);
  IrSender.begin(IR_SEND_PIN, ENABLE_LED_FEEDBACK);
}

void loop() {
  int lineState = digitalRead(LINE_SENSOR_PIN); 
  Serial.print("Stae : ");
  Serial.println(lineState);
  // HIGH (1) = Gặp vạch đen (hấp thụ tia, không phản xạ)
  // LOW (0) = Gặp nền trắng (phản xạ tia tốt)
  
  if (lineState == HIGH) {
    IrSender.sendNEC(0x00, 0x69, 0); 
  } 
  else {
    IrSender.sendNEC(0x00, 0x96, 0);
  }
  
  delay(200);
}