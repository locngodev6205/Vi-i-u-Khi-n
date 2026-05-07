#include <WiFi.h>
#include <WebServer.h>

const char* ssid     = "XiaoHuHu";
const char* password = "urauraura";

WebServer server(80);

const int LED_PIN = 18;
bool ledState = false;

String getHTML() {
  String onStyle  = ledState
    ? "padding:20px;background:green;color:white;border:3px solid #00ff00;box-shadow:0 0 12px #00ff00"
    : "padding:20px;background:#333;color:#777;border:3px solid #333";
  String offStyle = !ledState
    ? "padding:20px;background:red;color:white;border:3px solid #ff4444;box-shadow:0 0 12px #ff4444"
    : "padding:20px;background:#333;color:#777;border:3px solid #333";

  String html = "<html><head><meta charset='UTF-8'><title>Lab</title></head><body style='background:#111;color:#eee;font-family:Arial;text-align:center;padding-top:60px'>";
  html += "<h1>Dieu khien LED</h1>";
  html += "<p>Trang thai: <b>" + String(ledState ? "SANG" : "TAT") + "</b></p>";
  html += "<a href='/on'><button style='" + onStyle + ";border-radius:10px;font-size:1rem;cursor:pointer;margin:10px'>BAT LED</button></a>";
  html += "<a href='/off'><button style='" + offStyle + ";border-radius:10px;font-size:1rem;cursor:pointer;margin:10px'>TAT LED</button></a>";
  html += "</body></html>";
  return html;
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print('.'); }
  Serial.println("\nIP: " + WiFi.localIP().toString());

  server.on("/", []() {
    server.send(200, "text/html", getHTML());
  });
  server.on("/on", []() {
    ledState = true;
    digitalWrite(LED_PIN, HIGH);
    server.send(200, "text/html", getHTML());
  });
  server.on("/off", []() {
    ledState = false;
    digitalWrite(LED_PIN, LOW);
    server.send(200, "text/html", getHTML());
  });

  server.begin();
}

void loop() {
  server.handleClient();
}
