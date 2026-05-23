#include <IRremote.hpp> 
#include <DHT.h>        
#define DHTTYPE DHT11

const int DHT_PIN = 2;       
const int IR_SEND_PIN = 3;   

DHT dht(DHT_PIN, DHTTYPE);

//  DHT11 
unsigned long lastDHTReadTime = 0;
float tempC;
float humidity;

void setup() {
  Serial.begin(9600);
  
  dht.begin();
  IrSender.begin(IR_SEND_PIN, ENABLE_LED_FEEDBACK); 
}

void loop() {
  if (millis() - lastDHTReadTime >= 1000 || lastDHTReadTime == 0) {
    humidity = dht.readHumidity(); 
    tempC = dht.readTemperature();

    Serial.print("Temperature: "); Serial.print(tempC);
    Serial.print(" | Humidity: "); Serial.println(humidity);

    lastDHTReadTime = millis(); 
  }
  
  if (humidity > 80.0) {
    IrSender.sendNEC(0x00, 0x69, 0); 
  } 
  else {
    IrSender.sendNEC(0x00, 0x96, 0);
  }
  delay(200);
}