#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>
#include <DHT.h>

const char* ssid     = "Tầng 2- nha tro Bach Khoa";
const char* password = "12345678910";

WebServer server(80);
Servo servo;

// ── Chân GPIO ──────────────────────────────────────────────────────────────
const int SERVO_PIN = 13;

const int DHT_PIN   = 4;    // DHT11 DATA
const int LDR_PIN   = 5;    // LDR AO
const int GAS_PIN   = 6;    // MQ AO
const int LINE_PIN  = 7;    // TCRT5000 DO
const int JOY_X     = 1;    // Joystick VRX (AO)
const int JOY_Y     = 2;    // Joystick VRY (AO)
const int JOY_SW    = 3;    // Joystick SW (Digital, INPUT_PULLUP)
const int TRIG_PIN  = 14;    // HC-SR04 TRIG
const int ECHO_PIN  = 15;   // HC-SR04 ECHO

// ── Bật/tắt từng cảm biến (true = hiện trên web) ──────────────────────────
const bool USE_DHT      = true;
const bool USE_LDR      = true;
const bool USE_GAS      = true;
const bool USE_LINE     = true;
const bool USE_JOYSTICK = true;
const bool USE_ULTRASONIC = true;

DHT dht(DHT_PIN, DHT11);

// ── Servo state ────────────────────────────────────────────────────────────
volatile int  currentAngle = 0;
volatile bool isRunning    = false;
volatile int  targetAngle  = 90;
TaskHandle_t  servoTaskHandle = NULL;

void servoTask(void* param) {
  while (true) {
    if (isRunning) {
      servo.write(targetAngle);
      delay(500);
      currentAngle = targetAngle;
      servo.write(0);
      delay(500);
      currentAngle = 0;
    } else {
      vTaskDelay(pdMS_TO_TICKS(20));
    }
  }
}

// ── Đọc siêu âm ───────────────────────────────────────────────────────────
float readUltrasonic() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH, 30000); // timeout 30ms (~5m)
  if (duration == 0) return -1;
  return duration * 0.034 / 2.0;
}

