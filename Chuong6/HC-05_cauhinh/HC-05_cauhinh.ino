#include <SoftwareSerial.h>

// HC-05 RX/TX qua SoftwareSerial
// Chan 10 la RX (noi voi TX cua HC-05)
// Chan 11 la TX (noi voi RX cua HC-05)
SoftwareSerial BTSerial(10, 11);

const int PIN_KEY = 9;
const long USB_BAUD = 9600;
const long HC05_AT_BAUD = 38400;

const char DEFAULT_PSWD[] = "123456";
const char DEFAULT_UART[] = "38400,0,0";

String inputLine = "";

void printHelp() {
  Serial.println();
  Serial.println("===== HC-05 AT TOOL =====");
  Serial.println("Luu y: KEY phai HIGH truoc khi cap nguon HC-05 de vao AT mode on dinh.");
  Serial.println("Lenh:");
  Serial.println("  H                  : Hien thi huong dan");
  Serial.println("  S                  : Cau hinh module hien tai thanh SLAVE");
  Serial.println("  M <addr>           : Cau hinh MASTER va BIND toi dia chi SLAVE");
  Serial.println("                       Vi du: M 7D6E,4D,4668C0");
  Serial.println("  A                  : Doc dia chi module (AT+ADDR?)");
  Serial.println("  AT+...             : Gui truc tiep lenh AT bat ky");
  Serial.println("  RAW <noi_dung>     : Gui thang chuoi toi HC-05 (co CRLF)");
  Serial.println();
}

void sendRawLine(const String &line) {
  BTSerial.print(line);
  BTSerial.print("\r\n");
}

void readResponse(unsigned long waitMs) {
  unsigned long start = millis();
  while (millis() - start < waitMs) {
    while (BTSerial.available()) {
      Serial.write(BTSerial.read());
      start = millis();
    }
  }
  Serial.println();
}

void sendAT(const String &cmd, unsigned long waitMs = 800) {
  Serial.print(">> ");
  Serial.println(cmd);
  sendRawLine(cmd);
  readResponse(waitMs);
}

void configureSlave() {
  Serial.println();
  Serial.println("--- Cau hinh SLAVE ---");
  sendAT("AT");
  sendAT("AT+ORGL", 1200);
  sendAT("AT+ROLE=0");
  sendAT("AT+CMODE=0");
  sendAT(String("AT+PSWD=") + DEFAULT_PSWD);
  sendAT(String("AT+UART=") + DEFAULT_UART);
  sendAT("AT+NAME=HC05_SLAVE");
  sendAT("AT+ADDR?");
  sendAT("AT+RESET", 1500);
  Serial.println("Hoan tat cau hinh SLAVE.");
}

void configureMaster(const String &addr) {
  Serial.println();
  Serial.println("--- Cau hinh MASTER ---");
  sendAT("AT");
  sendAT("AT+ORGL", 1200);
  sendAT("AT+ROLE=1");
  sendAT("AT+CMODE=0");
  sendAT(String("AT+PSWD=") + DEFAULT_PSWD);
  sendAT(String("AT+UART=") + DEFAULT_UART);
  sendAT("AT+NAME=HC05_MASTER");
  sendAT(String("AT+BIND=") + addr);
  sendAT("AT+BIND?");
  sendAT("AT+RESET", 1500);
  Serial.println("Hoan tat cau hinh MASTER.");
}

void processLine(String line) {
  line.trim();
  if (line.length() == 0) {
    return;
  }

  if (line.equalsIgnoreCase("H")) {
    printHelp();
    return;
  }

  if (line.equalsIgnoreCase("S")) {
    configureSlave();
    return;
  }

  if (line.equalsIgnoreCase("A")) {
    sendAT("AT+ADDR?");
    return;
  }

  if (line.startsWith("M ") || line.startsWith("m ")) {
    String addr = line.substring(2);
    addr.trim();
    if (addr.length() == 0) {
      Serial.println("Thieu dia chi. Vi du: M 98D3,31,AB1234");
      return;
    }
    configureMaster(addr);
    return;
  }

  if (line.startsWith("RAW ") || line.startsWith("raw ")) {
    String raw = line.substring(4);
    raw.trim();
    Serial.print(">> RAW ");
    Serial.println(raw);
    sendRawLine(raw);
    readResponse(800);
    return;
  }

  if (line.startsWith("AT") || line.startsWith("at")) {
    sendAT(line);
    return;
  }

  Serial.println("Lenh khong hop le. Nhap H de xem huong dan.");
}

void setup() {
  pinMode(PIN_KEY, OUTPUT);
  digitalWrite(PIN_KEY, HIGH);

  Serial.begin(USB_BAUD);
  BTSerial.begin(HC05_AT_BAUD);

  Serial.println();
  Serial.println("HC-05 configurator is ready.");
  printHelp();
}

void loop() {
  while (Serial.available()) {
    char c = (char)Serial.read();
    if (c == '\r') {
      continue;
    }
    if (c == '\n') {
      processLine(inputLine);
      inputLine = "";
    } else {
      inputLine += c;
    }
  }

  while (BTSerial.available()) {
    Serial.write(BTSerial.read());
  }
}
