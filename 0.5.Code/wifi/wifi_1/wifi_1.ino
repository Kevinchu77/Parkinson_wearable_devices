#include <SoftwareSerial.h>

#define RX 8
#define TX 10

SoftwareSerial esp8266(RX, TX);

String AP = "LAU_3";          // Tên Wi-Fi
String PASS = "nhatro29almn#L3"; // Mật khẩu Wi-Fi
String HOST = "192.168.1.136";  // Địa chỉ IP của máy chủ
String PORT = "80";            // Cổng HTTP
String API_ENDPOINT = "/save_data.php";  // Đường dẫn đến file PHP

void setup() {
  Serial.begin(9600);
  esp8266.begin(115200);

  // Kết nối Wi-Fi
  connectToWiFi();
}

void loop() {
  int value = random(0, 100); // Dữ liệu ngẫu nhiên

  // Gửi yêu cầu HTTP GET tới trang PHP
  sendHTTPGet(value);

  delay(10000); // Gửi mỗi 10 giây
}

// Hàm kết nối Wi-Fi
void connectToWiFi() {
  sendCommand("AT", 5, "OK");
  sendCommand("AT+CWMODE=1", 5, "OK");
  sendCommand("AT+CWJAP=\"" + AP + "\",\"" + PASS + "\"", 20, "OK");

  Serial.println("Connected to Wi-Fi!");
}

// Gửi dữ liệu đến server
void sendHTTPGet(int value) {
  String url = "GET " + API_ENDPOINT + "?value=" + String(value) + " HTTP/1.1\r\n" +
               "Host: " + HOST + "\r\n" +
               "Connection: close\r\n\r\n";

  sendCommand("AT+CIPMUX=1", 5, "OK");
  sendCommand("AT+CIPSTART=0,\"TCP\",\"" + HOST + "\"," + PORT, 15, "OK");
  sendCommand("AT+CIPSEND=0," + String(url.length() + 2), 4, ">");
  esp8266.print(url);  // Gửi yêu cầu HTTP GET

  // Kiểm tra phản hồi từ ESP8266
  if (esp8266.find("SEND OK")) {
    Serial.println("Data sent successfully: " + String(value));
  } else {
    Serial.println("Failed to send data.");
  }

  sendCommand("AT+CIPCLOSE=0", 5, "OK"); // Đóng kết nối
}

// Hàm gửi lệnh AT
void sendCommand(String command, int maxTime, char readReplay[]) {
  esp8266.println(command);
  while (!esp8266.find(readReplay)) {
    delay(100);
  }
}
