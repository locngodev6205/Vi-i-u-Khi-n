int ledPins[10] = {2,3,4,5,6,7,8,9,10,11};

int mode = 0;

void setup() {
  Serial.begin(9600);

  for(int i = 0; i < 10; i++){
    pinMode(ledPins[i], OUTPUT);
  }
}

void clearLED(){
  for(int i = 0; i < 10; i++){
    digitalWrite(ledPins[i], LOW);
  }
}

void outsideToInside(){

  for(int i = 0; i < 5; i++){
    clearLED();

    digitalWrite(ledPins[i], HIGH);
    digitalWrite(ledPins[9-i], HIGH);

    delay(200);
  }
}

void insideToOutside(){

  for(int i = 4; i >= 0; i--){
    clearLED();

    digitalWrite(ledPins[i], HIGH);
    digitalWrite(ledPins[9-i], HIGH);

    delay(200);
  }
}

void pingpong(){

  outsideToInside();
  insideToOutside();
}

void loop() {

  if(Serial.available()){
    char cmd = Serial.read();

    if(cmd == '1') mode = 1;
    if(cmd == '2') mode = 2;
    if(cmd == '3') mode = 3;
  }

  if(mode == 1){
    outsideToInside();
  }

  if(mode == 2){
    insideToOutside();
  }

  if(mode == 3){
    pingpong();
  }
}