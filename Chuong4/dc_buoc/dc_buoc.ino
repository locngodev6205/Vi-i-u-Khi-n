#include <Stepper.h>

const int trigPin = 6;
const int echoPin = 7;

const int stepsPerRevolution = 2038;
const int step90  = stepsPerRevolution / 4;
const int step180 = stepsPerRevolution / 2; // nhớ sửa /8 → /2

// ✅ Đổi thứ tự chân: 8, 10, 9, 11
Stepper myStepper = Stepper(stepsPerRevolution, 8, 10, 9, 11);

void setup() {
    Serial.begin(9600);
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
}

void loop() {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    long duration = pulseIn(echoPin, HIGH, 30000);
    if (duration == 0) return;

    int distance = duration * 0.034 / 2;
    Serial.println(distance);

    myStepper.setSpeed(10);

    if (distance > 20) {
        myStepper.step(step90);    // quay thuận 90°
        delay(200);
    } else {
        myStepper.step(-step180);  // quay ngược 180°
        delay(200);
    }
}
