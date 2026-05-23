#include <IRremote.hpp>
#include <DHT.h>

const int IR_SEND_PIN      = 3;
const int FEEDBACK_LED_PIN = 13;
const int DHT_PIN          = 4;

const int MY_ADDRESS = 0x03;
const int CMD_1      = 0x01;  // Nhiệt độ & độ ẩm vượt ngưỡng
const int CMD_2      = 0x02;  // Bình thường

#define DHT_TYPE DHT11
DHT HT(DHT_PIN, DHT_TYPE);

float humi;
float temp;

void setup() {
    Serial.begin(9600);
    HT.begin();
    IrSender.begin(IR_SEND_PIN, FEEDBACK_LED_PIN);
    Serial.println("--- IR DHT11 SENSOR READY ---");
}

void loop() {
    humi = HT.readHumidity();
    temp = HT.readTemperature();

    // Kiểm tra cảm biến có đọc được không
    if (isnan(humi) || isnan(temp)) {
        Serial.println("Loi doc cam bien DHT11!");
        delay(500);
        return;
    }

    Serial.print("Humidity: ");
    Serial.println(humi);
    Serial.print("Temperature: ");
    Serial.println(temp);

    if (humi > 85 && temp > 29) {  // Vượt ngưỡng → gửi CMD_1
        IrSender.sendRC5(MY_ADDRESS, CMD_1, 0, true);
        Serial.println("-> Vuot nguong nhiet do & do am!");
    } else {                        // Bình thường → gửi CMD_2
        IrSender.sendRC5(MY_ADDRESS, CMD_2, 0, true);
        Serial.println("-> Binh thuong");
    }

    delay(500);
}
