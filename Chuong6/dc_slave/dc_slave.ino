#include <AFMotor.h>
#include <SoftwareSerial.h>

AF_DCMotor motor(3);
SoftwareSerial BTSerial(10, 11);

// 'S' = Dừng | '1' = Quay thuận | '0' = Quay ngược
char currentMode = 'S';

void setup() {
    motor.setSpeed(200);
    motor.run(RELEASE);
    BTSerial.begin(38400);
    Serial.begin(9600);
    Serial.println("--- SLAVE RECEIVER READY ---");
}

void applyMode() {
    if (currentMode == '1') {
        motor.setSpeed(200);
        motor.run(FORWARD);
        Serial.println("-> Motor 3: QUAY THUAN");
    }
    else if (currentMode == '0') {
        motor.setSpeed(100);
        motor.run(BACKWARD);
        Serial.println("-> Motor 3: QUAY NGUOC");
    }
    else if (currentMode == 'S') {
        motor.run(RELEASE);
        Serial.println("-> Motor 3: DUNG");
    }
}

void loop() {
    if (BTSerial.available()) {
        char cmd = BTSerial.read();
        Serial.print("Nhan lenh: ");
        Serial.println(cmd);

        if (cmd == '1' || cmd == '0' || cmd == 'S') {
            if (cmd != currentMode) {   // Chỉ cập nhật khi lệnh thay đổi
                currentMode = cmd;
                applyMode();
            }
        } else {
            Serial.println("Canh bao: Lenh khong hop le!");
        }
    }
}
