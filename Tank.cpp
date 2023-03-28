#include "Tank.h"

Tank::Tank(int sensorTriggerPin, int sensorEchoPin, long maxTankLevel, long minTankLevel, long triggerAlertTankLevel, Bot *botReference, AsyncWebServer *serverReference) : 
m_sensorTriggerPin(sensorTriggerPin),
m_sensorEchoPin(sensorEchoPin),
m_maxTankLevel(maxTankLevel),
m_minTankLevel(minTankLevel),
m_triggerAlertTankLevel(triggerAlertTankLevel),
m_tankBot(botReference),
m_TankWebServer(serverReference)
{
  Serial.println("Tank constructor");
  pinMode(m_sensorTriggerPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(m_sensorEchoPin, INPUT);
  
  m_TankWebServer->on("/", HTTP_GET, [&](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", WebServerContent::index_html);
  });
  m_TankWebServer->on("/level", HTTP_GET, [&](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(m_lastDistanceMeasurement).c_str());
  });

  m_TankWebServer->begin();
}

long Tank::getCurrentDistanceMeasure()
{
  long duration{};
  long distance{};

  // Clears the m_sensorTriggerPin condition
  digitalWrite(m_sensorTriggerPin, LOW); //
  delay(1);

  // Sets the m_sensorTriggerPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(m_sensorTriggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(m_sensorTriggerPin, LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(m_sensorEchoPin, HIGH);

  // Calculating the distance
  distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
  //ESP.wdtFeed();                   // Somehow this method got Node MCU stuck resetting WDT so let's keep it calmed.

  m_lastDistanceMeasurement = distance; // analyzeLastDistance & webServer will make use of this.
  return distance;
}

void Tank::printCurrentDistance()
{
  Serial.print("Current Distance: ");
  Serial.print(getCurrentDistanceMeasure());
  Serial.println(" cm");
}

void Tank::sendChatAlert(const notificationType &currentNotification)
{
  switch (currentNotification)
  {
  case notificationType::emptyTank:
    m_tankBot->sendMessage("Empty Tank.");
    break;
  case notificationType::alertTankLevel:
    m_tankBot->sendMessage("Alert Tank Level.");
    break;
  case notificationType::fullTank:
    m_tankBot->sendMessage("Full tank.");
    break;
  default:
    m_tankBot->sendMessage("UNKNOWN status");
    break;
  }
}

void Tank::actionWhile(const state &currentSate)
{
  long currentLevel = getCurrentDistanceMeasure();
  switch (currentSate)
  {
  case state::tankLevelIsTheSame:
    while (currentLevel == getCurrentDistanceMeasure())
    {
      delay(250);
      Serial.println("[WIP] Waiting for any action...");
    }
    if (getCurrentDistanceMeasure() > currentLevel)
    {
      actionWhile(state::levelKeepsDecreasing);
    }
    else
    {
      actionWhile(state::refillingDetected);
    }
    break;
  case state::levelKeepsDecreasing:
    while (getCurrentDistanceMeasure() >= currentLevel)
    {
      Serial.println("[WIP] Tank level keeps decreasing or hasn't reached alertTankLevel");
      Serial.print("[WIP] Current level is: ");
      Serial.println(getCurrentDistanceMeasure());
      if (getCurrentDistanceMeasure() == m_minTankLevel)
      {
        sendChatAlert(notificationType::emptyTank);
        actionWhile(state::tankKeepsEmpty);
        return;
      }
      delay(250);
    }
    actionWhile(state::refillingDetected);
    break;
  case state::tankKeepsEmpty:
    while (getCurrentDistanceMeasure() == m_minTankLevel)
    {
      Serial.println("[WIP] Tank is empty!");
      delay(250);
    }
    actionWhile(state::refillingDetected);
    break;
  case state::refillingDetected:
    Serial.println("[WIP] Refilling action detected, keep going...");
    break;
  case state::startUsingFullTank: // This state aims to prevent spam after getting the tank fulfilled
    while (getCurrentDistanceMeasure() <= m_maxTankLevel)
    {
      Serial.println("Tank has not been used after refilling it. Nice!");
      delay(250);
    }
    break;
  default:
    Serial.println("[WIP] Unknown State");
    break;
  }
}

void Tank::analyzeLastDistance()
{
  if (m_lastDistanceMeasurement == m_triggerAlertTankLevel)
  {
    sendChatAlert(notificationType::alertTankLevel);
    actionWhile(state::tankLevelIsTheSame);
  }
  else if (m_lastDistanceMeasurement == m_maxTankLevel)
  {
    sendChatAlert(notificationType::fullTank);
    actionWhile(state::startUsingFullTank);
  }
  else if (m_lastDistanceMeasurement == m_minTankLevel)
  {
    sendChatAlert(notificationType::emptyTank);
    actionWhile(state::tankKeepsEmpty);
  }
}

String Tank::getLastMeasure()
{
  return String(m_lastDistanceMeasurement);
}

float Tank::convertDistanceToPercentage(const int &distance)
{
    m_lastPercentageOfWaterInTank = ((m_maxTankDepth- distance) / (m_maxTankDepth - m_minTankDepth)) * 100;
    return m_lastPercentageOfWaterInTank;
}

void Tank::smartJobRoutine()
{
  printCurrentDistance();
  analyzeLastDistance();
}