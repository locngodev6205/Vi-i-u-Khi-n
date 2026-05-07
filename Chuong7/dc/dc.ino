#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "TRAM 247 STUDY CAFE & WORKSPACE";
const char* password = "tramloveyou";

WebServer server(80);

// DK Electronics L293D Motor Shield v1 (74HC595) + ESP32S3 jumper wiring
// ESP32S3 -> Shield digital pin labels: D12=LATCH, D4=CLK, D8=DATA, D7=ENABLE, D11=PWM(M1)
const int SHIELD_LATCH_PIN = 12;
const int SHIELD_CLK_PIN = 4;
const int SHIELD_DATA_PIN = 8;
const int SHIELD_ENABLE_PIN = 7;
const int M1_PWM_PIN = 11;

const int PWM_FREQUENCY = 5000;
const int PWM_RESOLUTION = 8;

const uint8_t MOTOR1_A = 2;
const uint8_t MOTOR1_B = 3;

uint8_t latchState = 0;

int currentSpeed = 150;
String motorState = "Stopped";

void latchTransmit() {
  digitalWrite(SHIELD_LATCH_PIN, LOW);
  shiftOut(SHIELD_DATA_PIN, SHIELD_CLK_PIN, MSBFIRST, latchState);
  digitalWrite(SHIELD_LATCH_PIN, HIGH);
}

void setMotor1(bool aHigh, bool bHigh) {
  bitWrite(latchState, MOTOR1_A, aHigh ? 1 : 0);
  bitWrite(latchState, MOTOR1_B, bHigh ? 1 : 0);
  latchTransmit();
}

void applySpeed(int speed) {
  currentSpeed = constrain(speed, 0, 255);
  ledcWrite(M1_PWM_PIN, currentSpeed);
}

void forwardMotor() {
  setMotor1(true, false);
  applySpeed(currentSpeed);
  motorState = "Forward";
}

void backwardMotor() {
  setMotor1(false, true);
  applySpeed(currentSpeed);
  motorState = "Backward";
}

void stopMotor() {
  setMotor1(false, false);
  ledcWrite(M1_PWM_PIN, 0);
  motorState = "Stopped";
}

String getHTML() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="vi">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Lab 3 - DC Motor</title>
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
      width: min(720px, 100%);
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
      padding: 16px 14px;
      font-size: 1rem;
      font-weight: 700;
      color: white;
      cursor: pointer;
    }
    .forward { background: #1ea24b; }
    .backward { background: #1968d2; }
    .stop { background: #d63c3c; }
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
    <h1>Lab 3 - DC Motor</h1>
    <div class="meta">
      <div>ESP32S3 điều khiển M1 trên L293D Shield v1 bằng WiFi</div>
      <div class="badge">2026</div>
    </div>
    <div class="panel">
      <div class="status">
        <div class="box">
          <div class="label">Trạng thái</div>
          <div class="value" id="state">)rawliteral";
  html += motorState;
  html += R"rawliteral(</div>
        </div>
        <div class="box">
          <div class="label">Tốc độ hiện tại</div>
          <div class="value"><span id="speedText">)rawliteral";
  html += String(currentSpeed);
  html += R"rawliteral(</span> / 255</div>
        </div>
      </div>

      <div class="buttons">
        <button class="forward" onclick="sendCommand('/forward')">THUẬN</button>
        <button class="backward" onclick="sendCommand('/backward')">NGƯỢC</button>
        <button class="stop" onclick="sendCommand('/stop')">DỪNG</button>
      </div>

      <div class="slider-wrap">
        <div class="label">Điều chỉnh tốc độ</div>
        <input type="range" min="0" max="255" value=")rawliteral";
  html += String(currentSpeed);
  html += R"rawliteral(" id="speedSlider" oninput="updateSpeed(this.value)">
      </div>

      <div class="note" id="message">Sẵn sàng điều khiển.</div>
    </div>
  </div>

  <script>
    const message = document.getElementById('message');
    const speedText = document.getElementById('speedText');
    const stateText = document.getElementById('state');

    function sendCommand(path) {
      fetch(path)
        .then(response => response.text())
        .then(text => {
          message.textContent = text;
          if (path === '/forward') stateText.textContent = 'Forward';
          if (path === '/backward') stateText.textContent = 'Backward';
          if (path === '/stop') stateText.textContent = 'Stopped';
        });
    }

    function updateSpeed(value) {
      speedText.textContent = value;
      fetch('/setspeed?val=' + value)
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
  Serial.println("Lab3 mode: ESP32S3 + DK L293D Shield v1 (74HC595)");
  Serial.printf("LATCH(D12)=%d, CLK(D4)=%d, DATA(D8)=%d, EN(D7)=%d, M1_PWM(D11)=%d\n",
                SHIELD_LATCH_PIN, SHIELD_CLK_PIN, SHIELD_DATA_PIN, SHIELD_ENABLE_PIN, M1_PWM_PIN);

  pinMode(SHIELD_LATCH_PIN, OUTPUT);
  pinMode(SHIELD_CLK_PIN, OUTPUT);
  pinMode(SHIELD_DATA_PIN, OUTPUT);
  pinMode(SHIELD_ENABLE_PIN, OUTPUT);
  digitalWrite(SHIELD_ENABLE_PIN, LOW);

  ledcAttach(M1_PWM_PIN, PWM_FREQUENCY, PWM_RESOLUTION);

  latchTransmit();

  stopMotor();
  applySpeed(currentSpeed);

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

  server.on("/forward", HTTP_GET, []() {
    forwardMotor();
    server.send(200, "text/plain", "Motor running forward");
  });

  server.on("/backward", HTTP_GET, []() {
    backwardMotor();
    server.send(200, "text/plain", "Motor running backward");
  });

  server.on("/stop", HTTP_GET, []() {
    stopMotor();
    server.send(200, "text/plain", "Motor stopped");
  });

  server.on("/setspeed", HTTP_GET, []() {
    if (server.hasArg("val")) {
      int speed = server.arg("val").toInt();
      applySpeed(speed);
      Serial.println("Speed updated to: " + String(currentSpeed));
    }
    server.send(200, "text/plain", "Speed updated");
  });

  server.begin();
}

void loop() {
  server.handleClient();
}
