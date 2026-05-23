#include <WiFi.h>
#include <WebServer.h>

// 1. CẤU HÌNH
const char* ssid     = "Thasi";
const char* password = "123456789";

// Cấu hình chân Joystick
const int VRX_PIN = 34; // Trục X 
const int VRY_PIN = 35; // Trục Y 
const int SW_PIN  = 21; // Nút nhấn (Digital)

// Cấu hình chân cho L298N / Motor DC
const int IN1_PIN = 16; // Chân chiều 1
const int IN2_PIN = 17; // Chân chiều 2
const int ENA_PIN = 18; // Chân (PWM) điều khiển tốc độ

WebServer server(80);

// 2. BIẾN TRẠNG THÁI HỆ THỐNG
bool motorRunning   = false;
int  speedInput     = 150;    // Tốc độ mặc định (0 - 255)
int  motorDirection = 1;      // 1: Thuận, -1: Ngược

// 3. GIAO DIỆN WEB
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset='UTF-8'>
  <title>ESP32 DC Motor Control</title>
  <style> body { font-family: Arial; padding: 20px; } </style>
</head>
<body>
  <h2>ESP32 - JOYSTICK & ĐỘNG CƠ DC</h2>
  <hr>
  <p><b>Trục X (Trái/Phải):</b> <span id='xValue'>0</span></p>
  <p><b>Trục Y (Lên/Xuống):</b> <span id='yValue'>0</span></p>
  <p><b>Nút nhấn (SW):</b> <span id='btnValue'>Chưa nhấn</span></p>
  <p><b>Trạng thái động cơ:</b> <span id='motorState'>Đã dừng</span></p>
  <hr>
  <form action='/start' method='GET'>
    <label>Tốc độ quay (0 - 255):</label><br>
    <input type='number' name='speed' min='0' max='255' value='%SPEED%'><br><br>
    
    <label>Chiều quay:</label><br>
    <input type='radio' name='dir' value='1' %CHECKED_FWD%> Quay thuận<br>
    <input type='radio' name='dir' value='-1' %CHECKED_BWD%> Quay ngược<br><br>
    
    <input type='submit' value='BẮT ĐẦU CHẠY'>
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

// 4. CÁC HÀM XỬ LÝ ĐỘNG CƠ & WEB

// Hàm dừng động cơ
void stopMotor() {
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, LOW);
  analogWrite(ENA_PIN, 0); 
}

// Hàm chạy động cơ với tốc độ và chiều xác định
void runMotor(int speed, int dir) {
  if (dir == 1) {
    digitalWrite(IN1_PIN, HIGH);
    digitalWrite(IN2_PIN, LOW);
  } else {
    digitalWrite(IN1_PIN, LOW);
    digitalWrite(IN2_PIN, HIGH);
  }
  analogWrite(ENA_PIN, speed); // Xuất xung PWM để chỉnh tốc độ
}

// Trả về JSON trạng thái
void handleStatus() {
  // Đọc giá trị Analog của trục X và Y (từ 0 đến 4095 trên ESP32)
  int xVal = analogRead(VRX_PIN);
  int yVal = analogRead(VRY_PIN);
  
  // Đọc trạng thái nút nhấn (Nối INPUT_PULLUP nên nhấn xuống sẽ là LOW)
  int swVal = digitalRead(SW_PIN);
  String btnState = (swVal == LOW) ? "Đang nhấn (LOW)" : "Nhả (HIGH)";

  String state = motorRunning ? (motorDirection == 1 ? "Đang quay thuận (Tốc độ: " + String(speedInput) + ")" : "Đang quay ngược (Tốc độ: " + String(speedInput) + ")") : "Đã dừng";
  
  // Đóng gói JSON gửi về Web
  String json = "{\"x\":\"" + String(xVal) + "\", " +
                "\"y\":\"" + String(yVal) + "\", " +
                "\"btn\":\"" + btnState + "\", " +
                "\"motor\":\"" + state + "\"}";
  server.send(200, "application/json", json);
}

// Xử lý khi nhấn nút BẮT ĐẦU
void handleStart() {
  if (server.hasArg("speed")) speedInput = server.arg("speed").toInt();
  if (server.hasArg("dir"))    motorDirection = server.arg("dir").toInt();

  // Giới hạn tốc độ trong khoảng 0-255 để tránh lỗi
  speedInput = constrain(speedInput, 0, 255);
  if (motorDirection != 1 && motorDirection != -1) motorDirection = 1;

  motorRunning = true;
  runMotor(speedInput, motorDirection); // Kích hoạt động cơ ngay lập tức

  server.sendHeader("Location", "/");
  server.send(303); 
}

// Xử lý khi nhấn nút DỪNG
void handleStop() {
  motorRunning = false;
  stopMotor();
  server.sendHeader("Location", "/");
  server.send(303);
}

// Hàm gửi trang Web giao diện
void handleRoot() {
  String html = String(index_html);
  
  html.replace("%SPEED%", String(speedInput));
  html.replace("%CHECKED_FWD%", motorDirection == 1 ? "checked" : "");
  html.replace("%CHECKED_BWD%", motorDirection == -1 ? "checked" : "");
  
  server.send(200, "text/html", html);
}

// 5. SETUP & LOOP
void setup() {
  Serial.begin(9600);

  // Cấu hình chân Joystick
  // Sử dụng INPUT_PULLUP cho nút nhấn để không cần điện trở ngoài
  pinMode(SW_PIN, INPUT_PULLUP);
  analogReadResolution(12); // ESP32 đọc Analog từ 0 - 4095

  // Cấu hình chân động cơ
  pinMode(IN1_PIN, OUTPUT);
  pinMode(IN2_PIN, OUTPUT);
  pinMode(ENA_PIN, OUTPUT);

  // Đảm bảo động cơ dừng lúc khởi động
  stopMotor();

  // Kết nối WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Dang ket noi WiFi");
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println("\nIP Web Server: " + WiFi.localIP().toString());

  // Định tuyến Web
  server.on("/", handleRoot);
  server.on("/status", handleStatus);
  server.on("/start", handleStart);
  server.on("/stop", handleStop);
  server.begin();
}

void loop() {
  server.handleClient();
}