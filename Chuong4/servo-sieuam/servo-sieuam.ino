#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

const char* ssid = "TRAM 247 STUDY CAFE & WORKSPACE";
const char* password = "tramloveyou";

WebServer server(80);
Servo servo;

const int SERVO_PIN = 8;

int currentAngle = 90;
bool isSweeping = false;
int sweepDirection = 1;
int sweepDelayMs = 15;
unsigned long lastSweepMs = 0;

void setServoAngle(int angle) {
  currentAngle = constrain(angle, 0, 180);
  servo.write(currentAngle);
}

void startSweep() {
  isSweeping = true;
  sweepDirection = 1;
  lastSweepMs = millis();
}

void stopSweep() {
  isSweeping = false;
}

String getHTML() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="vi">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Lab 4 - Servo Control</title>
  <style>
    :root {
      --bg: #f2f1ed;
      --panel: #15252b;
      --accent: #2f11a3;
      --text: #f7f5f0;
      --muted: #d5d1c8;
    }
    body {
      margin: 0;
      font-family: Arial, Helvetica, sans-serif;
      background: radial-gradient(circle at top, #ffffff 0%, var(--bg) 55%, #e4e0d6 100%);
      color: #161616;
      display: flex;
      justify-content: center;
      padding: 24px;
    }
    .card {
      width: min(760px, 100%);
      background: rgba(255, 255, 255, 0.72);
      border: 2px solid rgba(47, 17, 163, 0.22);
      border-radius: 28px;
      box-shadow: 0 18px 50px rgba(0, 0, 0, 0.08);
      padding: 28px;
      backdrop-filter: blur(6px);
    }
    h1 {
      margin: 0 0 8px;
      color: var(--accent);
      font-size: clamp(2rem, 5vw, 3.4rem);
      line-height: 1;
    }
    .meta {
      display: flex;
      justify-content: space-between;
      align-items: center;
      gap: 16px;
      margin-bottom: 24px;
      color: #4b4b4b;
    }
    .badge {
      background: var(--accent);
      color: white;
      border-radius: 999px;
      padding: 10px 18px;
      font-weight: 700;
      letter-spacing: 0.04em;
    }
    .panel {
      background: var(--panel);
      color: var(--text);
      border-radius: 22px;
      padding: 22px;
    }
    .status {
      display: grid;
      grid-template-columns: repeat(2, minmax(0, 1fr));
      gap: 12px;
      margin-bottom: 18px;
    }
    .box {
      background: rgba(255, 255, 255, 0.06);
      border: 1px solid rgba(255, 255, 255, 0.12);
      border-radius: 16px;
      padding: 14px 16px;
    }
    .label {
      font-size: 0.8rem;
      color: var(--muted);
      text-transform: uppercase;
      letter-spacing: 0.08em;
      margin-bottom: 6px;
    }
    .value {
      font-size: 1.2rem;
      font-weight: 700;
    }
    .buttons {
      display: grid;
      grid-template-columns: repeat(3, minmax(0, 1fr));
      gap: 12px;
      margin: 18px 0;
    }
    button {
      border: 0;
      border-radius: 16px;
      padding: 14px 12px;
      font-size: 0.96rem;
      font-weight: 700;
      color: white;
      cursor: pointer;
    }
    .preset { background: #1968d2; }
    .sweep-on { background: #1ea24b; }
    .sweep-off { background: #d63c3c; }
    .center { background: #7a4d1e; }
    .slider-wrap {
      margin-top: 12px;
      padding: 16px;
      background: rgba(255, 255, 255, 0.06);
      border: 1px solid rgba(255, 255, 255, 0.12);
      border-radius: 16px;
    }
    input[type="range"] {
      width: 100%;
      margin-top: 12px;
    }
    .note {
      margin-top: 14px;
      color: #cfc9bf;
      font-size: 0.92rem;
    }
    @media (max-width: 640px) {
      .status, .buttons {
        grid-template-columns: 1fr;
      }
      .meta {
        flex-direction: column;
        align-items: flex-start;
      }
    }
  </style>
</head>
<body>
  <div class="card">
    <h1>Lab 4 - Servo Web</h1>
    <div class="meta">
      <div>ESP32S3 điều khiển servo bằng WiFi</div>
      <div class="badge">2026</div>
    </div>
    <div class="panel">
      <div class="status">
        <div class="box">
          <div class="label">Góc hiện tại</div>
          <div class="value"><span id="angleText">)rawliteral";
  html += String(currentAngle);
  html += R"rawliteral(</span> độ</div>
        </div>
        <div class="box">
          <div class="label">Chế độ</div>
          <div class="value" id="modeText">)rawliteral";
  html += isSweeping ? "Sweep" : "Manual";
  html += R"rawliteral(</div>
        </div>
      </div>

      <div class="buttons">
        <button class="preset" onclick="setAngle(0)">0°</button>
        <button class="preset" onclick="setAngle(45)">45°</button>
        <button class="preset" onclick="setAngle(90)">90°</button>
        <button class="preset" onclick="setAngle(135)">135°</button>
        <button class="preset" onclick="setAngle(180)">180°</button>
        <button class="center" onclick="centerServo()">CENTER</button>
      </div>

      <div class="buttons">
        <button class="sweep-on" onclick="sendCommand('/sweep/start', 'Sweep started')">SWEEP ON</button>
        <button class="sweep-off" onclick="sendCommand('/sweep/stop', 'Sweep stopped')">SWEEP OFF</button>
      </div>

      <div class="slider-wrap">
        <div class="label">Góc servo</div>
        <input type="range" min="0" max="180" value=")rawliteral";
  html += String(currentAngle);
  html += R"rawliteral(" id="angleSlider" oninput="setAngle(this.value)">
      </div>

      <div class="slider-wrap">
        <div class="label">Tốc độ sweep (delay ms mỗi bước)</div>
        <input type="range" min="5" max="60" value=")rawliteral";
  html += String(sweepDelayMs);
  html += R"rawliteral(" id="sweepDelay" oninput="setSweepDelay(this.value)">
      </div>

      <div class="note" id="message">Sẵn sàng điều khiển.</div>
    </div>
  </div>

  <script>
    const message = document.getElementById('message');
    const angleText = document.getElementById('angleText');
    const modeText = document.getElementById('modeText');

    function sendCommand(path, fallbackMessage) {
      fetch(path)
        .then(response => response.text())
        .then(text => {
          message.textContent = text || fallbackMessage;
          if (path === '/sweep/start') modeText.textContent = 'Sweep';
          if (path === '/sweep/stop') modeText.textContent = 'Manual';
        });
    }

    function setAngle(val) {
      const angle = Number(val);
      angleText.textContent = angle;
      modeText.textContent = 'Manual';
      fetch('/setAngle?val=' + angle)
        .then(response => response.text())
        .then(text => {
          message.textContent = text;
        });
    }

    function centerServo() {
      fetch('/center')
        .then(response => response.text())
        .then(text => {
          angleText.textContent = '90';
          document.getElementById('angleSlider').value = 90;
          modeText.textContent = 'Manual';
          message.textContent = text;
        });
    }

    function setSweepDelay(val) {
      fetch('/setSweepDelay?val=' + val)
        .then(response => response.text())
        .then(text => {
          message.textContent = text;
        });
    }
  </script>
</body>
</html>
)rawliteral";
  return html;
}

void setup() {
  Serial.begin(115200);
  servo.setPeriodHertz(50);
  servo.attach(SERVO_PIN);
  setServoAngle(currentAngle);

  Serial.println("Servo web control started (ESP32S3)");
  Serial.printf("Servo pin: GPIO%d\n", SERVO_PIN);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print('.');
  }
  Serial.println();
  Serial.println("Connected! IP: " + WiFi.localIP().toString());

  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html", getHTML());
  });

  server.on("/setAngle", HTTP_GET, []() {
    if (server.hasArg("val")) {
      stopSweep();
      setServoAngle(server.arg("val").toInt());
      Serial.println("Angle set to: " + String(currentAngle));
    }
    server.send(200, "text/plain", "Angle set to " + String(currentAngle) + " deg");
  });

  server.on("/center", HTTP_GET, []() {
    stopSweep();
    setServoAngle(90);
    Serial.println("Servo centered");
    server.send(200, "text/plain", "Servo centered (90 deg)");
  });

  server.on("/sweep/start", HTTP_GET, []() {
    startSweep();
    Serial.println("Sweep started");
    server.send(200, "text/plain", "Sweep started");
  });

  server.on("/sweep/stop", HTTP_GET, []() {
    stopSweep();
    Serial.println("Sweep stopped");
    server.send(200, "text/plain", "Sweep stopped");
  });

  server.on("/setSweepDelay", HTTP_GET, []() {
    if (server.hasArg("val")) {
      sweepDelayMs = constrain(server.arg("val").toInt(), 5, 60);
      Serial.println("Sweep delay set to: " + String(sweepDelayMs) + " ms");
    }
    server.send(200, "text/plain", "Sweep delay: " + String(sweepDelayMs) + " ms");
  });

  server.begin();
}

void loop() {
  server.handleClient();

  if (isSweeping) {
    unsigned long now = millis();
    if (now - lastSweepMs >= (unsigned long)sweepDelayMs) {
      lastSweepMs = now;
      currentAngle += sweepDirection;

      if (currentAngle >= 180) {
        currentAngle = 180;
        sweepDirection = -1;
      } else if (currentAngle <= 0) {
        currentAngle = 0;
        sweepDirection = 1;
      }

      servo.write(currentAngle);
    }
  }
}
