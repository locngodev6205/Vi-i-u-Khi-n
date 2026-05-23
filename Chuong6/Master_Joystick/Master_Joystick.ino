#include <SoftwareSerial.h>

const int JOYSTICK_X_PIN = A0;
const int JOYSTICK_Y_PIN = A1;
const int SW = 2;
SoftwareSerial BTSerial(10, 11);

char lastSentCommand = ' ';

void setup()
{
  Serial.begin(9600);
  BTSerial.begin(38400);
}

void loop()
{
  int xValue = analogRead(JOYSTICK_X_PIN);
  int yValue = analogRead(JOYSTICK_Y_PIN);
  int swValue = digitalRead(SW);

  Serial.print("X: ");
  Serial.print(xValue);
  Serial.print(" | Y: ");
  Serial.println(yValue);
  Serial.print(" | SW: ");
  Serial.println(swValue);


  char commandToSend;
  if (xValue > 700 || yValue > 700)
  {
    commandToSend = '1';
  }
  else if (xValue < 300 || yValue < 300)
  {
    commandToSend = '0';
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

  delay(200);
}
