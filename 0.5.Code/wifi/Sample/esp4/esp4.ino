#include <SoftwareSerial.h>
#include <EEPROM.h>

// Kích thước EEPROM (phụ thuộc vào loại Arduino, với Nano là 1024 byte)
const int EEPROM_SIZE = EEPROM.length();
int eepromAddress = 0; // Địa chỉ hiện tại trong EEPROM
unsigned long iterationCount = 0; // Đếm số lần lặp

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
boolean found = false; 
int valSensor = 1;
SoftwareSerial esp8266(RX,TX); 
 
  
void setup() {
  Serial.begin(9600);
  esp8266.begin(115200);
  sendCommand("AT",5,"OK");
  sendCommand("AT+CWMODE=1",5,"OK");
  sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");
}

void loop() {
 valSensor = getSensorData();

 // 2. Ghi giá trị vào EEPROM
 EEPROM.put(eepromAddress, valSensor);
 String getData = "GET /update?api_key="+ API +"&"+ field +"="+String(valSensor);
 sendCommand("AT+CIPMUX=1",5,"OK");
 sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
 sendCommand("AT+CIPSEND=0," +String(getData.length()+4),4,">");
 esp8266.println(getData);
 delay(100);
 countTrueCommand++;
 sendCommand("AT+CIPCLOSE=0",5,"OK");
 Serial.print(" Sensor value: ");
 Serial.println(valSensor);
}

int getSensorData(){
  return random(100); // Replace with your own sensor code
}

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
    Serial.println("OKI");
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
