int sensorPin = 2;
int ledPin = 8;

void setup() {

  pinMode(sensorPin, INPUT);
  pinMode(ledPin, OUTPUT);

}

void loop() {

  int value = digitalRead(sensorPin);

  if(value == 0) {   // gặp line đen
    digitalWrite(ledPin, HIGH);
  }
  else {             // nền trắng
    digitalWrite(ledPin, LOW);
  }

}