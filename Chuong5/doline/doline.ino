#include <IRremote.hpp>

const int IR_SEND_PIN      = 3;
const int FEEDBACK_LED_PIN = 13;
const int LINE_PIN         = 2;

const int MY_ADDRESS = 0x03;
const int CMD_1      = 0x01;  // Phát hiện line (val == 0)
const int CMD_2      = 0x02;  // Không có line  (val == 1)

void setup() {
    Serial.begin(9600);
    IrSender.begin(IR_SEND_PIN, FEEDBACK_LED_PIN);
    pinMode(LINE_PIN, INPUT);
    Serial.println("--- IR LINE SENSOR READY ---");
}

void loop() {
    int val = digitalRead(LINE_PIN);
    Serial.print("Line value: ");
    Serial.println(val);

    if (val == 0) {  // Phát hiện line → gửi CMD_1
        IrSender.sendRC5(MY_ADDRESS, CMD_1, 0, true);
        Serial.println("-> Phat hien line");
    } else {         // Không có line → gửi CMD_2
        IrSender.sendRC5(MY_ADDRESS, CMD_2, 0, true);
        Serial.println("-> Khong co line");
    }

    delay(500);
}
