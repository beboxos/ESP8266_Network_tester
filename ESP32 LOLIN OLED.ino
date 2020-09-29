#include <WiFi.h>
#include <ESP32Ping.h>
#include <Wire.h>               // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306Wire.h"        // legacy: #include "SSD1306.h"
#include "time.h"
SSD1306Wire display(0x3c, 5, 4);   // ADDRESS, SDA, SCL  -  SDA and SCL usually populate automatically based on your board's pins_arduino.h
// configuration
char * SSIDList[] = {"AP1","AP2"};
char * SSIDPass[] = {"***********","*********"};
char * IPSurv[] = {"NOM1:10.1.1.234&NOM2:10.65.37.4&NOM3:10.65.50.102","Router:192.168.2.1&Google:216.58.215.36"};
#define NbPass (sizeof(SSIDPass)/sizeof(char *)) //array size  
const int RedLed = 2 ;// pin on red led
const int GreenLed = 14 ;
  
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;
// usable functions 

// *********** Some of Split function ****************
String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
// transform string in IP *************************
IPAddress str2IP(String str) {
    IPAddress ret( getIpBlock(0,str),getIpBlock(1,str),getIpBlock(2,str),getIpBlock(3,str) );
    return ret;
}
int getIpBlock(int index, String str) {
    char separator = '.';
    int found = 0;
    int strIndex[] = {0, -1};
    int maxIndex = str.length()-1;
  
    for(int i=0; i<=maxIndex && found<=index; i++){
      if(str.charAt(i)==separator || i==maxIndex){
          found++;
          strIndex[0] = strIndex[1]+1;
          strIndex[1] = (i == maxIndex) ? i+1 : i;
      }
    }    
    return found>index ? str.substring(strIndex[0], strIndex[1]).toInt() : 0;
}
// *****************************************************
int Conf = -1; // configuration number -1 none (return index for SSID and pass etc
void setup() {
  pinMode(RedLed, OUTPUT); // Red Led
  pinMode(GreenLed, OUTPUT); // Green Led
  Serial.begin(115200);
  delay(10);
  // display
  display.init();
  display.flipScreenVertically();
  // scan for wifi AP
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  Serial.println("scan start");
  display.clear();
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 24, "Scanning WIFI Networks");
  digitalWrite(GreenLed, HIGH); // on
  display.display();
  delay(1000);
  while ( Conf == -1){
    // while nbo found perform Scan for know Access point
    digitalWrite(GreenLed, HIGH); // off
    int n = WiFi.scanNetworks();
    if (n == 0) {
        Serial.println("no networks found");
        display.clear();
        display.setFont(ArialMT_Plain_24);
        display.drawString(0, 24, "No Network");
        display.display();
        delay(1000);
    } else {
        Serial.print(n);
        Serial.println(" networks found");
        display.clear();
        display.setFont(ArialMT_Plain_10);
        display.drawString(0, 0, "Found "+ String(n)+" Network(s)");
        for (int i = 0; i < n; ++i) {
          digitalWrite(GreenLed, HIGH); // on
          delay(100);
              for(int x=0; x<NbPass; ++x){
                if (String(SSIDList[x]) == WiFi.SSID(i)) {
                  Serial.println("Find "+  WiFi.SSID(i) + " in configuration list at pos : "+String(x));
                  Conf = x;
                  // find pass
                  Serial.println("Password is " + String(SSIDPass[x]));
                  Serial.println("");
                  display.setFont(ArialMT_Plain_10);
                  display.drawString(0, 24,WiFi.SSID(i)+ " is Found");
                  display.display();        
                  delay(1000);          
                }
              }
              if(Conf != -1) { break; }     
              digitalWrite(GreenLed, LOW); // on  
              delay(100);       
          }
    }
    delay(1000);
    digitalWrite(GreenLed, LOW); // off
  }
  
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println("Connecting to WiFi");
  //display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.clear();
  display.setFont(ArialMT_Plain_24);
  display.drawString(0, 24, "Connecting");
  display.display();
  digitalWrite(GreenLed, HIGH); // on
  delay(250);
  digitalWrite(GreenLed, LOW); // on
  delay(250);
  WiFi.begin(SSIDList[Conf], SSIDPass[Conf]);
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(GreenLed, HIGH); // on
    delay(200);
    digitalWrite(GreenLed, LOW); // off
    delay(200);
    Serial.print(".");
  }
  display.clear();
  display.setFont(ArialMT_Plain_24);
  display.drawString(0, 24, "Connected");
  display.display();
  Serial.println();
  Serial.print("WiFi connected with ip ");  
  Serial.println(WiFi.localIP());
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();
}

void loop() {
  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 0, " Network Monitor");
  display.setFont(ArialMT_Plain_10);
  printLocalTime();
  int index = 0;
  int ligne= 30; // start line for display
  while(getValue(String(IPSurv[Conf]),'&',index)!="") {
    String nom = getValue(getValue(String(IPSurv[Conf]),'&',index),':',0);
    String ip = getValue(getValue(String(IPSurv[Conf]),'&',index),':',1);
    Serial.println("nom : "+ nom);
    Serial.println("IP  : "+ ip);
    display.drawString(0, ligne, getValue(getValue(String(IPSurv[Conf]),'&',index),':',0));
    if(Ping.ping(str2IP(ip),1)) 
      {
        display.drawString(75, ligne, String(Ping.averageTime())+" ms");
        digitalWrite(GreenLed, HIGH); // on  
        delay(100);      
        } else {        
        display.drawString(75, ligne,"is Down");
        digitalWrite(RedLed, HIGH);
        delay(100);
      }
      ligne=ligne+10;
    ++index;
    digitalWrite(GreenLed, LOW); 
    delay(100);
    }
  display.display();
  delay(500);
  digitalWrite(RedLed, LOW);
  }

void printLocalTime(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
    }
  char timeHour[30];
  strftime(timeHour,30, " %d %B %Y %H:%M", &timeinfo);
  display.drawString(0, 17, String(timeHour));
}
