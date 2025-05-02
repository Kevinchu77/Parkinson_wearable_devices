#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>

// Thông tin WiFi
#define WIFI_SSID "LAU_3"
#define WIFI_PASSWORD "nhatro29almn#L3"

// URL của Google Apps Script
const String GOOGLE_SCRIPT_URL = "https://script.google.com/macros/s/AKfycbzq43sZMoAWAYBrh8CbPSkOseetNNIjx4Fl3qixEYm9vPec678FIpE38yYAem_pkpv8/exec";

bool sendingData = false; // Biến điều khiển việc gửi dữ liệu

void setup() {
    Serial.begin(9600);

    // Kết nối WiFi
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    Serial.println("\nWiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    // Hướng dẫn sử dụng trên Serial Monitor
    Serial.println("Send 'start' to begin sending data or 'stop' to stop.");
}

void loop() {
    // Đọc lệnh từ Serial Monitor
    if (Serial.available() > 0) {
        String command = Serial.readStringUntil('\n');
        command.trim(); // Loại bỏ khoảng trắng dư thừa

        if (command == "start") {
            sendingData = true; // Bắt đầu gửi dữ liệu
            Serial.println("Started sending data to Google Sheets.");
        } else if (command == "stop") {
            sendingData = false; // Dừng gửi dữ liệu
            Serial.println("Stopped sending data to Google Sheets.");
        }
    }

    // Nếu đang gửi dữ liệu
    if (sendingData) {
        // Sinh dữ liệu ngẫu nhiên để gửi (phần thực và ảo của số phức)
        float realPart = random(-10, 10) + random(0, 100) / 100.0;
        float imaginaryPart = random(-10, 10) + random(0, 100) / 100.0;

        // Làm tròn đến 3 chữ số thập phân
        realPart = round(realPart * 1000.0) / 1000.0;
        imaginaryPart = round(imaginaryPart * 1000.0) / 1000.0;

        // Tạo chuỗi dữ liệu
        String data = "Real: " + String(realPart, 3) + ", Imaginary: " + String(imaginaryPart, 3);

        // Gửi dữ liệu lên Google Sheets
        if (sendDataToGoogleSheets(data)) {
            Serial.println("Data sent: " + data);
        } else {
            Serial.println("Failed to send data to Google Sheets.");
        }

        delay(1000); // Gửi mỗi giây một lần
    }
}

bool sendDataToGoogleSheets(String data) {
    WiFiClientSecure client; // Sử dụng WiFiClientSecure để hỗ trợ HTTPS
    HTTPClient http;   // Đối tượng HTTPClient

    // Kết nối đến Google Apps Script
    if (http.begin(client, GOOGLE_SCRIPT_URL + "?data=" + data)) { // Thêm dữ liệu vào URL
        int httpCode = http.GET(); // Gửi yêu cầu HTTP GET

        if (httpCode > 0) {
            Serial.printf("HTTP GET Success, Code: %d\n", httpCode);
            String response = http.getString();
            Serial.println("Response: " + response);
        } else {
            Serial.printf("HTTP GET Failed, Error: %s\n", http.errorToString(httpCode).c_str());
        }

        http.end(); // Kết thúc phiên làm việc
        return httpCode == HTTP_CODE_OK;
    } else {
        Serial.println("Unable to connect to Google Sheets script");
        return false;
    }
}
