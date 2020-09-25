/*
BeBoX Network surveillance
Project for Novellini
Version 1.0

*/
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <ESP8266WiFi.h>
#include <Pinger.h>
extern "C"
{
  #include <lwip/icmp.h> // needed for icmp packet definitions
}
// Set global to avoid object removing after setup() routine
Pinger pinger;
String PingRes = "";
String retour = "";
// ST7789 TFT module connections
#define TFT_DC    D1     // TFT DC  pin is connected to NodeMCU pin D1 (GPIO5)
#define TFT_RST   D2     // TFT RST pin is connected to NodeMCU pin D2 (GPIO4)
#define TFT_CS    D8     // TFT CS  pin is connected to NodeMCU pin D8 (GPIO15)
// initialize ST7789 TFT library with hardware SPI module
// SCK (CLK) ---> NodeMCU pin D5 (GPIO14)
// MOSI(DIN) ---> NodeMCU pin D7 (GPIO13)
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
 float p = 3.1415926;
// add for 2 buttons 
const int buttonPin = 1;     // the number of the pushbutton 1 pin
const int button2Pin = 3;    // the number of the pushbutton 2 pin
int  buttonState1 = 0;
int  buttonState2 = 0;
int bouton = 0;
int modif = 1;
int x=0;
int y=0;
const unsigned char buzzer = 15; // gpio piezzo buzzer
const int RedLed = 0; //0
const int GreenLed = 10;
const int InternalLed1 = 2; // internal led near antenna
const int InternalLed2 = 16; // internal led near reset button

#define WIFI_SSID       "ACCESSPOINT"
#define WIFI_PASSWORD   "PASSWORD"

void setup() {
  // first run init
 pinMode(buttonPin, INPUT); // button 1
 pinMode(button2Pin, INPUT);// button 2
 pinMode(buzzer, OUTPUT);   // buzzer 
 pinMode(RedLed, OUTPUT); // Red Led
 pinMode(GreenLed, OUTPUT); // Green Led
 uint16_t time = millis();
 // if the display has CS pin try with SPI_MODE0
 tft.init(240, 240, SPI_MODE2);    // Init ST7789 display 240x240 pixel
 tft.setRotation(0);  
 tft.fillScreen(ST77XX_BLACK);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
 //WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
 LCDShow ("    Connecting...",0,112,ST77XX_WHITE);
 LCDShow ("    to "+String(WIFI_SSID),0,144,ST77XX_WHITE);
  while (WiFi.status() != WL_CONNECTED) 
  {    
    digitalWrite(GreenLed, HIGH); // on
    delay(250);
    digitalWrite(GreenLed, LOW); // off
    delay(250);
  }
  tft.fillScreen(ST77XX_BLACK);
  if (WiFi.status() != WL_CONNECTED) {
    LCDShow ("    FAILED",0,112,ST77XX_RED);
  } else {
    LCDShow ("    WIFI : OK",0,96,ST77XX_WHITE);
    LCDShow ("IP: "+WiFi.localIP().toString(),0,128,ST77XX_WHITE);
  }
  delay(5000);
  tft.fillScreen(ST77XX_BLACK);
  LCDShow( " Network controller",0,0,ST77XX_YELLOW);
  LCDShow( " ------------------",0,16,ST77XX_BLUE);
  LCDShow("VPN Mantova",0,48,ST77XX_WHITE);
  LCDShow("VPN Contres",0,96,ST77XX_WHITE);
  LCDShow("VPN Thenay",0,144,ST77XX_WHITE);
  LCDShow("WAN access test",0,192,ST77XX_WHITE);
}

