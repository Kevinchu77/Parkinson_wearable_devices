#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>

// Thông tin Firebase
#define FIREBASE_HOST "https://parkinson-patients-data-default-rtdb.asia-southeast1.firebasedatabase.app" // Địa chỉ Firebase
#define FIREBASE_AUTH "a9kvAlo2JGIw5dg146RA4TauY2LtC1d9FkGBr2jW"                      // Authentication key (nếu không dùng, để trống)

// Thông tin WiFi
#define WIFI_SSID "LAU_3"                  // Tên WiFi
#define WIFI_PASSWORD "nhatro29almn#L3"              // Mật khẩu WiFi

FirebaseData firebaseData; // Đối tượng để giao tiếp với Firebase
FirebaseConfig firebaseConfig; // Cấu hình Firebase
FirebaseAuth firebaseAuth; // Cấu hình xác thực (để trống nếu không dùng)

bool sendingData = false; // Biến điều khiển gửi dữ liệu (start/stop)

void setup() {
  Serial.begin(9600);

  // Kết nối WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.println("WiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Cấu hình Firebase
  firebaseConfig.host = FIREBASE_HOST;
  firebaseConfig.signer.tokens.legacy_token = FIREBASE_AUTH; // Sử dụng token xác thực

  // Kết nối Firebase
  Firebase.begin(&firebaseConfig, &firebaseAuth);

  if (Firebase.ready()) {
    Serial.println("Firebase connected!");
  } else {
    Serial.println("Firebase connection failed!");
    Serial.println(firebaseConfig.signer.tokens.error.message.c_str());
  }

  // Lắng nghe lệnh từ Serial Monitor
  Serial.println("Send 'start' to begin sending data and 'stop' to stop.");
}

void loop() {
  // Đọc lệnh từ Serial Monitor
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim(); // Loại bỏ ký tự trắng dư thừa

    if (command == "start") {
      sendingData = true; // Bắt đầu gửi dữ liệu
      Serial.println("Started sending data to Firebase.");
    } 
    else if (command == "stop") {
      sendingData = false; // Dừng gửi dữ liệu
      Serial.println("Stopped sending data to Firebase.");
    }
  }

  // Nếu đang gửi dữ liệu, tiến hành gửi
  if (sendingData) {
    // Giả sử bạn muốn gửi dữ liệu số phức dưới dạng float
    // Tạo một giá trị float ngẫu nhiên (giả định dữ liệu số phức là các số ngẫu nhiên)
    float realPart = random(-10, 10) + random(0, 100) / 100.0;  // Phần thực
    float imaginaryPart = random(-10, 10) + random(0, 100) / 100.0;  // Phần ảo

    // Làm tròn đến 3 chữ số thập phân
    realPart = round(realPart * 1000.0) / 1000.0;
    imaginaryPart = round(imaginaryPart * 1000.0) / 1000.0;

    // Chuyển giá trị float thành String
    String realStr = String(realPart, 3);  // Làm tròn đến 3 chữ số thập phân
    String imaginaryStr = String(imaginaryPart, 3);  // Làm tròn đến 3 chữ số thập phân

    // Kết hợp phần thực và phần ảo thành một chuỗi duy nhất
    String complexData = "Real: " + realStr + ", Imaginary: " + imaginaryStr;

    // Gửi dữ liệu lên Firebase dưới dạng String
    if (Firebase.setString(firebaseData, "/data/complexData", complexData)) {
      Serial.print("Sent data: ");
      Serial.println(complexData);
    } else {
      Serial.print("Failed to send data to Firebase. Reason: ");
      Serial.println(firebaseData.errorReason());
    }

    delay(25); // Giới hạn gửi mỗi giây 1 lần
  }
}
