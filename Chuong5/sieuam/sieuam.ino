#include <IRremote.hpp>

const int IR_SEND_PIN      = 3;
const int FEEDBACK_LED_PIN = 13;

const int MY_ADDRESS = 0x03;
const int CMD_1      = 0x01;  // Vật thể gần (distance < 20cm)
const int CMD_2      = 0x02;  // Không có vật thể

const int TRIG_PIN = 7;
const int ECHO_PIN = 6;

void setup() {
    Serial.begin(9600);
    IrSender.begin(IR_SEND_PIN, FEEDBACK_LED_PIN);
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    Serial.println("--- IR ULTRASONIC READY ---");
}

void loop() {
    // Phát xung siêu âm
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    // Đo khoảng cách
    long duration = pulseIn(ECHO_PIN, HIGH);
    int distance  = duration * 0.034 / 2.0;

    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    if (distance < 20) {  // Vật thể gần → gửi CMD_1
        IrSender.sendRC5(MY_ADDRESS, CMD_1, 0, true);
        Serial.println("-> Phat hien vat the!");
    } else {              // Không có vật thể → gửi CMD_2
        IrSender.sendRC5(MY_ADDRESS, CMD_2, 0, true);
        Serial.println("-> Khong co vat the");
    }

    delay(500);
}