void loop() {
  buttonState1 = digitalRead(buttonPin);
  buttonState2 = digitalRead(button2Pin); 
  bouton=0;
  if (buttonState1 == LOW){bouton = bouton + 1;} 
  if (buttonState2 == LOW){bouton = bouton + 2;}
  
  
  TestNetwork("10.1.1.234",1,1000);
  tft.fillRect(0,72 , 240, 16, ST77XX_BLACK);
    if( retour ==  "Er") 
      {
        LCDShow ("     Ping Error",0,72,ST77XX_BLUE);
    digitalWrite(RedLed, HIGH); // off
    digitalWrite(GreenLed, LOW); // off
        } else {
          if (retour == "-1")
            {
            LCDShow ("   Ping Time Out",0,72,ST77XX_RED);
      digitalWrite(RedLed, HIGH); // off
      digitalWrite(GreenLed, LOW); // off
      
            } else {
          LCDShow ("   Ping " + retour + " ms",0,72,ST77XX_GREEN);
      digitalWrite(GreenLed, HIGH); // off
      //digitalWrite(RedLed, LOW); // off
      delay(250);
          }
       } 
  digitalWrite(GreenLed, LOW); // off
  TestNetwork("10.65.37.4",1,1000);
  tft.fillRect(0,120 , 240, 16, ST77XX_BLACK);
    if( retour ==  "Er") 
      {
        LCDShow ("     Ping Error",0,120,ST77XX_BLUE);
    digitalWrite(RedLed, HIGH); // off
    digitalWrite(GreenLed, LOW); // off
        } else {
          if (retour == "-1")
            {
            LCDShow ("   Ping Time Out",0,120,ST77XX_RED);
      digitalWrite(RedLed, HIGH); // off
      digitalWrite(GreenLed, LOW); // off
      
            } else {
          LCDShow ("   Ping " + retour + " ms",0,120,ST77XX_GREEN);
      digitalWrite(GreenLed, HIGH); // off
      //digitalWrite(RedLed, LOW); // off
      delay(250);
          }
       } 
  digitalWrite(GreenLed, LOW); // off
  TestNetwork("10.65.50.102",1,1000);
  tft.fillRect(0,168 , 240, 16, ST77XX_BLACK);
    if( retour ==  "Er") 
      {
        LCDShow ("     Ping Error",0,168,ST77XX_BLUE);
    digitalWrite(RedLed, HIGH); // off
    digitalWrite(GreenLed, LOW); // off
        } else {
          if (retour == "-1")
            {
            LCDShow ("   Ping Time Out",0,168,ST77XX_RED);
      digitalWrite(RedLed, HIGH); // off
      digitalWrite(GreenLed, LOW); // off
      
            } else {
          LCDShow ("   Ping " + retour + " ms",0,168,ST77XX_GREEN);
      digitalWrite(GreenLed, HIGH); // off
      //digitalWrite(RedLed, LOW); // off
      delay(250);
          }
       } 
  digitalWrite(GreenLed, LOW); // off
  TestNetwork("216.58.215.36",1,1000);
  tft.fillRect(0,216 , 240, 16, ST77XX_BLACK);
    if( retour ==  "Er") 
      {
        LCDShow ("     Ping Error",0,216,ST77XX_BLUE);
    digitalWrite(RedLed, HIGH); // off
    digitalWrite(GreenLed, LOW); // off
        } else {
          if (retour == "-1")
            {
            LCDShow ("   Ping Time Out",0,216,ST77XX_RED);
      digitalWrite(RedLed, HIGH); // off
      digitalWrite(GreenLed, LOW); // off
      
            } else {
          LCDShow ("   Ping " + retour + " ms",0,216,ST77XX_GREEN);
      digitalWrite(GreenLed, HIGH); // off
      //digitalWrite(RedLed, LOW); // off
      delay(250);
          }
       } 
  digitalWrite(GreenLed, LOW); // off     
  delay(10000); // wait 10 sec   
  digitalWrite(RedLed, LOW);  
}

void LCDShow(String texte, int x, int y, uint16_t color) {
  tft.setCursor(x, y);
  tft.setTextColor(color);
  tft.setTextWrap(false);
  tft.setTextSize(2);
  tft.println(texte);
}


void TestNetwork(String IP, int NbPing, int TimeOut){
  retour = "";
  pinger.OnEnd([](const PingerResponse& response)
  {
     if(response.TotalReceivedResponses > 0)    
        { 
          // response.AvgResponseTime
          retour = String(response.AvgResponseTime);
         // retour = "ok";
        } 
        else 
        {
          // Time out
          retour = "-1";
        }
      return true;
  });
  if(pinger.Ping(IP,NbPing,TimeOut) == false){retour = "Er";} 
  delay(TimeOut+500);
}
