#include <SoftwareSerial.h>

const int JOYSTICK_X_PIN = A0;
const int JOYSTICK_Y_PIN = A1;
SoftwareSerial BTSerial(10, 11);

char lastSentCommand = ' ';

void setup()
{
  Serial.begin(9600);
  BTSerial.begin(9600);
}

void loop()
{
  int xValue = analogRead(JOYSTICK_X_PIN);
  int yValue = analogRead(JOYSTICK_Y_PIN);

  Serial.print("X: ");
  Serial.print(xValue);
  Serial.print(" | Y: ");
  Serial.println(yValue);

  char commandToSend;
  if (xValue > 700 || yValue > 700)
  {
    commandToSend = 'A';
  }
  else if (xValue < 300 || yValue < 300)
  {
    commandToSend = 'B';
  }
  else
  {
    commandToSend = 'S';
  }

  if (commandToSend != lastSentCommand)
  {
    BTSerial.write(commandToSend);
    lastSentCommand = commandToSend;
  }

  delay(100);
}