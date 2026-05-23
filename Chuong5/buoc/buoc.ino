#include <IRremote.hpp>
#include <Stepper.h>

const int IR_RECEIVE_PIN     = 2;
const int stepsPerRevolution = 2038;
const int MY_ADDRESS         = 0x03;
const int CMD_1              = 0x01;
const int CMD_2              = 0x02;

const int PIN_IN1 = 8;
const int PIN_IN2 = 9;
const int PIN_IN3 = 10;
const int PIN_IN4 = 11;

Stepper myStepper(stepsPerRevolution, PIN_IN1, PIN_IN3, PIN_IN2, PIN_IN4);

unsigned long lastReceiveTime = 0;
const unsigned long TIMEOUT   = 600;

void releaseStepper() {
    digitalWrite(PIN_IN1, LOW);
    digitalWrite(PIN_IN2, LOW);
    digitalWrite(PIN_IN3, LOW);
    digitalWrite(PIN_IN4, LOW);
}

void setup() {
    Serial.begin(9600);
    IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
    pinMode(PIN_IN1, OUTPUT);
    pinMode(PIN_IN2, OUTPUT);
    pinMode(PIN_IN3, OUTPUT);
    pinMode(PIN_IN4, OUTPUT);
    releaseStepper();
    lastReceiveTime = millis();
    Serial.println("--- IR STEPPER READY ---");
}

void loop() {
    if (IrReceiver.decode()) {
        uint16_t address = IrReceiver.decodedIRData.address;
        uint16_t command = IrReceiver.decodedIRData.command;

        Serial.print("ADDR: 0x"); Serial.print(address, HEX);
        Serial.print(" | CMD: 0x"); Serial.println(command, HEX);

        if (address == MY_ADDRESS) {
            if (command == CMD_1) {
                myStepper.setSpeed(5);
                myStepper.step(-(stepsPerRevolution / 8)); // Quay ngược 45 độ
                Serial.println("-> Quay nguoc 45 do");
                lastReceiveTime = millis();
                delay(200); 
            }
            else if (command == CMD_2) {
                myStepper.setSpeed(15);
                myStepper.step(stepsPerRevolution / 4);    // Quay thuận 90 độ
                Serial.println("-> Quay thuan 90 do");
                lastReceiveTime = millis();
                delay(200); 
            }
        } else {
            Serial.println("Canh bao: Sai dia chi!");
        }

        IrReceiver.resume();
    }

    // Timeout: mất tín hiệu quá 0.6s thì nhả cuộn dây
    if (millis() - lastReceiveTime > TIMEOUT) {
        releaseStepper();
    }
}
