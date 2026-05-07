// ===== JOYSTICK =====
int VRx = A0;
int VRy = A1;
int SW  = 2;

// ===== LED =====
int LED_BEDROOM = 3;
int LED_LIVING  = 4;
int LED_KITCHEN = 5;
int LED_TOILET  = 6;

// ===== ULTRASONIC HC-SR04 =====
int TRIG = 7;
int ECHO = 8;

// ===== BUZZER =====
int BUZZER = 9;

bool allLights = true;

void setup() {

  pinMode(SW, INPUT_PULLUP);

  pinMode(LED_BEDROOM, OUTPUT);
  pinMode(LED_LIVING, OUTPUT);
  pinMode(LED_KITCHEN, OUTPUT);
  pinMode(LED_TOILET, OUTPUT);

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  pinMode(BUZZER, OUTPUT);

  // bật tất cả đèn ban đầu
  digitalWrite(LED_BEDROOM, HIGH);
  digitalWrite(LED_LIVING, HIGH);
  digitalWrite(LED_KITCHEN, HIGH);
  digitalWrite(LED_TOILET, HIGH);
}

void loop() {

  int xValue = analogRead(VRx);
  int yValue = analogRead(VRy);
  int button = digitalRead(SW);

  // trái
  if (xValue < 300) {
    digitalWrite(LED_BEDROOM, LOW);
  }

  // phải
  if (xValue > 700) {
    digitalWrite(LED_LIVING, LOW);
  }

  // lên
  if (yValue > 700) {
    digitalWrite(LED_KITCHEN, LOW);
  }

  // xuống
  if (yValue < 300) {
    digitalWrite(LED_TOILET, LOW);
  }

  // nhấn joystick
  if (button == LOW) {

    allLights = !allLights;

    digitalWrite(LED_BEDROOM, allLights);
    digitalWrite(LED_LIVING, allLights);
    digitalWrite(LED_KITCHEN, allLights);
    digitalWrite(LED_TOILET, allLights);

    delay(300);
  }

  // ===== ULTRASONIC =====
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2); 

  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  long duration = pulseIn(ECHO, HIGH);

  float distance = duration * 0.034 / 2;

  if (distance <= 50) {

    for (int x = 0; x < 180; x++) {

      // chuyển độ sang radian
      float sinVal = sin(x * (3.1412 / 180));

      // tạo tần số dao động
      int toneVal = 2000 + (int)(sinVal * 1000);

      tone(BUZZER, toneVal);

      delay(2);
    }

  } else {
    noTone(BUZZER);
  }

  delay(100);
}