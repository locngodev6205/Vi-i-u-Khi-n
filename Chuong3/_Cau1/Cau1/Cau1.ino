#include <math.h>

int leds[10] = {3, 5, 6, 9, 10, 4, 7, 8, 12, 13};
int buttonPin = 2;

int i = 0;
int sign = 1;

int value_delay = ceil(2000.0 / 256);

bool pressed = false;

void setup() {

  attachInterrupt(0, PressButton, FALLING);

  pinMode(buttonPin, INPUT_PULLUP);

  for(int j = 0; j < 10; j++)
    pinMode(leds[j], OUTPUT);
}

void loop() {

  if(pressed) {  // nhấn nút

    for(int j = 5; j < 10; j++)
      digitalWrite(leds[j], LOW);

    for(int brightness = 255; brightness >= 0; brightness--)
    {
      for(int j = 0; j < 5; j++)
        analogWrite(leds[j], brightness);

      delay(value_delay);
      
    }
    pressed = false;

  }
  else {

    digitalWrite(leds[i], HIGH);
    delay(200);
    digitalWrite(leds[i], LOW);
    delay(200);


    if(i == 9) sign = -1;
    if(i == 0) sign = 1;

    i += sign;
  }
}

void PressButton(){
  pressed = true;
}