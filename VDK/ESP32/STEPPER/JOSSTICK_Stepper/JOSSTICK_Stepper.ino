#include <WiFi.h>
#include <WebServer.h>
#include <Stepper.h>

// 1. CẤU HÌNH
const char* ssid     = "Thasi";
const char* password = "123456789";

// Cấu hình Joystick
const int VRX_PIN = 34; // Trục X 
const int VRY_PIN = 35; // Trục Y 
const int SW_PIN  = 21; // Nút nhấn 

const int STEPS_PER_REV = 2048;
Stepper myStepper(STEPS_PER_REV, 16, 18, 17, 19);

WebServer server(80);

// 2. BIẾN TRẠNG THÁI HỆ THỐNG
bool motorRunning   = false;
int  degreeInput    = 60;
int  motorDirection = 1;      // 1: Thuận, -1: Ngược
int  steps          = 0;      // Số bước cần quay

// 3. GIAO DIỆN WEB
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset='UTF-8'>
  <title>ESP32 Control</title>
  <style> body { font-family: Arial; padding: 20px; } </style>
</head>
<body>
  <h2>ESP32 DEVKIT V1 - JOYSTICK & STEPPER</h2>
  <hr>
  <p><b>Trục X (Trái/Phải):</b> <span id='xValue'>0</span></p>
  <p><b>Trục Y (Lên/Xuống):</b> <span id='yValue'>0</span></p>
  <p><b>Nút nhấn (SW):</b> <span id='btnValue'>Chưa nhấn</span></p>
  <p><b>Trạng thái động cơ:</b> <span id='motorState'>Đã dừng</span></p>
  <hr>
  <form action='/start' method='GET'>
    <label>Góc quay mỗi lần:</label><br>
    <input type='number' name='degree' min='1' max='360' value='%DEGREE%'><br><br>
    
    <label>Chiều quay:</label><br>
    <input type='radio' name='dir' value='1' %CHECKED_FWD%> Quay thuận<br>
    <input type='radio' name='dir' value='-1' %CHECKED_BWD%> Quay ngược<br><br>
    
    <input type='submit' value='BẮT ĐẦU QUAY'>
    <button type='button' onclick="location.href='/stop'">DỪNG ĐỘNG CƠ</button>
  </form>

  <script>
    // Cập nhật mỗi 200ms để theo dõi Joystick mượt mà hơn
    setInterval(() => {
      fetch('/status')
        .then(res => res.json())
        .then(data => {
          document.getElementById('xValue').innerText = data.x;
          document.getElementById('yValue').innerText = data.y;
          document.getElementById('btnValue').innerText = data.btn;
          document.getElementById('motorState').innerText = data.motor;
        });
    }, 200); 
  </script>
</body>
</html>
)rawliteral";

// 4. CÁC HÀM XỬ LÝ 
void releaseStepper() {
  digitalWrite(16, LOW); digitalWrite(17, LOW);
  digitalWrite(18, LOW); digitalWrite(19, LOW);
}

void handleStatus() {
  // Đọc giá trị Analog của trục X và Y (từ 0 đến 4095 trên ESP32)
  int xVal = analogRead(VRX_PIN);
  int yVal = analogRead(VRY_PIN);
  
  // Đọc trạng thái nút nhấn (Nối INPUT_PULLUP nên nhấn xuống sẽ là LOW)
  int swVal = digitalRead(SW_PIN);
  String btnState = (swVal == LOW) ? "Đang nhấn (LOW)" : "Nhả (HIGH)";

  String state = motorRunning ? (motorDirection == 1 ? "Dang quay thuan" : "Dang quay nguoc") : "Da dung";
  
  // Đóng gói JSON gửi về Web
  String json = "{\"x\":\"" + String(xVal) + "\", " +
                "\"y\":\"" + String(yVal) + "\", " +
                "\"btn\":\"" + btnState + "\", " +
                "\"motor\":\"" + state + "\"}";
  server.send(200, "application/json", json);
}

void handleStart() {
  if (server.hasArg("degree")) degreeInput = server.arg("degree").toInt();
  if (server.hasArg("dir"))    motorDirection = server.arg("dir").toInt();

  degreeInput = constrain(degreeInput, 1, 360);
  if (motorDirection != 1 && motorDirection != -1) motorDirection = 1;

  steps = map(degreeInput, 0, 360, 0, STEPS_PER_REV);
  if (steps < 1) steps = 1;

  motorRunning = true;
  server.sendHeader("Location", "/");
  server.send(303); 
}

void handleStop() {
  motorRunning = false;
  releaseStepper();
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleRoot() {
  String html = String(index_html);
  
  html.replace("%DEGREE%", String(degreeInput));
  html.replace("%CHECKED_FWD%", motorDirection == 1 ? "checked" : "");
  html.replace("%CHECKED_BWD%", motorDirection == -1 ? "checked" : "");
  
  server.send(200, "text/html", html);
}

// 5. SETUP & LOOP
void setup() {
  Serial.begin(9600);

  // Cấu hình chân nút nhấn có sẵn điện trở kéo lên
  pinMode(SW_PIN, INPUT_PULLUP);
  
  // ESP32 có độ phân giải ADC 12-bit (0 - 4095)
  analogReadResolution(12);

  for (int pin = 16; pin <= 19; pin++) {
    pinMode(pin, OUTPUT);
  }

  releaseStepper();
  myStepper.setSpeed(15);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Dang ket noi WiFi");
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println("\nIP Web Server: " + WiFi.localIP().toString());

  server.on("/", handleRoot);
  server.on("/status", handleStatus);
  server.on("/start", handleStart);
  server.on("/stop", handleStop);
  server.begin();
}

void loop() {
  server.handleClient();

  if (motorRunning) {
    myStepper.step(-steps * motorDirection);
    delay(100);
  }
}