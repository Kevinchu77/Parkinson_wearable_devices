#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>

// Wi-Fi thông tin
const char* ssid = "LAU_3";
const char* password = "nhatro29almn#L3";

const char* host = "script.google.com";
const int httpsPort = 443;
String GAS_ID = "AKfycbynM-wp0op6_5esogYjCnf2crSgKZQQjj3YBka_HVwwAI55XwTO-YeyxuxDbWGoTctDxQ";  // ID của Web App Google Apps Script

WiFiClientSecure client;
ESP8266WebServer server(80); // HTTP server chạy trên cổng 80

String sheetName = "default";  // Mặc định là "default", sẽ được thay đổi khi tạo sheet mới
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

  client.setInsecure();

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
  Serial.println("Send 'new_[sheet_name]' to create a new sheet in Google Sheets.");
}

void loop() {
  server.handleClient(); // Xử lý các yêu cầu từ trình duyệt

  if (Serial.available()) {
    String command = Serial.readStringUntil('\n'); // Đọc lệnh từ Serial Monitor
    command.trim(); // Xóa khoảng trắng hoặc ký tự xuống dòng

    if (command.startsWith("new_")) {
      sheetName = command.substring(4);  // Lấy tên sheet từ lệnh
      Serial.println("Tạo sheet mới: " + sheetName);
      createNewSheet(sheetName);  // Tạo sheet mới
    } else if (command == "start") {
      startWriting(); // Bắt đầu ghi
    } else if (command == "stop") {
      stopWriting(); // Dừng ghi
    } else {
      Serial.println("Unknown command. Use 'start', 'stop' or 'new_[sheet_name]'.");
    }
  }

  // Nếu đang ghi, tự động sinh ra 2 giá trị random và lưu vào file
  if (isWriting) {
    float randomValue1 = random(1, 100) + random(0, 100) / 100.0; // Nhiệt độ
    float randomValue2 = random(1, 100) + random(0, 100) / 100.0; // Độ ẩm

    // Ghép hai giá trị lại và cách nhau bằng dấu cách
    String data = String(randomValue1, 2) + " " + String(randomValue2, 2);
    writeToFile(data); // Ghi giá trị vào file
    sendData(randomValue1, randomValue2); // Gửi dữ liệu lên Google Sheets
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

  // Gửi nội dung file về trình duyệt
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

// Hàm gửi dữ liệu lên Google Sheets
void sendData(float temp, float hum) {
  if (!client.connect(host, httpsPort)) {
    Serial.println("Kết nối thất bại");
    return;
  }

  String url = "/macros/s/" + GAS_ID + "/exec?action=send_data&sheet=" + sheetName +
               "&nhietdo=" + String(temp, 2) + "&doam=" + String(hum, 2);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");

  Serial.println("Dữ liệu đã gửi.");
}

// Hàm tạo sheet mới trên Google Sheets
void createNewSheet(String name) {
  if (!client.connect(host, httpsPort)) {
    Serial.println("Kết nối thất bại");
    return;
  }

  String url = "/macros/s/" + GAS_ID + "/exec?action=new_sheet&sheet=" + name;
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");

  Serial.println("Yêu cầu tạo sheet đã gửi.");
}
