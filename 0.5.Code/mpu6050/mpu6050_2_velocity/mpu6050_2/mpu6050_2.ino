#include <Wire.h>

float RateRoll, RatePitch, RateYaw;

float RateCalibrationRoll, RateCalibrationPitch, RateCalibrationYaw;

float RateCalibrationNumber;

//%%Ham lay tin hieu gia toc goc 
void gyro_signals(void){
  //%Cau hinh bo loc
  Wire.beginTransmission(0x68);//khoi tao giao tiep I2C, gia tri address default cua mpu6050 0x68
  Wire.write(0x1A); //Wire.write: truy cap den thanh ghi de config. Thanh ghi bo loc thong thap Low Pass Filter 0x1A
  Wire.write(0x04); // Loc tan so nao tren 21HZ voi Acc va 20 Hz voi Gyro. tra datasheet 
  Wire.endTransmission();

  //%Cau hinh full scale Gyro
  Wire.beginTransmission(0x68);
  Wire.write(0x1B); //Address thanh ghi Config Gyro
  Wire.write(0x8); // Config FS_SEL = 1, full_scale = +-500degree/s,LSB = 65.5 LSB/degree/s
  Wire.endTransmission();

  //%Doc gia tri ve tu thanh ghi
  Wire.beginTransmission(0x68);
  Wire.write(0x43); //Chi ra thanh ghi dau tien se su dung
  Wire.endTransmission();

  Wire.requestFrom(0x68, 6); // yeu cua 6 byte tu slave address MPU
  int16_t GyroX = Wire.read()<<8|Wire.read(); // Wire.read: doc 1 byte tu bo dem I2C sau khi Wire.requestFrom, dich sang trai 8 bit ket hop toan tu OR: | de cong tao thanh 16 bit. VD: 0x1A << 8 = 0x1A00 | 0x12 = 0x1A12
  int16_t GyroY = Wire.read()<<8|Wire.read();
  int16_t GyroZ = Wire.read()<<8|Wire.read();

  RateRoll = (float)GyroX/65.5; //convert ve degree/s
  RatePitch = (float)GyroY/65.5;
  RateYaw = (float)GyroZ/65.5;
}
void setup() {
  Serial.begin(57600);
  pinMode(13, OUTPUT);
  digitalWrite(13,HIGH);

  Wire.setClock(400000);//Communication for MPU605 is 400kHz follow datasheet
  Wire.begin();
  delay(250);

  //%Tat che do ngu
  Wire.beginTransmission(0x68);
  Wire.write(0x6B); // Thanh ghi management power
  Wire.write(0x00);
  Wire.endTransmission();

  //%Calibration vi tri home
  for(RateCalibrationNumber = 0; RateCalibrationNumber < 2000; RateCalibrationNumber++){ //doc value tai home 2000 lan, time 2s sethome
    gyro_signals();
    RateCalibrationRoll += RateRoll;
    RateCalibrationPitch += RatePitch;
    RateCalibrationYaw += RateYaw;
    delay(1); 
  }
  RateCalibrationRoll/=2000;
  RateCalibrationPitch/=2000;
  RateCalibrationYaw/=2000;
  
}

void loop() {
  gyro_signals();
  RateRoll -= RateCalibrationRoll;
  RatePitch -= RateCalibrationPitch;
  RateYaw -= RateCalibrationYaw;
  Serial.print("Roll = ");
  Serial.print(RateRoll);
  Serial.print(" Pitch  = ");
  Serial.print(RatePitch);
  Serial.print(" Yaw = ");
  Serial.println(RateYaw);
  delay(50);
}
