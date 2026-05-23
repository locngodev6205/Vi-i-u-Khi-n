#include <SoftwareSerial.h>

const int TRIG_PIN = 4;     
const int ECHO_PIN = 5;  
SoftwareSerial BTSerial(10, 11); 

char lastSentCommand = ' ';

void setup() {
  Serial.begin(9600);
  BTSerial.begin(9600); 
  
  pinMode(TRIG_PIN, OUTPUT); 
  pinMode(ECHO_PIN, INPUT);  
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

  char commandToSend = lastSentCommand;
  if ( distance <= 20) {
    commandToSend = 'A';
  } 
  else {
    commandToSend = 'B';
  }

  if (commandToSend != lastSentCommand) {
    BTSerial.write(commandToSend); 
    lastSentCommand = commandToSend;
  }

  delay(100);
}