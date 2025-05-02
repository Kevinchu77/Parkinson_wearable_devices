#include <Wire.h>
#include <MPU6050.h>
#include <math.h>

MPU6050 mpu;

// Các biến cần thiết
unsigned long prevTime = 0;
float yaw = 0;
float accelAngle = 0;  // Góc từ accelerometer
float gyroAngle = 0;   // Góc từ gyroscope
float alpha = 0.98;     // Hệ số của bộ lọc bổ sung

void setup() {
  Serial.begin(115200);
  Wire.begin();
  mpu.initialize();
  
  if (!mpu.testConnection()) {
    Serial.println("MPU6050 connection failed!");
    while (1);
  }
}

void loop() {
  static unsigned long prevTime = 0;
  unsigned long currentTime = millis();
  float dt = (currentTime - prevTime) / 1000.0; // Delta time in seconds
  prevTime = currentTime;

  // Đọc dữ liệu accelerometer và gyroscope
  int16_t ax, ay, az;
  int16_t gx, gy, gz;
  mpu.getAcceleration(&ax, &ay, &az);
  mpu.getRotation(&gx, &gy, &gz);

  // Chuyển đổi giá trị thô từ accelerometer sang góc
  accelAngle = atan2(ay, az) * 180.0 / PI; // Góc nghiêng (degree)

  // Chuyển đổi giá trị thô từ gyroscope sang góc (đơn vị °/s)
  float gyroZ = gz / 131.0; // Chuyển đổi gyroscope Z-axis (±250°/s)
  gyroAngle += gyroZ * dt; // Tích phân vận tốc góc để tính góc yaw

  // Bộ lọc bổ sung (Complementary Filter)
  yaw = alpha * (gyroAngle) + (1.0 - alpha) * (accelAngle);

  // In ra giá trị góc yaw
  Serial.print("Yaw: ");
  Serial.println(yaw);

  delay(10); // Điều chỉnh độ trễ theo nhu cầu
}
