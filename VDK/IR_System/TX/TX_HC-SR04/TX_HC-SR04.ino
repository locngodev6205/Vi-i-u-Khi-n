#include <IRremote.hpp>

const int TRIG_PIN = 4;     
const int ECHO_PIN = 5;      
const int IR_SEND_PIN = 3;   

void setup() {
  Serial.begin(9600);
  
  pinMode(TRIG_PIN, OUTPUT); 
  pinMode(ECHO_PIN, INPUT);  
  
  IrSender.begin(IR_SEND_PIN, ENABLE_LED_FEEDBACK); 
}

void loop() {

  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  int distance = duration * 0.034 / 2;

  Serial.print("Khoang cach: ");
  Serial.print(distance);
  Serial.println(" cm");

  if (distance > 0 && distance <= 20) {
    IrSender.sendNEC(0x00, 0x69, 0); 
  } 
  else {
    IrSender.sendNEC(0x00, 0x96, 0);
  }
  delay(200);
}
