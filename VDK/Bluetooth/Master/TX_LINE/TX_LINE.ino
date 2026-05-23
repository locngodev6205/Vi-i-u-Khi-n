#include <SoftwareSerial.h>

const int LINE_SENSOR_PIN = 2;
SoftwareSerial BTSerial(10, 11);

int lastLineState = -1;

void setup()
{
  Serial.begin(9600);
  pinMode(LINE_SENSOR_PIN, INPUT);
  BTSerial.begin(9600);
}

void loop()
{
  int currentLineState = digitalRead(LINE_SENSOR_PIN);

  if (currentLineState != lastLineState)
  {

    Serial.print("State : ");
    Serial.println(currentLineState);

    if (currentLineState == HIGH)
    {
      BTSerial.write('A');
      Serial.println("=> Da gui lenh: A");
    }
    else
    {
      BTSerial.write('B');
      Serial.println("=> Da gui lenh: B");
    }

    lastLineState = currentLineState;
  }

  delay(100);
}