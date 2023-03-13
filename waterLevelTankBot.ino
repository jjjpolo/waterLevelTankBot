#include "Tank.h"
#include "Bot.h"
#include "Webserver.h"

// Hardware defines
#define trigPin D2  //attach pin D3 Arduino to pin Trig of JSN-SR04T
#define echoPin D1  // attach pin D2 Arduino to pin Echo of JSN-SR04T

// Software defines
// Tank level 0 (it is actually 20cm) means full tank
#define maxTankLevel 20
#define minTankLevel 100
#define triggerAlertTankLevel 30

Bot myBot("TestBot");
Tank myTank{ trigPin, echoPin, maxTankLevel, minTankLevel, triggerAlertTankLevel, &myBot};

void setup() {
  ESP.wdtDisable();  // Disabling watchdog to prevent NodeMCU from reseting due to not visiting loop function that often.
  Serial.begin(115200);
  Serial.println("Ultrasonic Sensor HC-SR04 Test V0.1");
  delay(2000);
}

void loop() {
  myTank.smartJobRoutine();
  delay(250);
}