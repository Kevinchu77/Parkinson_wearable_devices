#include <SoftwareSerial.h>
#include <EEPROM.h>

// Cấu hình ESP8266
#define RX 8
#define TX 10
String AP = "LAU_3";               // Tên Wi-Fi
String PASS = "nhatro29almn#L3";   // Mật khẩu Wi-Fi
String API = "ZY4AS3D7QC7IMFEC";   // API Key của ThingSpeak
String HOST = "api.thingspeak.com";
String PORT = "80";
String field = "field1";
boolean commandSent = false;

SoftwareSerial esp8266(RX, TX); // Kết nối với ESP8266 qua RX và TX

// Biến EEPROM
int eepromAddress = 0;             // Địa chỉ hiện tại trong EEPROM
const int EEPROM_SIZE = EEPROM.length();

void setup() {
  Serial.begin(9600);
  esp8266.begin(115200);

  // Xóa toàn bộ EEPROM
  clearEEPROM();
  Serial.println("EEPROM cleared!");

  // Kết nối ESP8266 với Wi-Fi
  sendCommand("AT", 5, "OK");
  sendCommand("AT+CWMODE=1", 5, "OK");
  sendCommand("AT+CWJAP=\"" + AP + "\",\"" + PASS + "\"", 20, "OK");

  Serial.println("Setup completed!");
}

void loop() {
  // 1. Sinh giá trị ngẫu nhiên và lưu vào EEPROM
  randomSeed(analogRead(A0)); // Seed ngẫu nhiên từ tín hiệu analog
  int randomValue = random(0, 1024); // Sinh giá trị ngẫu nhiên từ 0-1023
  EEPROM.put(eepromAddress, randomValue);
  Serial.print("Value written to EEPROM: ");
  Serial.println(randomValue);

  // 2. Đọc giá trị từ EEPROM
  int sensorValue;
  EEPROM.get(eepromAddress, sensorValue);
  Serial.print("Value read from EEPROM: ");
  Serial.println(sensorValue);

  // 3. Gửi dữ liệu lên ThingSpeak
  if (commandSent) {
    Serial.print("Successfully sent value: ");
    Serial.println(sensorValue);

    // Chuyển đến địa chỉ EEPROM tiếp theo
    eepromAddress += sizeof(int);
    if (eepromAddress >= EEPROM_SIZE) {
      eepromAddress = 0; // Quay lại đầu EEPROM nếu hết dung lượng
    }
  } else {
    Serial.println("Failed to send data, retrying...");
  }
}

// Hàm xóa toàn bộ dữ liệu trong EEPROM
void clearEEPROM() {
  for (int i = 0; i < EEPROM_SIZE; i++) {
    EEPROM.write(i, 0xFF); // Ghi giá trị mặc định (0xFF) vào tất cả các byte
  }
}

// Hàm gửi dữ liệu lên ThingSpeak
bool sendToThingSpeak(int value) {
  String getData = "GET /update?api_key=" + API + "&" + field + "=" + String(value);
  sendCommand("AT+CIPMUX=1", 5, "OK");
  sendCommand("AT+CIPSTART=0,\"TCP\",\"" + HOST + "\"," + PORT, 15, "OK");
  sendCommand("AT+CIPSEND=0," + String(getData.length() + 4), 4, ">");

  esp8266.println(getData);  // Gửi dữ liệu lên ThingSpeak

  // Kiểm tra phản hồi xem dữ liệu đã được gửi hay chưa
  if (checkResponse()) {
    sendCommand("AT+CIPCLOSE=0", 5, "OK");
    return true;  // Gửi thành công
  } else {
    return false; // Gửi thất bại
  }
}

// Hàm kiểm tra phản hồi từ ESP8266 để xác nhận dữ liệu đã được gửi
bool checkResponse() {
  if (esp8266.find("OK")) {  // Kiểm tra nếu phản hồi chứa "+OK"
    return true;
  } else {
    return false;
  }
}

// Hàm gửi lệnh AT đến ESP8266
void sendCommand(String command, int maxTime, char readReplay[]) {
  Serial.print("AT command => ");
  Serial.println(command);


  while (maxTime > 0 && !commandSent) {
    esp8266.println(command);
    if (esp8266.find(readReplay)) {
      commandSent = true;
    }
    maxTime--;
  }

  if (commandSent) {
    Serial.println("OK");
  } else {
    Serial.println("Fail");
  }
}
