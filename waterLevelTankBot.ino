// Libs for wifi connectivity
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

// Libs for Wifi Manager (Web Server)
#include <ESPAsyncWiFiManager.h> //https://github.com/tzapu/WiFiManager WiFi Configuration Magic

// Custom Libs
#include "Bot.h"
#include "Credentials.h"
#include "Tank.h"
#include "ConfigManager.h"

// #define NodeMCU
#define MANAGER

#ifdef NodeMCU
#define trigPin D1 // attach pin Trig of JSN-SR04T
#define echoPin D2 // attach pin Echo of JSN-SR04T
#else
#define trigPin 0 // attach pin Trig of JSN-SR04T
#define echoPin 2 // attach pin Echo of JSN-SR04T
#endif

// Libs for OTA
// #include "OTAInterfaceManager.h"
// OTAInterfaceManager myOTAServer(Credentials::OTAserverHostname, Credentials::OTAserverPassword);

ConfigManager *configManager = ConfigManager::getInstance();
AsyncWiFiManagerParameter customTelegramToken("TelegramToken", "Telegram bot token", "", 47);
AsyncWiFiManagerParameter customTelegramChatID("TelegramChatID", "Telegram bot chat ID", "", 15);

// Callback when wifi manager is done.
void saveConfigCallback()
{
  Serial.println("Should save config");
  configManager->setParameter("telegram_token", customTelegramToken.getValue());
  configManager->setParameter("telegram_chatID", customTelegramChatID.getValue());
  configManager->setParameter("rebootNeeded", "1");
}

void setup()
{
  Serial.begin(115200);

#ifdef MANAGER
  // Set web server port number to 80
  AsyncWebServer server(80);
  DNSServer dns;
  Serial.println("\n\nWifi Manager is running on port 80\n\n");
  AsyncWiFiManager wifiManager(&server, &dns);
  wifiManager.addParameter(&customTelegramToken);
  wifiManager.addParameter(&customTelegramChatID);
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  wifiManager.autoConnect("HydroNotify", "abc.123");
  if (configManager->getParameter("rebootNeeded", "0") == "1")
  {
    configManager->setParameter("rebootNeeded", "0");
    Serial.println("Reboot needed after 1st configuration with wifi manager...");
    ESP.restart();
  }
#else
  WiFi.begin(Credentials::ssid, Credentials::password);
  Serial.print("Connecting to Wifi...");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.print("\nWiFi connected. IP address: ");
  Serial.println(WiFi.localIP());
#endif
}

void loop()
{
  Serial.println("\n\nUltrasonic Sensor HC-SR04 Test V0.2\n\n");
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
  WiFiClientSecure wifiClient;
  Bot myBot("TankBot", &wifiClient,
            configManager->getParameter("telegram_token", Credentials::telegramToken),
            configManager->getParameter("telegram_chatID", Credentials::telegramChatID));
  // Bot myBot("TankBot", &wifiClient);
  Tank myTank(trigPin, echoPin, &myBot);
  myBot.sendMessage("Water Level Tank Bot is running. Check me out at http://" + WiFi.localIP().toString() + "/");
  while ((1))
  {
    myTank.run();
  }
}