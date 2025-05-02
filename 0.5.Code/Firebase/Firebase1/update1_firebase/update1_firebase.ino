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
}

void loop() {
  // Ghi giá trị ban đầu cho "led"
  if (Firebase.setFloat(firebaseData, "/led", 0)) {
    Serial.println("Initial value set to /led: 0");
  } else {
    Serial.print("Failed to set /led. Reason: ");
    Serial.println(firebaseData.errorReason());
  }

  // Tăng giá trị "led" từ 0 đến 9
  for (int i = 0; i < 10; i++) {
    if (Firebase.setFloat(firebaseData, "/led", i)) {
      Serial.print("Value set to /led: ");
      Serial.println(i);
    } else {
      Serial.print("Failed to set /led. Reason: ");
      Serial.println(firebaseData.errorReason());
    }
    delay(25);
  }
}
