#include <IRremote.hpp>

const int IR_SEND_PIN      = 3;
const int FEEDBACK_LED_PIN = 13;
const int LDR_PIN          = A0;
const int MY_ADDRESS       = 0x03;
const int CMD_1           = 0x01;
const int CMD_2           = 0x02;

void setup() {
    Serial.begin(9600);
    IrSender.begin(IR_SEND_PIN, FEEDBACK_LED_PIN);
}

void loop() {
    int lightValue = analogRead(LDR_PIN);
    Serial.print("Light value: ");
    Serial.println(lightValue);

    if (lightValue > 700) {  // Tối → gửi tín hiệu
        IrSender.sendRC5(MY_ADDRESS, CMD_1, 0, true);
        Serial.println("Dark");
    } else {
        IrSender.sendRC5(MY_ADDRESS, CMD_2, 0, true);
        Serial.println("Bright");
    }

    delay(500);
}
