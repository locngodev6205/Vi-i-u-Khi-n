  #include <SoftwareSerial.h>

  //  L298N
  const int IN1 = 8;
  const int IN2 = 7;
  const int ENA = 9;

  SoftwareSerial BTSerial(10, 11);

  //
  char currentMode = 'S';

  void setup()
  {
    Serial.begin(9600);
    BTSerial.begin(9600);

    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(ENA, OUTPUT);
  }

  void loop()
  {
    if (BTSerial.available())
    {
      char newChar = BTSerial.read();

      if (newChar == 'A' || newChar == 'B' || newChar == 'S')
      {
        currentMode = newChar;
        Serial.print("CMD : ");
        Serial.println(currentMode);
      }
      else
      {
        Serial.print("Canh bao: Ky tu khong hop le -> ");
        Serial.println(newChar);
      }
    }

    if (currentMode == 'A')
    {
      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, LOW);
      analogWrite(ENA, 200);
    }
    else if (currentMode == 'B')
    {
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, HIGH);
      analogWrite(ENA, 200);
    }
    else if (currentMode == 'S')
    {
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, LOW);
      analogWrite(ENA, 0);
    }
  }