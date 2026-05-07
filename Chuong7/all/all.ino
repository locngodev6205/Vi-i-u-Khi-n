#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>
#include <DHT.h>
#include <Stepper.h>

const char* ssid     = "Tầng 2- nha tro Bach Khoa";
const char* password = "12345678910";

WebServer server(80);
Servo servo;

// ── Bật/tắt động cơ ───────────────────────────────────────────────────────
const bool USE_SERVO   = false;
const bool USE_STEPPER = false;
const bool USE_MOTOR_L298N = true;

// ── Bật/tắt cảm biến ──────────────────────────────────────────────────────
const bool USE_DHT        = false;
const bool USE_LDR        = false;
const bool USE_GAS        = false;
const bool USE_LINE       = false;
const bool USE_JOYSTICK   = true;
const bool USE_ULTRASONIC = true; // sieu am

// ── Chân GPIO ──────────────────────────────────────────────────────────────
const int SERVO_PIN = 13;

// Stepper ULN2003 - thứ tự chân: IN1, IN3, IN2, IN4
const int STEP_IN1 = 8, STEP_IN2 = 10, STEP_IN3 = 9, STEP_IN4 = 11;
const int STEPS_PER_REV = 2038;
Stepper myStepper(STEPS_PER_REV, STEP_IN1, STEP_IN2, STEP_IN3, STEP_IN4);

// L298N DC motor
const int MOTOR_ENA = 18;
const int MOTOR_IN1 = 16;
const int MOTOR_IN2 = 17;

const int DHT_PIN  = 4;
const int LDR_PIN  = 5;
const int GAS_PIN  = 6;
const int LINE_PIN = 7;
const int JOY_X    = 1;
const int JOY_Y    = 2;
const int JOY_SW   = 3;
const int TRIG_PIN = 14;
const int ECHO_PIN = 15;

DHT dht(DHT_PIN, DHT11);

// ── Servo state ────────────────────────────────────────────────────────────
volatile int  currentAngle = 0;
volatile bool servoRunning = false;
volatile int  targetAngle  = 90;
TaskHandle_t  servoTaskHandle = NULL;

void servoTask(void* param) {
  while (true) {
    if (servoRunning) {
      servo.write(targetAngle); delay(500); currentAngle = targetAngle;
      servo.write(0);           delay(500); currentAngle = 0;
    } else {
      vTaskDelay(pdMS_TO_TICKS(20));
    }
  }
}

// ── Stepper state ──────────────────────────────────────────────────────────
volatile bool stepperRunning = false;
volatile int  stepperDir     = 1;    // 1 = thuận, -1 = ngược
volatile int  stepperSpeed   = 10;   // RPM
TaskHandle_t  stepperTaskHandle = NULL;

void stepperTask(void* param) {
  while (true) {
    if (stepperRunning) {
      myStepper.setSpeed(stepperSpeed);
      // Quay từng bước nhỏ để vẫn check được stepperRunning
      myStepper.step(stepperDir * 10);
      // Nhường CPU để tránh watchdog reset (Stepper.step() là hàm blocking/busy-wait)
      vTaskDelay(pdMS_TO_TICKS(1));
    } else {
      vTaskDelay(pdMS_TO_TICKS(20));
    }
  }
}

// ── Siêu âm ───────────────────────────────────────────────────────────────
float readUltrasonic() {
  digitalWrite(TRIG_PIN, LOW);  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long d = pulseIn(ECHO_PIN, HIGH, 30000);
  return d == 0 ? -1 : d * 0.034 / 2.0;
}

