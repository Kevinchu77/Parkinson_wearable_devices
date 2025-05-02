#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>

const char* ssid = "LAU_3";           // Tên Wi-Fi
const char* password = "nhatro29almn#L3"; // Mật khẩu Wi-Fi

ESP8266WebServer server(80); // HTTP server chạy trên cổng 80

void setup() {
  Serial.begin(9600);d

  // Kết nối Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to Wi-Fi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi!");

  // Hiển thị địa chỉ IP của ESP8266 trên Serial Monitor
  Serial.print("ESP8266 IP Address: ");
  Serial.println(WiFi.localIP());

  // Khởi tạo LittleFS
  if (!LittleFS.begin()) {
    Serial.println("LittleFS mount failed!");
    return;
  }
  Serial.println("LittleFS mounted successfully!");

  // Endpoint 1: Xem nội dung file trực tiếp
  server.on("/view", []() {
    const char* filename = "/data.txt"; // Đường dẫn file trong LittleFS

    File file = LittleFS.open(filename, "r");
    if (!file) {
      server.send(404, "text/plain", "File not found!");
      return;
    }

    String content;
    while (file.available()) {
      content += (char)file.read();
    }
    file.close();

    // Gửi nội dung file về trình duyệt
    server.send(200, "text/plain", content);
  });

  // Endpoint 2: Tải file về máy tính
  server.on("/download", []() {
    const char* filename = "/data.txt"; // Đường dẫn file trong LittleFS

    File file = LittleFS.open(filename, "r");
    if (!file) {
      server.send(404, "text/plain", "File not found!");
      return;
    }

    // Thiết lập header HTTP để trình duyệt hiểu đây là file tải xuống
    server.sendHeader("Content-Disposition", "attachment; filename=data.txt");
    server.streamFile(file, "text/plain");
    file.close();
  });

  // Bắt đầu server
  server.begin();
  Serial.println("HTTP server started!");
}

void loop() {
  server.handleClient(); // Xử lý các request từ trình duyệt
}
