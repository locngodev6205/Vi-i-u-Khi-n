#include <IRremote.hpp>
#include <Servo.h>

const int IR_RECEIVE_PIN = 2;
const int SERVO_PIN      = 9;

const int MY_ADDRESS = 0x03;
const int CMD_1      = 0x01;  // Servo quay 150 độ
const int CMD_2      = 0x02;  // Servo quay 50 độ

Servo myservo;

void setup() {
    Serial.begin(9600);
    myservo.attach(SERVO_PIN);
    IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
    myservo.write(0);
}

void loop() {
    if (IrReceiver.decode()) {
        Serial.print("Address: 0x");
        Serial.print(IrReceiver.decodedIRData.address, HEX);
        Serial.print("  Command: 0x");
        Serial.println(IrReceiver.decodedIRData.command, HEX);

        if (IrReceiver.decodedIRData.address == MY_ADDRESS) {
            if (IrReceiver.decodedIRData.command == CMD_1) {
                Serial.println("CMD_1 - Servo 150 do");
                myservo.write(150);
                delay(500);
                myservo.write(0);
                delay(500);
            }
            else if (IrReceiver.decodedIRData.command == CMD_2) {
                Serial.println("CMD_2 - Servo 50 do");
                myservo.write(50);
                delay(500);
                myservo.write(0);
                delay(500);
            }
        }

        IrReceiver.resume();
    }
}