// ── HTML ───────────────────────────────────────────────────────────────────
String getHTML() {
  String flags = "{";
  flags += "\"useServo\":"+   String(USE_SERVO   ?"true":"false")+",";
  flags += "\"useStepper\":"+String(USE_STEPPER?"true":"false")+",";
  flags += "\"useMotorL298N\":"+String(USE_MOTOR_L298N?"true":"false")+",";
  flags += "\"dht\":"+       String(USE_DHT      ?"true":"false")+",";
  flags += "\"ldr\":"+       String(USE_LDR      ?"true":"false")+",";
  flags += "\"gas\":"+       String(USE_GAS      ?"true":"false")+",";
  flags += "\"line\":"+      String(USE_LINE     ?"true":"false")+",";
  flags += "\"joystick\":"+  String(USE_JOYSTICK ?"true":"false")+",";
  flags += "\"ultrasonic\":"+String(USE_ULTRASONIC?"true":"false");
  flags += "}";

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
  h3{font-size:.95rem;color:#aaa;text-transform:uppercase;letter-spacing:.08em}
  .section{width:100%;max-width:600px;background:#1a1a2e;border-radius:18px;padding:18px;display:flex;flex-direction:column;gap:12px}
  .row{display:flex;gap:10px;flex-wrap:wrap;align-items:center}
  .val{font-size:2.2rem;font-weight:700;min-width:100px;text-align:center;
       background:#0d0d1a;border-radius:12px;padding:8px 16px}
  input[type=number],input[type=range]{background:#0d0d1a;border:1px solid #333;
       border-radius:10px;padding:10px 14px;font-size:.9rem;color:#eee;outline:none}
  input[type=number]{width:150px}
  input[type=range]{width:180px;padding:6px}
  button{border:none;border-radius:10px;padding:11px 18px;font-size:.9rem;
         font-weight:700;cursor:pointer;background:#1968d2;color:#fff}
  button.stop{background:#c0392b}
  button.rev{background:#7a4d1e}
  .sensors{display:flex;gap:12px;flex-wrap:wrap;justify-content:center;width:100%;max-width:600px}
  .scard{flex:1;min-width:130px;background:#1a1a2e;border-radius:14px;padding:14px;text-align:center}
  .slabel{font-size:.72rem;color:#888;text-transform:uppercase;letter-spacing:.07em;margin-bottom:6px}
  .sval{font-size:1.6rem;font-weight:700}
  .ssub{font-size:.78rem;color:#666;margin-top:4px}
  .badge{display:inline-block;padding:3px 10px;border-radius:999px;font-size:.75rem;font-weight:700;margin-top:6px}
  .badge.on{background:#1a4a1a;color:#4cff4c}
  .badge.off{background:#4a1a1a;color:#ff4c4c}
  #msg{font-size:.85rem;color:#666;min-height:1.1em}
</style>
</head>
<body>
<h2>ESP32 Control Panel</h2>
<div id="msg">San sang.</div>
<div id="motorSections"></div>
<div class="sensors" id="sensorGrid"></div>

<script>
const F = )rawliteral";
  html += flags;
  html += R"rawliteral(;
const msgEl = document.getElementById('msg');

function post(url) {
  fetch(url).then(r=>r.text()).then(t=>{ msgEl.textContent=t; });
}
function set(id,v){ const e=document.getElementById(id); if(e) e.textContent=v; }

// ── Build giao diện động cơ ───────────────────────────────────────────────
const ms = document.getElementById('motorSections');

if (F.useServo) {
  ms.innerHTML += `
  <div class="section">
    <h3>Servo</h3>
    <div class="row">
      <div class="val"><span id="servoAngle">0</span>°</div>
      <input id="servoInput" type="number" min="1" max="180" placeholder="Goc (1-180)"
             onkeydown="if(event.key==='Enter') servoStart()"/>
      <button onclick="servoStart()">Bat dau</button>
      <button class="stop" onclick="post('/servo/stop')">Stop</button>
    </div>
  </div>`;
}

if (F.useStepper) {
  ms.innerHTML += `
  <div class="section">
    <h3>Dong co buoc (28BYJ-48)</h3>
    <div class="row">
      <button onclick="post('/stepper/forward')">Thuan</button>
      <button class="rev" onclick="post('/stepper/reverse')">Nguoc</button>
      <button class="stop" onclick="post('/stepper/stop')">Stop</button>
    </div>
    <div class="row">
      <span style="font-size:.85rem;color:#aaa">Toc do (RPM):</span>
      <input type="range" min="1" max="15" value="10" id="speedSlider"
             oninput="updateSpeed(this.value)"/>
      <span id="speedVal">10</span> RPM
    </div>
  </div>`;
}

if (F.useMotorL298N) {
  ms.innerHTML += `
  <div class="section">
    <h3>Dong co DC L298N</h3>
    <div class="row">
      <button onclick="post('/motor/forward')">Thuan</button>
      <button class="rev" onclick="post('/motor/reverse')">Nguoc</button>
      <button class="stop" onclick="post('/motor/stop')">Dung</button>
    </div>
  </div>`;
}

// ── Build sensor cards ─────────────────────────────────────────────────────
const sg = document.getElementById('sensorGrid');
if (F.dht) {
  sg.innerHTML += `
    <div class="scard"><div class="slabel">Nhiet do</div><div class="sval"><span id="temp">--</span> °C</div></div>
    <div class="scard"><div class="slabel">Do am</div><div class="sval"><span id="humi">--</span> %</div></div>`;
}
if (F.ldr)
  sg.innerHTML += `<div class="scard"><div class="slabel">Anh sang</div><div class="sval"><span id="ldrRaw">--</span></div><div class="ssub">Raw ADC</div></div>`;
if (F.gas)
  sg.innerHTML += `<div class="scard"><div class="slabel">Gas/Khoi</div><div class="sval"><span id="gasRaw">--</span></div><div class="ssub">Raw ADC</div></div>`;
if (F.line)
  sg.innerHTML += `<div class="scard"><div class="slabel">Do line</div><div class="sval" id="lineVal">--</div><span class="badge" id="lineBadge">--</span></div>`;
if (F.joystick)
  sg.innerHTML += `<div class="scard"><div class="slabel">Joystick</div><div class="sval" id="joyVal">--</div><div class="ssub">SW: <span id="joySW">--</span></div></div>`;
if (F.ultrasonic)
  sg.innerHTML += `<div class="scard"><div class="slabel">Sieu am</div><div class="sval"><span id="dist">--</span> cm</div></div>`;

// ── Điều khiển ────────────────────────────────────────────────────────────
function servoStart() {
  const v = parseInt(document.getElementById('servoInput').value);
  if (isNaN(v)||v<1||v>180){ msgEl.textContent='Nhap goc 1-180!'; return; }
  post('/servo/set?val='+v);
}

function updateSpeed(v) {
  document.getElementById('speedVal').textContent = v;
  post('/stepper/speed?val='+v);
}

// ── Poll status ────────────────────────────────────────────────────────────
function update() {
  fetch('/status').then(r=>r.json()).then(d=>{
    if(F.useServo)   set('servoAngle', d.servoAngle);
    if(F.dht)      { set('temp', d.temp); set('humi', d.humi); }
    if(F.ldr)        set('ldrRaw', d.ldrRaw);
    if(F.gas)        set('gasRaw', d.gasRaw);
    if(F.line){
      set('lineVal', d.line===1?'DETECT':'CLEAR');
      const b=document.getElementById('lineBadge');
      if(b){ b.textContent=d.line===1?'Phat hien':'Trong'; b.className='badge '+(d.line===1?'on':'off'); }
    }
    if(F.joystick){ set('joyVal','X:'+d.joyX+' Y:'+d.joyY); set('joySW',d.joySW===0?'Nhan':'Tha'); }
    if(F.ultrasonic) set('dist', d.dist<0?'--':d.dist);
  });
}
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
  if (USE_JOYSTICK)  { pinMode(JOY_X,INPUT); pinMode(JOY_Y,INPUT); pinMode(JOY_SW,INPUT_PULLUP); }
  if (USE_ULTRASONIC){ pinMode(TRIG_PIN,OUTPUT); pinMode(ECHO_PIN,INPUT); }
  if (USE_MOTOR_L298N){ pinMode(MOTOR_ENA,OUTPUT); pinMode(MOTOR_IN1,OUTPUT); pinMode(MOTOR_IN2,OUTPUT); }

  if (USE_SERVO) {
    servo.setPeriodHertz(50);
    servo.attach(SERVO_PIN);
    servo.write(0);
    xTaskCreatePinnedToCore(servoTask, "servo", 4096, NULL, 1, &servoTaskHandle, 0);
  }

  if (USE_STEPPER) {
    myStepper.setSpeed(stepperSpeed);
    xTaskCreatePinnedToCore(stepperTask, "stepper", 4096, NULL, 1, &stepperTaskHandle, 0);
  }

  if (USE_MOTOR_L298N) {
    digitalWrite(MOTOR_ENA, LOW);
    digitalWrite(MOTOR_IN1, LOW);
    digitalWrite(MOTOR_IN2, LOW);
  }

  WiFi.begin(ssid, password);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print('.'); }
  Serial.println("IP: " + WiFi.localIP().toString());

  server.on("/", []() { server.send(200, "text/html", getHTML()); });

  // Servo routes
  server.on("/servo/set", []() {
    if (!server.hasArg("val")) { server.send(400,"text/plain","Missing val"); return; }
    targetAngle  = constrain(server.arg("val").toInt(), 1, 180);
    servoRunning = true;
    server.send(200,"text/plain","Servo: 0 <-> "+String(targetAngle)+" do");
  });
  server.on("/servo/stop", []() {
    servoRunning = false; servo.write(0); currentAngle = 0;
    server.send(200,"text/plain","Servo da dung");
  });

  // Stepper routes
  server.on("/stepper/forward", []() {
    stepperDir = 1; stepperRunning = true;
    server.send(200,"text/plain","Stepper: quay thuan");
  });
  server.on("/stepper/reverse", []() {
    stepperDir = -1; stepperRunning = true;
    server.send(200,"text/plain","Stepper: quay nguoc");
  });
  server.on("/stepper/stop", []() {
    stepperRunning = false;
    server.send(200,"text/plain","Stepper da dung");
  });
  server.on("/stepper/speed", []() {
    if (server.hasArg("val")) {
      stepperSpeed = constrain(server.arg("val").toInt(), 1, 15);
      myStepper.setSpeed(stepperSpeed);
    }
    server.send(200,"text/plain","Toc do: "+String(stepperSpeed)+" RPM");
  });

  // L298N motor routes
  server.on("/motor/forward", []() {
    if (!USE_MOTOR_L298N) { server.send(404,"text/plain","Motor disabled"); return; }
    digitalWrite(MOTOR_ENA, HIGH);
    digitalWrite(MOTOR_IN1, HIGH);
    digitalWrite(MOTOR_IN2, LOW);
    server.send(200,"text/plain","Motor: quay thuan");
  });
  server.on("/motor/reverse", []() {
    if (!USE_MOTOR_L298N) { server.send(404,"text/plain","Motor disabled"); return; }
    digitalWrite(MOTOR_ENA, HIGH);
    digitalWrite(MOTOR_IN1, LOW);
    digitalWrite(MOTOR_IN2, HIGH);
    server.send(200,"text/plain","Motor: quay nguoc");
  });
  server.on("/motor/stop", []() {
    if (!USE_MOTOR_L298N) { server.send(404,"text/plain","Motor disabled"); return; }
    digitalWrite(MOTOR_ENA, LOW);
    digitalWrite(MOTOR_IN1, LOW);
    digitalWrite(MOTOR_IN2, LOW);
    server.send(200,"text/plain","Motor: dung");
  });

  // Status
  server.on("/status", []() {
    String json = "{";
    json += F("\"servoAngle\":");
    json += String(currentAngle);
    if (USE_DHT) {
      float t=dht.readTemperature(), h=dht.readHumidity();
      json += F(",\"temp\":");
      json += (isnan(t) ? String("-1") : String(t,1));
      json += F(",\"humi\":");
      json += (isnan(h) ? String("-1") : String(h,1));
    }
    if (USE_LDR)  { json += F(",\"ldrRaw\":"); json += String(analogRead(LDR_PIN)); }
    if (USE_GAS)  { json += F(",\"gasRaw\":"); json += String(analogRead(GAS_PIN)); }
    if (USE_LINE) { json += F(",\"line\":");   json += String(digitalRead(LINE_PIN)); }
    if (USE_JOYSTICK) {
      json += F(",\"joyX\":");  json += String(analogRead(JOY_X));
      json += F(",\"joyY\":");  json += String(analogRead(JOY_Y));
      json += F(",\"joySW\":"); json += String(digitalRead(JOY_SW));
    }
    if (USE_ULTRASONIC) {
      json += F(",\"dist\":");
      json += String(readUltrasonic(),1);
    }
    json += "}";
    server.send(200,"application/json",json);
  });

  server.begin();
}

void loop() {
  server.handleClient();
}
