#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>

// Wi-Fi thông tin
const char* ssid = "LAU_3";
const char* password = "nhatro29almn#L3";

ESP8266WebServer server(80); // HTTP server chạy trên cổng 80

bool isWriting = false; // Biến trạng thái kiểm tra việc ghi dữ liệu

void setup() {
  Serial.begin(9600);

  // Kết nối Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to Wi-Fi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi!");
  Serial.print("ESP8266 IP Address: ");
  Serial.println(WiFi.localIP()); // In địa chỉ IP của ESP8266

  // Khởi tạo LittleFS
  if (!LittleFS.begin()) {
    Serial.println("LittleFS mount failed!");
    return;
  }
  Serial.println("LittleFS mounted successfully!");

  // Cấu hình server
  server.on("/view", handleViewFile); // Xem nội dung file
  server.begin();
  Serial.println("HTTP server started!");

  // Hướng dẫn trên Serial Monitor
  Serial.println("Send 'start' to begin writing data.");
  Serial.println("Send 'stop' to stop writing data.");
}

void loop() {
  server.handleClient(); // Xử lý các yêu cầu từ trình duyệt

  // Kiểm tra lệnh từ Serial Monitor
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n'); // Đọc lệnh từ Serial Monitor
    command.trim(); // Xóa khoảng trắng hoặc ký tự xuống dòng

    if (command == "start") {
      startWriting(); // Bắt đầu ghi
    } else if (command == "stop") {
      stopWriting(); // Dừng ghi
    } else {
      Serial.println("Unknown command. Use 'start' or 'stop'.");
    }
  }

  // Nếu đang trong chế độ ghi, ghi giá trị random vào file
  if (isWriting) {
    int randomValue = random(1, 100); // Sinh giá trị ngẫu nhiên
    writeToFile(String(randomValue)); // Ghi giá trị vào file
    delay(1000); // Ghi dữ liệu mỗi giây
  }
}

// Hàm bắt đầu ghi dữ liệu
void startWriting() {
  const char* filename = "/data.txt";

  // Xóa file trước khi ghi
  if (LittleFS.exists(filename)) {
    LittleFS.remove(filename);
    Serial.println("File deleted before starting.");
  }

  isWriting = true;
  Serial.println("Started writing data to file.");
}

// Hàm dừng ghi dữ liệu
void stopWriting() {
  isWriting = false;
  Serial.println("Stopped writing data to file.");
}

// Hàm ghi dữ liệu vào file
void writeToFile(String data) {
  const char* filename = "/data.txt";

  // Mở file ở chế độ "a" (append), thêm dữ liệu vào cuối file
  File file = LittleFS.open(filename, "a");
  if (!file) {
    Serial.println("Failed to open file for appending!");
    return;
  }

  // Ghi dữ liệu kèm ký tự xuống dòng
  file.println(data);
  file.close();

  // In ra dữ liệu đã ghi
  Serial.print("Written to file: ");
  Serial.println(data);
}

// Hàm xử lý yêu cầu "/view" để xem nội dung file trên trình duyệt
void handleViewFile() {
  const char* filename = "/data.txt";

  if (!LittleFS.exists(filename)) {
    server.send(404, "text/plain", "File not found!");
    return;
  }

  File file = LittleFS.open(filename, "r");
  if (!file) {
    server.send(500, "text/plain", "Failed to open file!");
    return;
  }

  String content;
  while (file.available()) {
    content += (char)file.read();
  }
  file.close();

  // Gửi nội dung file về trình duyệt
  server.send(200, "text/plain", content);
}
