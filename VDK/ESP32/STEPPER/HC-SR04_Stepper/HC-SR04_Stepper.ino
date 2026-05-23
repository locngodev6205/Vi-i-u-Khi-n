#include <WiFi.h>
#include <WebServer.h>
#include <Stepper.h>

// 1. CẤU HÌNH
const char* ssid     = "Thasi";
const char* password = "123456789";

// Cấu hình cảm biến siêu âm HC-SR04
const int TRIG_PIN = 21; // Chân phát sóng (Output)
const int ECHO_PIN = 22; // Chân nhận sóng (Input)

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
  <h2>ESP32 DEVKIT V1 - HC-SR04 & STEPPER</h2>
  <hr>
  <p><b>Khoảng cách:</b> <span id='distValue'>Đang cập nhật...</span></p>
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
    // Cập nhật mỗi 500ms
    setInterval(() => {
      fetch('/status')
        .then(res => res.json())
        .then(data => {
          document.getElementById('distValue').innerText = data.dist;
          document.getElementById('motorState').innerText = data.motor;
        });
    }, 500); 
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
  // Phát xung từ chân TRIG
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Đọc thời gian xung phản hồi từ chân ECHO
  // Sử dụng timeout 30000 micro-giây (30ms) để server không bị treo nếu không thấy vật cản
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  
  String distStr;
  if (duration == 0) {
    distStr = "Ngoai tam do"; // Trả về nếu không có tín hiệu dội lại
  } else {
    // Tính khoảng cách (cm)
    float distance = duration * 0.034 / 2;
    distStr = String(distance, 1) + " cm"; // In ra kèm đơn vị cm, làm tròn 1 chữ số
  }

  String state = motorRunning ? (motorDirection == 1 ? "Dang quay thuan" : "Dang quay nguoc") : "Da dung";
  
  // Tạo chuỗi JSON gửi về client
  String json = "{\"dist\":\"" + distStr + "\", \"motor\":\"" + state + "\"}";
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

  // Khởi tạo chân HC-SR04
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

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