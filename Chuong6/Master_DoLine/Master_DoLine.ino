#include <SoftwareSerial.h>

const int LINE_SENSOR_PIN = 2;
SoftwareSerial BTSerial(10, 11);

int lastLineState = -1;

void setup()
{
  Serial.begin(9600);
  pinMode(LINE_SENSOR_PIN, INPUT);
  BTSerial.begin(38400);
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
      BTSerial.write('1');
      Serial.println("=> Da gui lenh: A");
    }
    else
    {
      BTSerial.write('0');
      Serial.println("=> Da gui lenh: B");
    }

    lastLineState = currentLineState;
  }

  delay(200);
}
