#include <SoftwareSerial.h>

const int GAS_SENSOR_PIN = A0;
SoftwareSerial BTSerial(10, 11);

char lastSentCommand = ' ';

void setup()
{
  Serial.begin(9600);
  BTSerial.begin(38400);
}

void loop()
{
  int gasValue = analogRead(GAS_SENSOR_PIN);

  Serial.print("GAS Value: ");
  Serial.println(gasValue);

  char commandToSend = lastSentCommand;
  if (gasValue > 400)
  {
    commandToSend = '1';
  }
  else
  {
    commandToSend = '0';
  }

  if (commandToSend != lastSentCommand)
  {
    BTSerial.write(commandToSend);
    lastSentCommand = commandToSend;
  }

  delay(200);
}
