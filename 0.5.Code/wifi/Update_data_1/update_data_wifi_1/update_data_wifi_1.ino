#include <Wire.h>
#include <SoftwareSerial.h>

float RateRoll, RatePitch, RateYaw;

float RateCalibrationRoll, RateCalibrationPitch, RateCalibrationYaw;

float RateCalibrationNumber;

//Configure wifi channel
#define RX 8  //TX ---> RX
#define TX 10  //RX ---> TX
String AP = "LAU_3";       // AP NAMEPORT
String PASS = "nhatro29almn#L3"; // AP PASSWORD
String API = "ZY4AS3D7QC7IMFEC";   // Write API KEY
String HOST = "api.thingspeak.com";
String PORT = "80";
String field = "field2";
int countTrueCommand;
int countTimeCommand; 
boolean found = false; 
int valSensor = 1;
SoftwareSerial esp8266(RX,TX); 

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

//Ham sendcomand
void sendCommand(String command, int maxTime, char readReplay[]) {
  Serial.print(countTrueCommand);
  Serial.print(". at command => ");
  Serial.print(command);
  Serial.print(" ");
  while(countTimeCommand < (maxTime*1))
  {
    esp8266.println(command);//at+cipsend
    if(esp8266.find(readReplay))//ok
    {
      found = true;
      break;
    }
  
    countTimeCommand++;
  }
  
  if(found == true)
  {
    Serial.println("OK");
    countTrueCommand++;
    countTimeCommand = 0;
  }
  
  if(found == false)
  {
    Serial.println("Fail");
    countTrueCommand = 0;
    countTimeCommand = 0;
  }
  
  found = false;
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

  //Setup wifi
  esp8266.begin(115200);
  sendCommand("AT",5,"OK");
  sendCommand("AT+CWMODE=1",5,"OK"); // chon mode 1 apt: thiet bi tram thu
  sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");
  
}

int getSensorData(){
  return random(10); // Replace with your own sensor code
}

void loop() {
  gyro_signals();
  RateRoll -= RateCalibrationRoll;
  RatePitch -= RateCalibrationPitch;
  RateYaw -= RateCalibrationYaw;

  valSensor = getSensorData();
  String getData = "GET /update?api_key="+ API +"&"+ field +"="+String(RateRoll);
  sendCommand("AT+CIPMUX=1",5,"OK");
  sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
  sendCommand("AT+CIPSEND=0," +String(getData.length()+4),4,">");
  esp8266.println(getData);
  delay(100);
  countTrueCommand++;
  sendCommand("AT+CIPCLOSE=0",5,"OK");
  
  Serial.print("Roll rate[°/s] = ");
  Serial.print(RateRoll);
  Serial.print(" Pitch rate[°/s] = ");
  Serial.print(RatePitch);
  Serial.print(" Roll rate[°/s] = ");
  Serial.println(RateYaw);
  delay(50);
}