// ── HTML ───────────────────────────────────────────────────────────────────
String getHTML() {
  // Build danh sách sensor JSON để JS biết cái nào được bật
  String enabled = "{";
  enabled += "\"dht\":"       + String(USE_DHT       ? "true" : "false") + ",";
  enabled += "\"ldr\":"       + String(USE_LDR       ? "true" : "false") + ",";
  enabled += "\"gas\":"       + String(USE_GAS       ? "true" : "false") + ",";
  enabled += "\"line\":"      + String(USE_LINE      ? "true" : "false") + ",";
  enabled += "\"joystick\":"  + String(USE_JOYSTICK  ? "true" : "false") + ",";
  enabled += "\"ultrasonic\":" + String(USE_ULTRASONIC ? "true" : "false");
  enabled += "}";

  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="vi">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>ESP32 Control</title>
<style>
  *{box-sizing:border-box;margin:0;padding:0}
  body{font-family:Arial,sans-serif;background:#111;color:#eee;
       display:flex;flex-direction:column;align-items:center;
       padding:28px 16px;gap:20px;min-height:100vh}
  h2{font-size:1.3rem;letter-spacing:.06em;color:#7eb3ff}
  .servo-angle{font-size:2.8rem;font-weight:700;background:#1a1a2e;
               border-radius:14px;padding:12px 32px;min-width:140px;text-align:center}
  .form{display:flex;gap:10px;flex-wrap:wrap;justify-content:center;align-items:center}
  input[type=number]{background:#1a1a2e;border:1px solid #333;border-radius:12px;
                     padding:13px 16px;font-size:.95rem;color:#eee;width:180px;outline:none}
  input[type=number]:focus{border-color:#1968d2}
  button{border:none;border-radius:12px;padding:13px 20px;font-size:.95rem;
         font-weight:700;cursor:pointer;background:#1968d2;color:#fff}
  button.stop{background:#c0392b}
  .sensors{display:flex;gap:14px;flex-wrap:wrap;justify-content:center;width:100%;max-width:600px}
  .scard{flex:1;min-width:140px;background:#1a1a2e;border-radius:16px;padding:16px;text-align:center}
  .slabel{font-size:.75rem;color:#888;text-transform:uppercase;letter-spacing:.07em;margin-bottom:8px}
  .sval{font-size:1.7rem;font-weight:700;line-height:1.2}
  .ssub{font-size:.8rem;color:#666;margin-top:4px}
  .bar-bg{width:100%;height:8px;background:#2a2a3e;border-radius:4px;margin-top:8px;overflow:hidden}
  .bar-fill{height:100%;border-radius:4px;transition:width .4s}
  .c-temp .bar-fill{background:#e07840}
  .c-humi .bar-fill{background:#40a0e0}
  .c-ldr  .bar-fill{background:#f0c040}
  .c-gas  .bar-fill{background:#e05050}
  .c-dist .bar-fill{background:#a040e0}
  .badge{display:inline-block;padding:3px 10px;border-radius:999px;
         font-size:.75rem;font-weight:700;margin-top:6px}
  .badge.on {background:#1a4a1a;color:#4cff4c}
  .badge.off{background:#4a1a1a;color:#ff4c4c}
  #msg{font-size:.85rem;color:#666;min-height:1.1em}
</style>
</head>
<body>
<h2>ESP32 Control Panel</h2>

<div class="servo-angle"><span id="angle">--</span> °</div>
<div class="form">
  <input id="angleInput" type="number" min="1" max="180" placeholder="Nhap goc (1-180)"
         onkeydown="if(event.key==='Enter') cmd()" />
  <button onclick="cmd()">Bat dau</button>
  <button class="stop" onclick="cmdStop()">Stop</button>
</div>
<div id="msg">San sang.</div>

<div class="sensors" id="sensorGrid"></div>

<script>
const ENABLED = )rawliteral";

  html += enabled;
  html += R"rawliteral(;

const angleEl = document.getElementById('angle');
const msgEl   = document.getElementById('msg');
const grid    = document.getElementById('sensorGrid');

// Tạo card động theo sensor được bật
function buildCards() {
  if (ENABLED.dht) {
    grid.innerHTML += `
      <div class="scard c-temp">
        <div class="slabel">Nhiet do</div>
        <div class="sval"><span id="temp">--</span> °C</div>
      </div>
      <div class="scard c-humi">
        <div class="slabel">Do am</div>
        <div class="sval"><span id="humi">--</span> %</div>
      </div>`;
  }
  if (ENABLED.ldr) {
    grid.innerHTML += `
      <div class="scard c-ldr">
        <div class="slabel">Anh sang</div>
        <div class="sval"><span id="ldrRaw">--</span></div>
        <div class="ssub">Raw ADC (0-4095)</div>
      </div>`;
  }
  if (ENABLED.gas) {
    grid.innerHTML += `
      <div class="scard c-gas">
        <div class="slabel">Gas / Khoi</div>
        <div class="sval"><span id="gasRaw">--</span></div>
        <div class="ssub">Raw ADC (0-4095)</div>
      </div>`;
  }
  if (ENABLED.line) {
    grid.innerHTML += `
      <div class="scard">
        <div class="slabel">Do line</div>
        <div class="sval" id="lineVal">--</div>
        <span class="badge" id="lineBadge">--</span>
      </div>`;
  }
  if (ENABLED.joystick) {
    grid.innerHTML += `
      <div class="scard">
        <div class="slabel">Joystick</div>
        <div class="sval" id="joyVal">--</div>
        <div class="ssub">SW: <span id="joySW">--</span></div>
      </div>`;
  }
  if (ENABLED.ultrasonic) {
    grid.innerHTML += `
      <div class="scard c-dist">
        <div class="slabel">Sieu am</div>
        <div class="sval"><span id="dist">--</span> cm</div>
      </div>`;
  }
}

function cmd() {
  const val = parseInt(document.getElementById('angleInput').value);
  if (isNaN(val) || val < 1 || val > 180) {
    msgEl.textContent = 'Nhap goc hop le tu 1 den 180!'; return;
  }
  fetch('/set?val=' + val).then(r => r.text()).then(t => { msgEl.textContent = t; });
}

function cmdStop() {
  fetch('/stop').then(r => r.text()).then(t => { msgEl.textContent = t; });
}

function set(id, val) { const el = document.getElementById(id); if(el) el.textContent = val; }
function setW(id, pct){ const el = document.getElementById(id); if(el) el.style.width = pct + '%'; }

function update() {
  fetch('/status').then(r => r.json()).then(d => {
    angleEl.textContent = d.angle;

    if (ENABLED.dht) {
      set('temp', d.temp);
      set('humi', d.humi);
    }
    if (ENABLED.ldr) {
      set('ldrRaw', d.ldrRaw);
    }
    if (ENABLED.gas) {
      set('gasRaw', d.gasRaw);
    }
    if (ENABLED.line) {
      const on = d.line === 1;
      set('lineVal', on ? 'DETECT' : 'CLEAR');
      const b = document.getElementById('lineBadge');
      if(b){ b.textContent = on ? 'Phat hien' : 'Trong'; b.className = 'badge ' + (on?'on':'off'); }
    }
    if (ENABLED.joystick) {
      set('joyVal', 'X:' + d.joyX + ' Y:' + d.joyY);
      set('joySW', d.joySW === 0 ? 'Nhan' : 'Tha');
    }
    if (ENABLED.ultrasonic) {
      set('dist', d.dist < 0 ? '--' : d.dist);

    }
  });
}

buildCards();
setInterval(update, 500);
update();
</script>
</body>
</html>
)rawliteral";
  return html;
}

// ── Setup ──────────────────────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);

  if (USE_DHT)       dht.begin();
  if (USE_LDR)       pinMode(LDR_PIN,  INPUT);
  if (USE_GAS)       pinMode(GAS_PIN,  INPUT);
  if (USE_LINE)      pinMode(LINE_PIN, INPUT);
  if (USE_JOYSTICK) {
    pinMode(JOY_X,  INPUT);
    pinMode(JOY_Y,  INPUT);
    pinMode(JOY_SW, INPUT_PULLUP);
  }
  if (USE_ULTRASONIC) {
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
  }

  servo.setPeriodHertz(50);
  servo.attach(SERVO_PIN);
  servo.write(0);

  xTaskCreatePinnedToCore(servoTask, "servo", 4096, NULL, 1,
                          &servoTaskHandle, 0);

  WiFi.begin(ssid, password);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print('.'); }
  Serial.println("\nIP: " + WiFi.localIP().toString());

  server.on("/", []() {
    server.send(200, "text/html", getHTML());
  });

  server.on("/set", []() {
    if (!server.hasArg("val")) { server.send(400, "text/plain", "Missing val"); return; }
    targetAngle = constrain(server.arg("val").toInt(), 1, 180);
    isRunning   = true;
    server.send(200, "text/plain", "Dang quay 0 <-> " + String(targetAngle) + " do");
  });

  server.on("/stop", []() {
    isRunning = false;
    servo.write(0);
    currentAngle = 0;
    server.send(200, "text/plain", "Da dung");
  });

  server.on("/status", []() {
    String json = "{";
    json += "\"angle\":" + String(currentAngle);

    if (USE_DHT) {
      float temp = dht.readTemperature();
      float humi = dht.readHumidity();
      json += ",\"temp\":"  + (isnan(temp) ? String("-1") : String(temp, 1));
      json += ",\"humi\":"  + (isnan(humi) ? String("-1") : String(humi, 1));
    }
    if (USE_LDR) {
      int raw = analogRead(LDR_PIN);
      json += ",\"ldrRaw\":" + String(raw);
    }
    if (USE_GAS) {
      int raw = analogRead(GAS_PIN);
      json += ",\"gasRaw\":" + String(raw);
    }
    if (USE_LINE) {
      json += ",\"line\":" + String(digitalRead(LINE_PIN));
    }
    if (USE_JOYSTICK) {
      json += ",\"joyX\":"  + String(analogRead(JOY_X));
      json += ",\"joyY\":"  + String(analogRead(JOY_Y));
      json += ",\"joySW\":" + String(digitalRead(JOY_SW));
    }
    if (USE_ULTRASONIC) {
      float dist = readUltrasonic();
      json += ",\"dist\":" + String(dist, 1);
    }

    json += "}";
    server.send(200, "application/json", json);
  });

  server.begin();
}

void loop() {
  server.handleClient();
}
