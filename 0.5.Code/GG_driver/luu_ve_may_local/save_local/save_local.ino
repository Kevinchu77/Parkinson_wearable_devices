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
bool isSending = false; // Biến trạng thái kiểm tra việc gửi dữ liệu
unsigned long previousMillis = 0;  // Lưu thời gian của lần cập nhật trước
const long interval = 50;  // Cập nhật mỗi 50ms (20Hz)

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
  server.on("/save_data1", saveData1);    // Lưu file với tên data1.txt
  server.begin();
  Serial.println("HTTP server started!");

  // Hướng dẫn trên Serial Monitor
  Serial.println("Send 'start' to begin generating data at 20Hz.");
  Serial.println("Send 'stop' to stop generating data.");
  Serial.println("Send 'start_send' to begin sending data.");
  Serial.println("Send 'new_[sheet_name]' to create a new sheet in Google Sheets.");
  Serial.println("Send 'delete' to delete data from local storage.");
  Serial.println("Send 'save_data1' to save file data1.txt and download it.");
}

void loop() {
  server.handleClient(); // Xử lý các yêu cầu từ trình duyệt

  unsigned long currentMillis = millis();

  // Kiểm tra xem đã đến thời gian sinh dữ liệu mới chưa (20Hz)
  if (currentMillis - previousMillis >= interval && isWriting) {
    previousMillis = currentMillis;

    // Sinh và ghi dữ liệu khi đang ở chế độ ghi
    if (isWriting) {
      float randomValue1 = random(1, 100) + random(0, 100) / 100.0; // Nhiệt độ
      float randomValue2 = random(1, 100) + random(0, 100) / 100.0; // Độ ẩm

      // Ghép hai giá trị lại và cách nhau bằng dấu cách
      String data = String(randomValue1, 2) + " " + String(randomValue2, 2);
      writeToFile(data); // Ghi giá trị vào file

      // Nếu đang gửi dữ liệu, gửi đến Google Sheets
      if (isSending) {
        sendData(randomValue1, randomValue2); // Gửi dữ liệu lên Google Sheets
      }
    }
  }

  if (Serial.available()) {
    String command = Serial.readStringUntil('\n'); // Đọc lệnh từ Serial Monitor
    command.trim(); // Xóa khoảng trắng hoặc ký tự xuống dòng

    if (command.startsWith("new_")) {
      sheetName = command.substring(4);  // Lấy tên sheet từ lệnh
      Serial.println("Tạo sheet mới: " + sheetName);
      createNewSheet(sheetName);  // Tạo sheet mới
    } else if (command == "start_send") {
      isSending = true;  // Bắt đầu gửi dữ liệu lên Google Sheets
      Serial.println("Started sending data to Google Sheets.");
      sendDataFromFile();  // Gửi dữ liệu từ file
    } else if (command == "stop") {
      isWriting = false; // Dừng ghi dữ liệu
      Serial.println("Stopped generating data.");
    } else if (command == "start") {
      isWriting = true; // Bắt đầu ghi dữ liệu với tần suất 20Hz
      Serial.println("Started generating random data at 20Hz.");
    } else if (command == "delete") {
      deleteLocalData(); // Xóa dữ liệu trong local storage
    } else if (command == "save_data1") {
      saveData1(); // Lưu dữ liệu vào file data1.txt và tải về
    } else {
      Serial.println("Unknown command. Use 'start_send', 'stop', 'new_[sheet_name]', 'start', or 'delete'.");
    }
  }
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

// Hàm để lưu file data1.txt và tải về
void saveData1() {
  const char* filename = "/data.txt";

  if (!LittleFS.exists(filename)) {
    Serial.println("No data found in local storage.");
    return;
  }

  File file = LittleFS.open(filename, "r");
  if (!file) {
    Serial.println("Failed to open file!");
    return;
  }

  // Lưu file vào thư mục cụ thể (nhưng chỉ có thể tải xuống qua HTTP)
  String content;
  while (file.available()) {
    content += (char)file.read();
  }
  file.close();

  // Gửi dữ liệu về trình duyệt (lưu vào thư mục trên máy tính)
  server.sendHeader("Content-Disposition", "attachment; filename=data1.txt");
  server.send(200, "text/plain", content);
  Serial.println("File 'data1.txt' is ready for download.");
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

// Hàm xóa dữ liệu trong local storage
void deleteLocalData() {
  const char* filename = "/data.txt";

  if (LittleFS.exists(filename)) {
    LittleFS.remove(filename);  // Xóa file chứa dữ liệu
    Serial.println("Data deleted from local storage.");
  } else {
    Serial.println("No data found in local storage.");
  }
}
