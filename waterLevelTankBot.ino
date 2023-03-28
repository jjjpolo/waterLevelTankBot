// Custom Libs
#include "Bot.h"
#include "Credentials.h"
#include "Tank.h"

// Libs for wifi connectivity
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

// Libs for async web server
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebSrv.h>

// Libs for OTA
#include "OTAInterfaceManager.h"

#ifdef ARDUINO_ESP8266_NODEMCU
  #define trigPin D1 // attach pin Trig of JSN-SR04T
  #define echoPin D2 // attach pin Echo of JSN-SR04T
#elif ESP8266
  #define trigPin 0 // attach pin Trig of JSN-SR04T
  #define echoPin 2 // attach pin Echo of JSN-SR04T 
#endif

// Software defines
#define maxTankDepth            100 // Max tank depth in centimeters
#define minTankDepth            20   // Min tank depth in centimeters
#define percentageAlarmTrigger  50  // Percentage level that triggers the alarm.
#define sampleRate              500 // Delay between ultrasonic measurements.

WiFiClientSecure wifiClient;
Bot myBot("TankBot", Credentials::telegramToken, Credentials::telegramChatID, &wifiClient);
AsyncWebServer myWebServer(80);
Tank myTank(trigPin, echoPin, maxTankDepth, minTankDepth, percentageAlarmTrigger, &myBot, &myWebServer);
OTAInterfaceManager myOTAServer(Credentials::OTAserverHostname, Credentials::OTAserverPassword);

void setup()
{
  Serial.begin(115200);
  Serial.println("Ultrasonic Sensor HC-SR04 Test V0.2");
  
  WiFi.begin(Credentials::ssid, Credentials::password);
  Serial.print("Connecting to Wifi...");
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.print("/nWiFi connected. IP address: ");
  Serial.println(WiFi.localIP());
  myBot.sendMessage("Water Level Tank Bot is running. Check me out at http://" 
                    + WiFi.localIP().toString() + ":80/");
}

void loop()
{
  // Using delay affects the OTA functionality so the following code is a 
  // workaround for including a delay, using millis(), that does not affect
  // the OTA functionality.
  unsigned long timeSinceLastStart = millis();
  while(millis() - timeSinceLastStart < sampleRate)
  {
    myOTAServer.run();
  }

  myTank.run();
}