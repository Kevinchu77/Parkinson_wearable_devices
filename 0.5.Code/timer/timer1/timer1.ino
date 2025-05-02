#include <TimerOne.h> // Thư viện để điều khiển Timer1

volatile bool flag17s = false; // Cờ để báo khi ngắt 17s xảy ra
volatile int counter = 0;      // Bộ đếm số lần timer ngắt (tính từng giây)

void setup() {
  Serial.begin(9600);

  // Cấu hình Timer1 với tần số 1Hz (1 giây)
  Timer1.initialize(1000000); // 1 giây (1.000.000 microseconds)
  Timer1.attachInterrupt(timerCallback); // Gắn hàm callback cho Timer1

  Serial.println("Setup completed. Waiting for interrupt...");
}

void loop() {
  if (flag17s) {
    flag17s = false; // Reset cờ

    // Thực thi tác vụ khi đạt 17 giây
    Serial.println("Interrupt triggered: 17 seconds passed!");
  }
}

// Hàm ngắt Timer1, thực thi mỗi giây
void timerCallback() {
  counter++; // Tăng bộ đếm mỗi giây

  if (counter >= 17) { // Kiểm tra nếu đã đạt 17 giây
    counter = 0;       // Reset bộ đếm
    flag17s = true;    // Đặt cờ để thực thi trong loop()
  }
}
