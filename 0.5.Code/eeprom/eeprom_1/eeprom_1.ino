#include <EEPROM.h>

// Kích thước EEPROM (phụ thuộc vào loại Arduino, với Nano là 1024 byte)
const int EEPROM_SIZE = EEPROM.length();
int eepromAddress = 0; // Địa chỉ hiện tại trong EEPROM
unsigned long iterationCount = 0; // Đếm số lần lặp

void setup() {
  Serial.begin(9600);
  Serial.println("Starting EEPROM write/read test...");
}

void loop() {
  // 1. Tạo giá trị ngẫu nhiên
  randomSeed(analogRead(A0)); // Sử dụng tín hiệu từ A0 làm seed ngẫu nhiên
  int randomValue = random(0, 1024); // Tạo giá trị ngẫu nhiên từ 0-1023

  // 2. Ghi giá trị vào EEPROM
  EEPROM.put(eepromAddress, randomValue);

  // 3. Đọc lại giá trị từ EEPROM
  int readValue;
  EEPROM.get(eepromAddress, readValue);

  // 4. Kiểm tra kết quả
  if (randomValue != readValue) {
    Serial.print("Error at iteration: ");
    Serial.println(iterationCount);
    Serial.print("Written: ");
    Serial.println(randomValue);
    Serial.print("Read: ");
    Serial.println(readValue);
    while (1); // Dừng chương trình nếu xảy ra lỗi
  } else {
    Serial.print("Iteration: ");
    Serial.print(iterationCount);
    Serial.print(", Written: ");
    Serial.print(randomValue);
    Serial.print(", Read: ");
    Serial.println(readValue);
  }

  // 5. Cập nhật địa chỉ EEPROM
  eepromAddress += sizeof(int); // Tiến đến ô nhớ tiếp theo
  if (eepromAddress >= EEPROM_SIZE) {
    eepromAddress = 0; // Quay lại từ đầu nếu vượt quá dung lượng EEPROM
  }

  // 6. Tăng số lần lặp
  iterationCount++;

  // 7. Thêm trễ nhỏ để tránh quá tải
  delay(100); // 100ms (có thể điều chỉnh theo nhu cầu)
}
