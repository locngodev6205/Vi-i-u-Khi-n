#include <WiFi.h>

const char* ssid = "TRAM 247 STUDY CAFE & WORKSPACE";
const char* password = "tramloveyou";

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);

  Serial.print("Dang ket noi WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nDa ket noi!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void loop() {

}
