#include <SoftwareSerial.h>
#include <EEPROM.h>
#include <TimerOne.h> // Thư viện để điều khiển Timer1

volatile bool flag17s = false; // Cờ để báo khi ngắt 17s xảy ra
volatile int counter = 0;      // Bộ đếm số lần timer ngắt (tính từng giây)

#define RX 8
#define TX 10 
String AP = "LAU_3";       // AP NAME
String PASS = "nhatro29almn#L3"; // AP PASSWORD
String API = "ZY4AS3D7QC7IMFEC";   // Write API KEY
String HOST = "api.thingspeak.com";
String PORT = "80";
String field = "field1";
int countTrueCommand;
int countTimeCommand; 
int flag;
boolean found = false; 
int valSensor = 1;
SoftwareSerial esp8266(RX,TX); 
 

// Biến EEPROM
int eepromAddressSend = 0;             // Địa chỉ hiện tại trong EEPROM
int eepromAddressGet = 0;             // Địa chỉ hiện tại trong EEPROM
const int EEPROM_SIZE = EEPROM.length();

void setup() {
  Serial.begin(9600);
  esp8266.begin(115200);
  clearEEPROM();
  sendCommand("AT",5,"OK");
  sendCommand("AT+CWMODE=1",5,"OK");
  sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");
  Timer1.initialize(1000000); // 1 giây (1.000.000 microseconds)
  Timer1.attachInterrupt(timerCallback); // Gắn hàm callback cho Timer
}

void loop() {
 valSensor = getSensorData(); 
 EEPROM.put(eepromAddressSend, valSensor);
 
 Serial.print(valSensor);
 Serial.print("   ");
 
  int sensorValue;
 EEPROM.get(eepromAddressGet, sensorValue);
 Serial.println(sensorValue); 

 if (flag17s) {
    flag17s = false; // Reset cờ

    // Thực thi tác vụ khi đạt 17 giây
    String getData = "GET /update?api_key="+ API +"&"+ field +"="+String(sensorValue);
    sendCommand("AT+CIPMUX=1",5,"OK");
    sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
    sendCommand("AT+CIPSEND=0," +String(getData.length()+4),4,">");
    esp8266.println(getData);
 //countTrueCommand++;
    sendCommand("AT+CIPCLOSE=0",5,"OK");
 
    //delay(500);
    eepromAddressGet += sizeof(int); // Gửi dữ liệu lên ThingSpeak
    Serial.println("Interrupt triggered: 17 seconds passed!");
}

 
 eepromAddressSend += sizeof(int);
 if (eepromAddressSend >= EEPROM_SIZE) {
      eepromAddressSend = 0; // Quay lại đầu EEPROM nếu hết dung lượng
    }
 if (eepromAddressGet >= EEPROM_SIZE) {
      eepromAddressGet = 0; // Quay lại đầu EEPROM nếu hết dung lượng
    }
 

}

int getSensorData(){
  return random(100); // Replace with your own sensor code
}

void sendCommand(String command, int maxTime, char readReplay[]) {
  //Serial.print(countTrueCommand);
  //Serial.print(". at command => ");
  //Serial.print(command);
  //Serial.print(" ");
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
    //Serial.println("OK"); 
    flag = countTrueCommand;
    countTrueCommand++;
    countTimeCommand = 0;
  }
  
  if(found == false)
  {
    //Serial.println("Fail");
    countTrueCommand = 0;
    countTimeCommand = 0;
    flag = -1;
  }
  
  found = false;
 }
 void clearEEPROM() {
  for (int i = 0; i < EEPROM_SIZE; i++) {
    EEPROM.write(i, 0xFF); // Ghi giá trị mặc định (0xFF) vào tất cả các byte
  }
}

void timerCallback() {
  counter++; // Tăng bộ đếm mỗi giây

  if (counter >= 20) { // Kiểm tra nếu đã đạt 17 giây
    counter = 0;       // Reset bộ đếm
    flag17s = true;    // Đặt cờ để thực thi trong loop()
  }
}
