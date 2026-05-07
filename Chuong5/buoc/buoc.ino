#include <IRremote.hpp>
#include <Stepper.h>

const int IR_RECEIVE_PIN     = 2;
const int stepsPerRevolution = 2038;
const int MY_ADDRESS         = 0x03;
const int CMD_1              = 0x01;
const int CMD_2              = 0x02;

// Chân điều khiển stepper
const int PIN_IN1 = 8;
const int PIN_IN2 = 9;
const int PIN_IN3 = 10;
const int PIN_IN4 = 11;

Stepper myStepper(stepsPerRevolution, PIN_IN1, PIN_IN3, PIN_IN2, PIN_IN4);

unsigned long lastReceiveTime = 0;
const unsigned long TIMEOUT   = 600;

// Trạng thái hiện tại của motor
int  currentCommand = 0;      // 0 = dừng, 1 = ngược, 2 = thuận
bool motorRunning   = false;

// Tắt hoàn toàn cuộn dây stepper để tránh nóng và tiết kiệm điện
void stopMotor() {
    digitalWrite(PIN_IN1, LOW);
    digitalWrite(PIN_IN2, LOW);
    digitalWrite(PIN_IN3, LOW);
    digitalWrite(PIN_IN4, LOW);
    motorRunning   = false;
    currentCommand = 0;
}

void setup() {
    Serial.begin(9600);
    IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
    pinMode(PIN_IN1, OUTPUT);
    pinMode(PIN_IN2, OUTPUT);
    pinMode(PIN_IN3, OUTPUT);
    pinMode(PIN_IN4, OUTPUT);
    lastReceiveTime = millis();
}

void loop() {
    // --- Nhận tín hiệu IR ---
    if (IrReceiver.decode()) {
        if (IrReceiver.decodedIRData.address == MY_ADDRESS) {
            if (IrReceiver.decodedIRData.command == CMD_1) {
                currentCommand  = 1;
                motorRunning    = true;
                lastReceiveTime = millis();
                myStepper.setSpeed(15);
                Serial.println("Quay nguoc");
            }
            else if (IrReceiver.decodedIRData.command == CMD_2) {
                currentCommand  = 2;
                motorRunning    = true;
                lastReceiveTime = millis();
                myStepper.setSpeed(5);
                Serial.println("Quay thuan");
            }
        }
        IrReceiver.resume();
    }

    // --- Timeout: mất tín hiệu quá 0.6s thì dừng hẳn ---
    if (millis() - lastReceiveTime > TIMEOUT) {
        if (motorRunning) {
            stopMotor();
            Serial.println("Khong nhan tin hieu - Dung motor");
        }
        return; // Không chạy tiếp phần bên dưới
    }

    // --- Quay motor liên tục khi đang nhận tín hiệu ---
    if (motorRunning) {
        if (currentCommand == 1) {
            myStepper.step(-1);
        } else if (currentCommand == 2) {
            myStepper.step(1);
        }
    }
}
