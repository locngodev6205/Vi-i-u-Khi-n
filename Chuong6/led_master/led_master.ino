#include <SoftwareSerial.h>

// D10(RX), D11(TX)
SoftwareSerial BTSerial(10, 11);
const int buttonPin = 7;
bool lastButtonState = HIGH;
bool ledStatus = false;

void setup() {
  pinMode(buttonPin, INPUT);
  BTSerial.begin(38400);
  Serial.begin(9600);

  Serial.println("\n--- MASTER READY! ---");
}

void loop() {
  bool currentButtonState = digitalRead(buttonPin);

  if (lastButtonState == HIGH && currentButtonState == LOW) {
    ledStatus = !ledStatus;

    if (ledStatus) {
      BTSerial.write('1');
      Serial.println("💡 Sent: ON");
    } else {
      BTSerial.write('0');
      Serial.println("🌑 Sent: OFF");
    }
    delay(200);
  }

  lastButtonState = currentButtonState;
}
