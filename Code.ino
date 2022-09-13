/* 
 @Autor: SagnikC
 @Date: 03-Sept-2022
 @Modify-1: 10-Sept-2022
 GthubID: https://github.com/FunWithElectrobots
  
 Link for ESP8266 library
 https://arduino.esp8266.com/stable/package_esp8266com_index.json
 */
 
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <EEPROM.h>

char AUTH[] = "_-ArP1OBlqPKyi33-K6lZh17HxYGff1E"; //Blynk Auth Code
char WIFI_SSID[] = "FIBRE_4g"; //Wifi name
char WIFI_PASS[] = "togre123";  //Wifi Password
int mq2 = A0; // smoke sensor is connected with the analog pin A0 
int data = 0; 
int load1, load2;
int wifiFlag = 1;

#define Relay1  D0 //Relay 1 connected to D0
#define Relay2  D1 //Relay 2 connected to D1
#define Switch1 D5 //Switch 1 connected to D5
#define Switch2 D6 //Switch 2 connected to D6
#define wifiLed D4 //OutputLED to show status of wifi connectivity

BlynkTimer timer;

void setup() {  

Serial.begin(9600);
WiFi.begin(WIFI_SSID, WIFI_PASS);
Blynk.config(AUTH);

pinMode(Switch1, INPUT_PULLUP);
pinMode(Switch2, INPUT_PULLUP);
pinMode(Relay1, OUTPUT); 
pinMode(Relay2, OUTPUT); 
pinMode(wifiLed, OUTPUT);
pinMode(mq2, INPUT);

EEPROM.begin(512);
load1 = EEPROM.read(1);
load2 = EEPROM.read(2);

Relays();
timer.setInterval(1000L, getSendData);      // calls the function every 1second to get data from MQ2 sensor.
timer.setInterval(1000L, checkBlynkStatus); // check if Blynk server is connected every 3 seconds
delay(100);
}

void checkBlynkStatus() {
  bool isconnected = Blynk.connected();
  if (isconnected == false) {wifiFlag = 1;
    digitalWrite(wifiLed, HIGH); //Turn off WiFi LED
  }
  if (isconnected == true) {
  if(wifiFlag==1){wifiFlag = 0;
  update_blynk();
  }  
    digitalWrite(wifiLed, LOW); //Turn on WiFi LED
  }
}

void loop() {

if (WiFi.status() != WL_CONNECTED){
    //Serial.println("WiFi Not Connected");
  }
  else{
    //Serial.println("WiFi Connected");
    Blynk.run();
  }
 
if(wifiFlag==0){with_internet();}
           else{without_internet();}

 timer.run(); //simple timer initiated
 
}

BLYNK_WRITE(V0){
load1 = param.asInt();
Relays();
}

BLYNK_WRITE(V1){
load2 = param.asInt(); 
Relays();
}

void with_internet(){
     if(digitalRead(Switch1) == LOW){
      load1 = !load1;
      Relays(); 
      update_blynk();
      delay(300); 
    }
else if(digitalRead(Switch2) == LOW){
      load2 = !load2;
      Relays(); 
      update_blynk();
      delay(300);
    }
}

void without_internet(){
     if(digitalRead(Switch1) == LOW){
      load1 = !load1;
      Relays(); 
      delay(300); 
    }
else if(digitalRead(Switch2) == LOW){
      load2 = !load2;
      Relays(); 
      delay(300);
    }
}

void update_blynk(){
Blynk.virtualWrite(V0, load1);  
Blynk.virtualWrite(V1, load2);
}

void Relays(){
digitalWrite(Relay1, load1);  
digitalWrite(Relay2, load2);
write_eeprom();
}

void write_eeprom(){
EEPROM.write(1, load1);
EEPROM.write(2, load2);
EEPROM.commit(); 
}

void getSendData()
{
data = analogRead(mq2); 
  Blynk.virtualWrite(V2, data);
 
  if (data > 700 )
  {
    Blynk.notify("Smoke Detected!"); 
  }
 
}