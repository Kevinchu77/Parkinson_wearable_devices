unsigned long loopCount = 0;  // Biến đếm số lần vòng lặp

void setup() {
  Serial.begin(9600);  // Khởi tạo Serial Monitor
}

void loop() {
  loopCount++;  // Tăng biến đếm mỗi lần vòng lặp chạy

  // In số vòng lặp ra Serial Monitor mỗi 1 giây
  if (millis() % 1000 == 0) {
    Serial.print("Số vòng lặp trong 1 giây: ");
    Serial.println(loopCount);
  }
  
  // Để tránh in quá nhiều lần trong mỗi giây, ta chỉ cần in 1 lần
  delay(10);  // Giảm tần suất in kết quả ra màn hình, tránh làm chậm chương trình quá mức
}
