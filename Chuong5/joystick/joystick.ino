#include <IRremote.hpp>

const int IR_SEND_PIN      = 3;
const int FEEDBACK_LED_PIN = 13;

const int MY_ADDRESS = 0x03;
const int CMD_1      = 0x01;  // Joystick kích hoạt
const int CMD_2      = 0x02;  // Joystick ở trạng thái nghỉ

// Chân joystick
const int JOYSTICK_X = A1;
const int JOYSTICK_Y = A2;
const int JOYSTICK_Z = 2;

int threshold1 = 600;
int threshold2 = 400;

//bool joystick(int threshold1 = 600, int threshold2 = 400) {
//    int x = analogRead(JOYSTICK_X);
//    int y = analogRead(JOYSTICK_Y);
//    int z = digitalRead(JOYSTICK_Z);
//
//    Serial.println("JOYSTICK " + String(x) + ", " + String(y) + ", " + String(z));
//
//    if (x >= threshold1) {
//        return true;
//    } else if (x <= threshold2) {
//        return false;
//    }
//}

void setup() {
    Serial.begin(9600);
    IrSender.begin(IR_SEND_PIN, FEEDBACK_LED_PIN);
    pinMode(JOYSTICK_Z, INPUT_PULLUP);
    Serial.println("--- IR JOYSTICK READY ---");
}

void loop() {
    int x = analogRead(JOYSTICK_X);
    int y = analogRead(JOYSTICK_Y);
    int z = digitalRead(JOYSTICK_Z);
    Serial.println("JOYSTICK " + String(x) + ", " + String(y) + ", " + String(z));
    if (x >= threshold1) {
        IrSender.sendRC5(MY_ADDRESS, CMD_1, 0, true);
        Serial.println("-> Joystick kich hoat 1");
    } else if (x <= threshold2) {
        IrSender.sendRC5(MY_ADDRESS, CMD_2, 0, true);
        Serial.println("-> Joystick kich hoat 2");
    }
//    if (joystick()) {  // Joystick kích hoạt → gửi CMD_1
//        IrSender.sendRC5(MY_ADDRESS, CMD_1, 0, true);
//        Serial.println("-> Joystick kich hoat");
//    } else {           // Nghỉ → gửi CMD_2
//        IrSender.sendRC5(MY_ADDRESS, CMD_2, 0, true);
//        Serial.println("-> Joystick nghi");
//    }

    delay(500);
}
