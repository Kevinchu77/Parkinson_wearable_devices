#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

const char* host = "script.google.com";
const int httpsPort = 443;
String GAS_ID = "AKfycbynM-wp0op6_5esogYjCnf2crSgKZQQjj3YBka_HVwwAI55XwTO-YeyxuxDbWGoTctDxQ";  // ID của Web App Google Apps Script
const char* ssid = "LAU_3";
const char* password = "nhatro29almn#L3";

WiFiClientSecure client;
String sheetName = "default";  // Sheet hiện tại

void setup() {
  Serial.begin(115200);
  delay(500);

  WiFi.begin(ssid, password);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("\nWiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  client.setInsecure();
}

void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');  
    input.trim();  // Loại bỏ khoảng trắng và xuống dòng

    if (input.startsWith("new_")) {
      sheetName = input.substring(4);  // Lấy tên sheet từ lệnh
      Serial.println("Tạo sheet mới: " + sheetName);
      createNewSheet(sheetName);
    }
  }

  int nd = random(20, 40);
  int dm = random(70, 95);
  Serial.println("Nhiệt độ: " + String(nd) + " | Độ ẩm: " + String(dm));
  
  sendData(nd, dm);
  delay(5000);  // Gửi dữ liệu mỗi 5 giây
}

void createNewSheet(String name) {
  Serial.print("Gửi yêu cầu tạo sheet mới: ");
  Serial.println(name);

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

void sendData(int temp, int hum) {
  if (!client.connect(host, httpsPort)) {
    Serial.println("Kết nối thất bại");
    return;
  }

  String url = "/macros/s/" + GAS_ID + "/exec?action=send_data&sheet=" + sheetName +
               "&nhietdo=" + String(temp) + "&doam=" + String(hum);
  
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");

  Serial.println("Dữ liệu đã gửi.");
}
