#include <LittleFS.h> // Dùng LittleFS (khuyến nghị)

void setup() {
  Serial.begin(9600);

  // Khởi tạo LittleFS
  if (!LittleFS.begin()) {
    Serial.println("LittleFS mount failed!");
    return;
  }
  Serial.println("LittleFS mounted successfully!");

  // Ghi dữ liệu vào file
  File file = LittleFS.open("/data.txt", "w");
  if (!file) {
    Serial.println("Failed to open file for writing!");
    return;
  }
  file.println("Hello, ESP8266!"); // Ghi chuỗi vào file
  file.close();
  Serial.println("Data written to file!");

  // Đọc dữ liệu từ file
  file = LittleFS.open("/data.txt", "r");
  if (!file) {
    Serial.println("Failed to open file for reading!");
    return;
  }
  Serial.println("Reading data from file:");
  while (file.available()) {
    Serial.write(file.read());
  }
  file.close();
}

void loop() {
  // Không cần làm gì trong loop
}
