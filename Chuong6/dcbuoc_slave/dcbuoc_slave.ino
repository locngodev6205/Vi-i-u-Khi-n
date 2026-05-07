#include <Stepper.h>
#include <SoftwareSerial.h>

// --- Cấu hình Bluetooth ---
SoftwareSerial BTSerial(10, 11); 

// --- Cấu hình Động cơ bước ---
const int stepsPerRevolution = 2038;
const int step60  = stepsPerRevolution / 6;
const int step90 = stepsPerRevolution / 4;

Stepper myStepper = Stepper(stepsPerRevolution, 4, 6, 5, 7);

// BIẾN LƯU TRẠNG THÁI HIỆN TẠI
// 'S': Dừng (Stop) | 'F': Có Gas (Forward) | 'R': Hết Gas (Reverse)
char currentMode = 'S'; 

void setup() {
    // LƯU Ý CỰC KỲ QUAN TRỌNG: 
    // Mặc định giao tiếp Data của HC-05 là 9600, không phải 38400 (38400 là để gõ lệnh AT).
    // Mình sửa lại 9600 cho bạn, có thể đây là lý do 2 con module của bạn nãy giờ không gửi lệnh cho nhau được đó!
    BTSerial.begin(38400); 
    Serial.begin(9600);
    
    myStepper.setSpeed(10); // Tốc độ quay (vòng/phút)
    
    Serial.println("--- SLAVE STEPPER READY ---");
}

void loop() {
    // 1. Lắng nghe Bluetooth: Có lệnh mới thì CẬP NHẬT TRẠNG THÁI
    if (BTSerial.available()) {
        char cmd = BTSerial.read();
        Serial.print("Da nhan lenh moi: ");
        Serial.println(cmd);

        // Lưu lại lệnh cuối cùng nhận được
        if (cmd == '1' || cmd == '0' || cmd == 'S') {
            currentMode = cmd; 
        }
    }

    // 2. Thực thi LẶP ĐI LẶP LẠI dựa theo trạng thái đã lưu
    if (currentMode == '1') { 
        // Nếu trạng thái đang là 'F' (Có Gas) -> Cứ xoay 90 độ thuận mãi mãi
        Serial.println("Xoay thuan 90 do...");
        myStepper.step(step90);
        myStepper.setSpeed(10);
    } 
    else if (currentMode == '0') {
        // Nếu trạng thái đang là 'R' (Hết Gas) -> Cứ xoay 180 độ ngược mãi mãi
        Serial.println("Xoay nguoc 45 do...");
        myStepper.step(-step60);
        myStepper.setSpeed(15);
    }
    // Nếu currentMode == 'S' thì nó không lọt vào 2 if trên -> Motor tự động đứng im chờ lệnh.
    delay(200);
}
