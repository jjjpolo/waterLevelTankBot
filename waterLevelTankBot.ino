#include "Bot.h"
#include "Credentials.h"
#include "Tank.h"
#include "Webserver.h"

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

// Hardware defines
#define trigPin D2 // attach pin D3 Arduino to pin Trig of JSN-SR04T
#define echoPin D1 // attach pin D2 Arduino to pin Echo of JSN-SR04T

// Software defines
// Tank level 0 (it is actually 20cm) means full tank
#define maxTankLevel 20
#define minTankLevel 100
#define triggerAlertTankLevel 30

WiFiClientSecure wifiClient;
Bot myBot("TankBot", Credentials::telegramToken, Credentials::telegramChatID, &wifiClient);
Tank myTank(trigPin, echoPin, maxTankLevel, minTankLevel, triggerAlertTankLevel, &myBot);

void setup()
{
  Serial.begin(115200);
  Serial.println("Ultrasonic Sensor HC-SR04 Test V0.2");
  
  WiFi.begin(Credentials::ssid, Credentials::password);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.print("WiFi connected. IP address: ");
  Serial.println(WiFi.localIP());
  
  myBot.sendMessage("Water Level Tank Bot is running. Checkme out at http://" 
                    + WiFi.localIP().toString() + ":80");
}

void loop()
{
  myTank.smartJobRoutine();
  delay(250);
}