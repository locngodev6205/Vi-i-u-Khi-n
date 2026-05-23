#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h> 

// 1. CẤU HÌNH
const char* ssid     = "Thasi";
const char* password = "123456789";

// Cấu hình DHT11
#define DHT_PIN 21    
#define DHT_TYPE DHT11
DHT dht(DHT_PIN, DHT_TYPE);

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
  <h2>ESP32 - CẢM BIẾN DHT11 & ĐỘNG CƠ DC</h2>
  <hr>
  <p><b>Nhiệt độ:</b> <span id='tempValue'>Đang cập nhật...</span> &deg;C</p>
  <p><b>Độ ẩm:</b> <span id='humValue'>Đang cập nhật...</span> %</p>
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
    // DHT11 cần thời gian phản hồi chậm, nên cập nhật mỗi 2000ms (2 giây)
    setInterval(() => {
      fetch('/status')
        .then(res => res.json())
        .then(data => {
          document.getElementById('tempValue').innerText = data.temp;
          document.getElementById('humValue').innerText = data.hum;
          document.getElementById('motorState').innerText = data.motor;
        });
    }, 2000);
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
  // Đọc nhiệt độ và độ ẩm
  float t = dht.readTemperature();
  float h = dht.readHumidity();

  // Kiểm tra nếu bị lỗi (do đọc quá nhanh hoặc lỏng dây)
  String tempStr = isnan(t) ? "Lỗi" : String(t, 1);
  String humStr  = isnan(h) ? "Lỗi" : String(h, 1);

  String state = motorRunning ? (motorDirection == 1 ? "Đang quay thuận (Tốc độ: " + String(speedInput) + ")" : "Đang quay ngược (Tốc độ: " + String(speedInput) + ")") : "Đã dừng";
  
  // Cập nhật JSON để gửi biến "temp" và "hum"
  String json = "{\"temp\":\"" + tempStr + "\", \"hum\":\"" + humStr + "\", \"motor\":\"" + state + "\"}";
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

  // Khởi tạo cảm biến DHT11
  dht.begin();

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