// Libs for wifi connectivity
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

// Libs for OTA
//#include "OTAInterfaceManager.h"

// Custom Libs
#include "Bot.h"
#include "Credentials.h"
#include "Tank.h"
#include "ConfigManager.h"

#define NodeMCU

#ifdef NodeMCU
  #define trigPin D1 // attach pin Trig of JSN-SR04T
  #define echoPin D2 // attach pin Echo of JSN-SR04T
#else
  #define trigPin 0 // attach pin Trig of JSN-SR04T
  #define echoPin 2 // attach pin Echo of JSN-SR04T 
#endif

// Software defines
#define sampleRate              500 // Delay between ultrasonic measurements and OTA handling.

//OTAInterfaceManager myOTAServer(Credentials::OTAserverHostname, Credentials::OTAserverPassword);

void setup()
{
  Serial.begin(115200);
  Serial.println("\n\nUltrasonic Sensor HC-SR04 Test V0.2\n\n");
  
  WiFi.begin(Credentials::ssid, Credentials::password);
  Serial.print("Connecting to Wifi...");
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.print("\nWiFi connected. IP address: ");
  Serial.println(WiFi.localIP());
}

void loop()
{
  // Using delay affects the OTA functionality so the following code is a 
  // workaround for including a delay, using millis(), that does not affect
  // the OTA functionality.
  /*
  unsigned long timeSinceLastStart = millis();
  while(millis() - timeSinceLastStart < sampleRate)
  {
    myOTAServer.run();
  }
  */
ConfigManager *configManager = ConfigManager::getInstance();
WiFiClientSecure wifiClient;
Bot myBot("TankBot", &wifiClient, 
          configManager->getParameter("telegram_token", Credentials::telegramToken),
          configManager->getParameter("telegram_chatID", Credentials::telegramChatID));
//Bot myBot("TankBot", &wifiClient);
Tank myTank(trigPin, echoPin, &myBot);
myBot.sendMessage("Water Level Tank Bot is running. Check me out at http://" 
                    + WiFi.localIP().toString() + "/");
 while ((1))
 {
    myTank.runV2();
 } 
}