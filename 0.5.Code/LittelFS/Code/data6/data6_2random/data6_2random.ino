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
  server.on("/view", handleViewFile);       // Xem nội dung file
  server.on("/download", handleDownload);  // Tải file về
  server.begin();
  Serial.println("HTTP server started!");

  // Hướng dẫn trên Serial Monitor
  Serial.println("Send 'start' to begin writing data.");
  Serial.println("Send 'stop' to stop writing data.");
}

void loop() {
  server.handleClient(); // Xử lý các yêu cầu từ trình duyệt

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

  if (isWriting) {
    // Sinh hai giá trị ngẫu nhiên dạng số thực với 2 chữ số sau dấu phẩy
    float randomValue1 = random(1, 100) + random(0, 100) / 100.0;
    float randomValue2 = random(1, 100) + random(0, 100) / 100.0;

    // Ghép hai giá trị lại và cách nhau bằng dấu cách
    String data = String(randomValue1, 2) + " " + String(randomValue2, 2);
    writeToFile(data); // Ghi giá trị vào file
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

  // Ghi dữ liệu vào file và thêm ký tự xuống dòng sau mỗi lần ghi
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

  // Gửi nội dung file về trình duyệt (nội dung sẽ có các giá trị ngẫu nhiên cạnh nhau và mỗi dòng cách nhau)
  server.send(200, "text/plain", content);
}


// Hàm xử lý yêu cầu "/download" để tải file về
void handleDownload() {
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

  // Gửi header để trình duyệt hiểu đây là file tải về
  server.sendHeader("Content-Disposition", "attachment; filename=data.txt");
  server.streamFile(file, "text/plain");
  file.close();
}
