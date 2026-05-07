int LDR = A2;        // cảm biến ánh sáng
int GAS = A3;        // cảm biến khí gas

int lightLED = 10;   // đèn bật khi trời tối
int buzzer = 9;      // còi báo gas

int lightThreshold = 400;   // ngưỡng tối
int gasThreshold = 300;     // ngưỡng khí gas

void setup() {


  pinMode(lightLED, OUTPUT);
  pinMode(buzzer, OUTPUT);
}

void loop() {

  int lightValue = analogRead(LDR);
  int gasValue = analogRead(GAS);


  // ===== CẢM BIẾN ÁNH SÁNG =====
  if(lightValue < lightThreshold){
    digitalWrite(lightLED, HIGH);   // trời tối → bật đèn
  }
  else{
    digitalWrite(lightLED, LOW);    // trời sáng → tắt đèn
  }

  // ===== CẢM BIẾN KHÍ GAS =====
  if(gasValue > gasThreshold){
    for (int x = 0; x < 180; x++) {

      // chuyển độ sang radian
      float sinVal = sin(x * (3.1412 / 180));

      // tạo tần số dao động
      int toneVal = 2000 + (int)(sinVal * 1000);

      tone(buzzer, toneVal);

      delay(2);
    }
  }
  else{
    noTone(buzzer);
  }

  delay(200);
}