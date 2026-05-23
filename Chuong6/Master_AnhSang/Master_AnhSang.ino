#include <SoftwareSerial.h>

const int LDR_PIN = A0;
SoftwareSerial BTSerial(10, 11);

char lastSentCommand = ' ';

void setup()
{
  Serial.begin(9600);
  BTSerial.begin(38400);
}

void loop()
{
  int lightValue = analogRead(LDR_PIN);
  Serial.print("LDR : ");
  Serial.println(lightValue);

  char commandToSend = lastSentCommand;
  if (lightValue > 600)
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
