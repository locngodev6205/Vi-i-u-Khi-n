#include <IRremote.hpp>
#include <AFMotor.h>

const int IR_RECEIVE_PIN = 2;

const int MY_ADDRESS = 0x03;
const int CMD_1      = 0x01;  // Motor quay thuận
const int CMD_2      = 0x02;  // Motor quay ngược

const unsigned long TIMEOUT = 600; // 0.6s không nhận tín hiệu thì dừng

uint8_t speed_thuan = 200;
uint8_t speed_nguoc = 100;

AF_DCMotor motor(3);

unsigned long lastReceiveTime = 0;
bool motorRunning = false;

void setup() {
    Serial.begin(9600);
    IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
    motor.setSpeed(speed_thuan);
    motor.run(RELEASE);
    lastReceiveTime = millis();
    Serial.println("--- IR MOTOR RECEIVER READY ---");
}
 
void loop() {
    if (IrReceiver.decode()) {
        if (IrReceiver.decodedIRData.address == MY_ADDRESS) {
            if (IrReceiver.decodedIRData.command == CMD_1) {
                motor.setSpeed(speed_thuan);
                motor.run(FORWARD);
                motorRunning    = true;
                lastReceiveTime = millis();
                Serial.print("-> Motor 3: QUAY THUAN toc ");
                Serial.println(speed_thuan);
            }
            else if (IrReceiver.decodedIRData.command == CMD_2) {
                motor.setSpeed(speed_nguoc);
                motor.run(BACKWARD);
                motorRunning    = true;
                lastReceiveTime = millis();
                Serial.print("-> Motor 3: QUAY NGUOC toc do ");
                Serial.println(speed_nguoc);
            }
        }

        IrReceiver.resume();
    }

    // Timeout: không nhận tín hiệu quá 0.6s thì dừng motor
    if (motorRunning && millis() - lastReceiveTime > TIMEOUT) {
        motor.run(RELEASE);
        motorRunning = false;
        Serial.println("-> Khong nhan tin hieu - Dung motor");
    }
}
