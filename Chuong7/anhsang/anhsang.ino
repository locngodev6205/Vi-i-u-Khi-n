#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "TRAM 247 STUDY CAFE & WORKSPACE";
const char* password = "tramloveyou";

WebServer server(80);
const int LDR_PIN = 4; // Chân Analog ADC1

String getHTML() {
  String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<style>";
  html += "body { text-align: center; font-family: sans-serif; background: #f4f4f4; }";
  html += ".sensor-box { font-size: 50px; color: #2ecc71; background: white; ";
  html += "display: inline-block; padding: 20px; border-radius: 15px; ";
  html += "box-shadow: 0 4px 8px rgba(0,0,0,0.1); }";
  html += "</style>";

  html += "<script>";
  // Hàm tự động cập nhật dữ liệu mỗi 500ms
  html += "setInterval(function() {";
  html += "fetch('/readLDR').then(response => response.text()).then(data => {";
  html += "document.getElementById('ldrValue').innerText = data;";
  html += "});";
  html += "}, 500);"; // cập nhật 2 lần mỗi giây
  html += "</script>";

  html += "</head><body>";
  html += "<h1>GIAM SAT ANH SANG REAL-TIME</h1>";
  html += "<div class='sensor-box'><span id='ldrValue'>0</span></div>";
  html += "<p>Don vi: Gia tri ADC (0 - 4095)</p>";
  html += "</body></html>";

  return html;
}

void setup() {
  Serial.begin(115200);
  analogReadResolution(12); // 12-bit (0–4095)

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nIP: " + WiFi.localIP().toString());

  server.on("/", []() {
    server.send(200, "text/html", getHTML());
  });

  // API ngầm để lấy dữ liệu LDR
  server.on("/readLDR", []() {
    int val = analogRead(LDR_PIN);
    server.send(200, "text/plain", String(val));
  });

  server.begin();
}

void loop() {
  server.handleClient();
}
