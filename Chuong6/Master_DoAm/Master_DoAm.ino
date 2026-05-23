#include <SoftwareSerial.h>
#include <DHT.h>        

#define DHTTYPE DHT11
const int DHT_PIN = 4; 
SoftwareSerial BTSerial(10, 11); 

// DHT11 
DHT dht(DHT_PIN, DHTTYPE);

unsigned long lastDHTReadTime = 0;
float tempC;
float humidity;

char lastSentCommand = ' '; 

void setup() {
  Serial.begin(9600);

  dht.begin();
  BTSerial.begin(38400);  
}

void loop() {
  if (millis() - lastDHTReadTime >= 1000 || lastDHTReadTime == 0) {
    lastDHTReadTime = millis();
    
    humidity = dht.readHumidity(); 
    tempC = dht.readTemperature();


    Serial.print("Temperature: "); Serial.print(tempC);
    Serial.print(" *C | Humidity: "); Serial.print(humidity);
    Serial.println(" %");

    char commandToSend;
    if (humidity > 80.0) {
      commandToSend = '1'; 
    } 
    else {
      commandToSend = '0';
    }

    if (commandToSend != lastSentCommand) {
      BTSerial.write(commandToSend); 
      lastSentCommand = commandToSend; 
    }
  }
}
