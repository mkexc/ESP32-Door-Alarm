#include <Arduino.h>
#include <Preferences.h>
#include <WiFi.h>
#include <WiFiAP.h>
#include <WiFiMulti.h>
#include <WiFiUdp.h>
#include <WiFiScan.h>
#include <ETH.h>
#include <WiFiClient.h>
#include <WiFiSTA.h>
#include <WiFiServer.h>
#include <WiFiType.h>
#include <WiFiGeneric.h>
#include <NTPClient.h>
#include <ESP32MailHTTPClient.h>
#include <WiFiClientSecureESP32.h>
#include <ESP32_MailClient.h>
#include <RFC2047.h>
#include <ESP32TimeHelper.h>
#include <ssl_client32.h>
#include <dummy.h>
#include <EasyDDNS.h>
#include <ESPmDNS.h>
#include <ArduinoOTA.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>

#define LED 26 
#define HALL 25

//global variables
char ssid[]= "............";
char pass[]="............";
char smtpServer[]="............";
char smtpLogin[]="............";
char smtpPass[]="............";
char smtpRecipient[]="............";
char smtpSenderEmail[]="............";
char smtpSenderName[]="............";
char smtpSubject[]="............";
bool armed;
int magn,timezone=+1; //set GMT timezone
time_t currentTime,previousTime;

//objects
Preferences eeprom;
SMTPData smtpData;
WiFiUDP ntpUDP;
NTPClient ntptime(ntpUDP,timezone*3600); 
AsyncWebServer server(80);  //webserver, define port
String LedState;

//prototypes
String processor(const String& var);

//functions

void setup() {
  //get last setting before shutdown
  eeprom.begin("allarme",false);
  armed=eeprom.getInt("armed");
  eeprom.end();
  Serial.println(armed);

  //set pins
  pinMode(HALL,INPUT);
  pinMode(LED,OUTPUT);
  digitalWrite(LED,armed);
  
  Serial.begin(115200);

  //WIFI Setup
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid,pass);
  WiFi.setHostname("ESP32");
  
  while(!WiFi.isConnected())
  {
    Serial.println(".");
  }
  Serial.println("Connected.");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  
  //SPIFFS Mount
  if(!SPIFFS.begin(true))
  {
    Serial.println("Error mounting SPIFFS");
    return;
  }
  //SMTP Server & Email Settings
  smtpData.setLogin(smtpServer,587,smtpLogin,smtpPass);
  smtpData.setPriority("High");
  smtpData.setSubject(smtpSubject);
  smtpData.addRecipient(smtpRecipient);
  smtpData.setSender(smtpSenderName,smtpSenderEmail);

  //NTP Time initial setup
  ntptime.begin();  

  //WebServer Settings and handling
  if(MDNS.begin("esp32"))
  {
    Serial.println("MDNS Responder started");
  }
  server.on("/",HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", String(),false,processor);
  });
  server.on("/switch", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("ok");
    armed = !armed;
    digitalWrite(LED,armed);
    eeprom.begin("allarme",false);
    eeprom.putInt("armed",armed); //store in eeprom current value to resume from shutdown
    eeprom.end();
    delay(2000);
    request->send(SPIFFS, "/index.html", String(),false,processor);
  });
  //webapp
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");
  });
  server.on("/apple-icon-57x57.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/apple-icon-57x57.png", "image/png");
  });
  server.on("/apple-icon-60x60.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/apple-icon-60x60.png", "image/png");
  });
  server.on("/apple-icon-72x72.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/apple-icon-72x72.png", "image/png");
  });
  server.on("/apple-icon-76x76.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/apple-icon-76x76.png", "image/png");
  });
  server.on("/apple-icon-114x114.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/apple-icon-114x114.png", "image/png");
  });
  server.on("/apple-icon-120x120.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/apple-icon-120x120.png", "image/png");
  });
  server.on("/apple-icon-144x144.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/apple-icon-144x144.png", "image/png");
  });
  server.on("/apple-icon-152x152.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/apple-icon-152x152.png", "image/png");
  });
  server.on("/apple-icon-180x180.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/apple-icon-180x180.png", "image/png");
  });
  server.on("/android-icon-192x192.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/android-icon-192x192.png", "image/png");
  });
  server.on("/favicon-32x32.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/favicon-32x32.png", "image/png");
  });
  server.on("/favicon-96x96.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/favicon-96x96.png", "image/png");
  });
  server.on("/favicon-16x16.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/favicon-16x16.png", "image/png");
  });
  server.on("/manifest.json", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/manifest.json", "application/json");
  });
    server.on("/splashscreens/iphone5_splash.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/fsplashscreens/iphone5.png", "image/png");
  });
    server.on("/splashscreens/iphone6_splash.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/splashscreens/iphone6.png", "image/png");
  });
    server.on("/splashscreens/iphoneplus_splash.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/splashscreens/iphoneplus.png", "image/png");
  });
    server.on("/splashscreens/iphonex_splash.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/splashscreens/iphonex.png", "image/png");
  });
    server.on("/splashscreens/iphonexr_splash.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/splashscreens/iphonexr.png", "image/png");
  });
    server.on("/splashscreens/iphonexsmax_splash.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/splashscreens/iphonexsmax.png", "image/png");
  });
    server.on("/splashscreens/ipad_splash.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/splashscreens/ipad.png", "image/png");
  });
    server.on("/splashscreens/ipadpro1_splash.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/splashscreens/ipadpro1.png", "image/png");
  });
    server.on("/splashscreens/ipadpro3_splash.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/splashscreens/ipadpro3.png", "image/png");
  });
    server.on("/splashscreens/ipadpro2_splash.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/splashscreens/ipadpro2.png", "image/png");
  });
  
  server.begin();

  //setup DDNS
  EasyDDNS.service("............");
  EasyDDNS.client("............","............","............");

  previousTime=0;

  //OTA Setup
  ArduinoOTA.setPort(3232);
  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();
}

void loop() {
  ArduinoOTA.handle();
  EasyDDNS.update(10000);
  magn=digitalRead(HALL);
  ArduinoOTA.handle();
  currentTime=millis();
  if(!(magn)&&(armed)&&(currentTime-previousTime>=60000))
  {
    previousTime=currentTime;
    currentTime=millis();
     ntptime.update();
     String day = ntptime.getFormattedDate();
     int T=day.indexOf("T");
     smtpData.setMessage("Rilevata un' intrusione alle "+ntptime.getFormattedTime()+" il "+day.substring(0,T)+"",false);
     MailClient.sendMail(smtpData);
     Serial.println("Email sent!");
  }
  ArduinoOTA.handle();
}

//switch HTML variables
String processor(const String& var)
{
  Serial.println(var);
  if (var=="STATE")
  {
    if (armed)
    {
      LedState = "ON";
    }
    else
    {
      LedState = "OFF";
    }
    return LedState;
  }
  return String();
}