#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

// 1. CẤU HÌNH
const char* ssid     = "Thasi";
const char* password = "123456789";

// Cấu hình Cảm biến Joystick
const int VRX_PIN = 34; // Trục X 
const int VRY_PIN = 35; // Trục Y
const int SW_PIN  = 21; // Nút nhấn (Digital)

// Cấu hình Servo
const int SERVO_PIN = 18; 

Servo myServo;
WebServer server(80);

// 2. BIẾN TRẠNG THÁI HỆ THỐNG
bool motorRunning   = false;
int  degreeInput    = 90;     // Góc quay mặc định
int  delayInput     = 500;    // Thời gian chờ (ms) mặc định

// Biến phục vụ cho hàm millis() (Non-blocking)
unsigned long previousMillis = 0;
bool isAtMaxAngle = false; 

// 3. GIAO DIỆN WEB
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset='UTF-8'>
  <title>ESP32 Servo Control</title>
  <style> body { font-family: Arial; padding: 20px; } </style>
</head>
<body>
  <h2>ESP32 - JOYSTICK & SERVO</h2>
  <hr>
  <p><b>Trục X (Trái/Phải):</b> <span id='xValue'>0</span></p>
  <p><b>Trục Y (Lên/Xuống):</b> <span id='yValue'>0</span></p>
  <p><b>Nút nhấn (SW):</b> <span id='btnValue'>Chưa nhấn</span></p>
  <p><b>Trạng thái Servo:</b> <span id='motorState'>Đã dừng</span></p>
  <hr>
  <form action='/start' method='GET'>
    <label>Góc quay (Độ):</label><br>
    <input type='number' name='degree' min='1' max='180' value='%DEGREE%'><br><br>
    
    <label>Thời gian trễ (ms):</label><br>
    <input type='number' name='delay' min='100' max='5000' value='%DELAY%'><br><br>
    
    <input type='submit' value='BẮT ĐẦU QUÉT'>
    <button type='button' onclick="location.href='/stop'">DỪNG SERVO</button>
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

// Trả về JSON trạng thái
void handleStatus() {
  // Đọc giá trị Analog của trục X và Y (từ 0 đến 4095 trên ESP32)
  int xVal = analogRead(VRX_PIN);
  int yVal = analogRead(VRY_PIN);
  
  // Đọc trạng thái nút nhấn (Nối INPUT_PULLUP nên nhấn xuống sẽ là LOW)
  int swVal = digitalRead(SW_PIN);
  String btnState = (swVal == LOW) ? "Đang nhấn (LOW)" : "Nhả (HIGH)";

  String state = motorRunning ? "Đang quét liên tục (0 - " + String(degreeInput) + " độ)" : "Đã dừng (Về 0 độ)";
  
  // Đóng gói JSON gửi về Web
  String json = "{\"x\":\"" + String(xVal) + "\", " +
                "\"y\":\"" + String(yVal) + "\", " +
                "\"btn\":\"" + btnState + "\", " +
                "\"motor\":\"" + state + "\"}";
  server.send(200, "application/json", json);
}

// Xử lý khi nhấn nút BẮT ĐẦU
void handleStart() {
  if (server.hasArg("degree")) degreeInput = server.arg("degree").toInt();
  if (server.hasArg("delay"))  delayInput  = server.arg("delay").toInt();

  // Giới hạn giá trị an toàn
  degreeInput = constrain(degreeInput, 1, 180);
  delayInput  = constrain(delayInput, 100, 5000);

  motorRunning = true;
  previousMillis = millis(); // Đặt lại bộ đếm thời gian
  isAtMaxAngle = false;
  myServo.write(degreeInput); // Cho Servo chạy ngay vòng đầu tiên
  isAtMaxAngle = true;

  server.sendHeader("Location", "/");
  server.send(303); 
}

// Xử lý khi nhấn nút DỪNG
void handleStop() {
  motorRunning = false;
  myServo.write(0); // Trả Servo về vị trí 0 độ khi dừng
  server.sendHeader("Location", "/");
  server.send(303);
}

// Hàm gửi trang Web giao diện
void handleRoot() {
  String html = String(index_html);
  
  html.replace("%DEGREE%", String(degreeInput));
  html.replace("%DELAY%", String(delayInput));
  
  server.send(200, "text/html", html);
}

// 5. SETUP & LOOP
void setup() {
  Serial.begin(9600);

  // Cấu hình chân Joystick
  pinMode(SW_PIN, INPUT_PULLUP); // Bật điện trở kéo lên bên trong cho nút nhấn
  analogReadResolution(12);      // Độ phân giải ADC 12-bit (0-4095)

  // Cấu hình Servo
  ESP32PWM::allocateTimer(0);
  myServo.setPeriodHertz(50); 
  myServo.attach(SERVO_PIN, 500, 2400);
  myServo.write(0); // Mặc định ở 0 độ

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

  if (motorRunning) {
    unsigned long currentMillis = millis();
    
    // Kiểm tra xem đã trôi qua đủ thời gian delayInput chưa
    if (currentMillis - previousMillis >= delayInput) {
      previousMillis = currentMillis; // Cập nhật lại mốc thời gian
      
      if (isAtMaxAngle) {
        myServo.write(0);          
        isAtMaxAngle = false;
      } else {
        myServo.write(degreeInput); 
        isAtMaxAngle = true;
      }
    }
  }
}