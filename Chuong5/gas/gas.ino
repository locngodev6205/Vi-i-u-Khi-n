#include <IRremote.hpp>

const int IR_SEND_PIN      = 3;
const int FEEDBACK_LED_PIN = 13;
const int GAS_PIN          = A0;  // Cảm biến khí gas MQ-2 hoặc MQ-135

const int MY_ADDRESS = 0x03;
const int CMD_1      = 0x01;  // Phát hiện khí gas
const int CMD_2      = 0x02;  // Không có khí gas

const int GAS_THRESHOLD = 400; // Ngưỡng phát hiện gas (chỉnh theo thực tế)

void setup() {
    Serial.begin(9600);
    IrSender.begin(IR_SEND_PIN, FEEDBACK_LED_PIN);
}

void loop() {
    int gasValue = analogRead(GAS_PIN);
    Serial.print("Gas value: ");
    Serial.println(gasValue);

    if (gasValue > GAS_THRESHOLD) {  // Phát hiện khí gas → gửi CMD_1
        IrSender.sendRC5(MY_ADDRESS, CMD_1, 0, true);
        Serial.println("-> Phat hien khi gas!");
    } else {                          // Bình thường → gửi CMD_2
        IrSender.sendRC5(MY_ADDRESS, CMD_2, 0, true);
        Serial.println("-> Binh thuong");
    }

    delay(500);
}
