#include "Tank.h"

Tank::Tank(int sensorTriggerPin, int sensorEchoPin, int maxTankDepth, int minTankDepth, int percentageAlarmTrigger, Bot *botReference, AsyncWebServer *serverReference) : 
m_sensorTriggerPin(sensorTriggerPin),
m_sensorEchoPin(sensorEchoPin),
m_maxTankDepth(maxTankDepth),
m_minTankDepth(minTankDepth),
m_percentageAlarmTrigger(percentageAlarmTrigger),
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
    request->send_P(200, "text/plain", String(m_lastPercentageOfWater).c_str());
  });
  m_TankWebServer->on("/restart", HTTP_GET, [&](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String("Restart").c_str());
    ESP.restart();
  });

  m_TankWebServer->on("/sendParameters", HTTP_POST, [&](AsyncWebServerRequest *request){
    handlePostParameters(request);
  });

    m_TankWebServer->on("/getParameters", HTTP_GET, [&](AsyncWebServerRequest *request){
    handleGetParameters(request);
  });

  m_TankWebServer->begin();
}

void Tank::handlePostParameters(AsyncWebServerRequest *request)
{
 if (request->contentType() != "application/json") {
    request->send(400, "text/plain", "Invalid content type");
    return;
  }
  String json_string = request->getParam("plain", true)->value();

  // Parse the JSON document
  DynamicJsonDocument json_obj(json_string.length() + 1);
  DeserializationError error = deserializeJson(json_obj, json_string);
  if (error) 
  {
    request->send(400, "text/plain", "Invalid JSON data");
    return;
  }
  m_maxTankDepth = (int)json_obj["maxDepth"];
  m_minTankDepth = (int)json_obj["minDepth"];
  m_percentageAlarmTrigger = (int)json_obj["alarmTrigger"];
}

void Tank::handleGetParameters(AsyncWebServerRequest *request)
{
   DynamicJsonDocument response(1024);
  response["maxDepth"] = m_maxTankDepth;
  response["minDepth"] = m_minTankDepth;
  response["alarmTrigger"] = m_percentageAlarmTrigger;
  String jsonResponse_string;
  serializeJson(response, jsonResponse_string);
  
  // Send the JSON response
  request->send(200, "application/json", jsonResponse_string);
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

void Tank::actionWhen(const state &currentSate)
{
  int percentageOfWaterCheckpoint = getCurrentPercentageOfWater();
  switch (currentSate)
  {
  case state::alertTankLevel:
    while (getCurrentPercentageOfWater() ==percentageOfWaterCheckpoint) // Has not changed
    {
      delay(100);
      Serial.println("Water level has not changed, waiting for any action...");
    }
    if (getCurrentPercentageOfWater() < percentageOfWaterCheckpoint)
    {
      actionWhen(state::levelKeepsDecreasing);
    }
    else
    {
      actionWhen(state::refillingDetected);
    }
    break;
  case state::levelKeepsDecreasing:
    while (getCurrentPercentageOfWater() <= percentageOfWaterCheckpoint)
    {
      Serial.println("Tank level keeps decreasing.");
      Serial.print("Current level is: ");
      Serial.print(getCurrentPercentageOfWater());
      Serial.println("%");
      if (getCurrentPercentageOfWater() == 0)
      {
        sendChatAlert(notificationType::emptyTank);
        actionWhen(state::emptyTank);
        return;
      }
      delay(100);
    }
    actionWhen(state::refillingDetected);
    break;
  case state::emptyTank:
    while (getCurrentPercentageOfWater() == 0)
    {
      Serial.println("Tank is empty!");
      delay(100);
    }
    actionWhen(state::refillingDetected);
    break;
  case state::refillingDetected:
    Serial.println("Refilling action detected, keep going...");
    // After this analyzeWaterLevel method will keep in idle state util something else happens.
    // It does not make senses to wait for full tank detection since there might be cases when
    // it is not possible to full fill the tank.
    break;
  case state::fullTank: // This state aims to prevent spam after getting the tank fulfilled
    while (getCurrentPercentageOfWater() == 100)
    {
      Serial.println("Tank has not been used after refilling it. Nice!");
      delay(100);
    }
    break;
  default:
    Serial.println("Unknown State, consider rebooting the system");
    break;
  }
}

void Tank::analyzeWaterLevel()
{
  if (m_lastPercentageOfWater == m_percentageAlarmTrigger)
  {
    sendChatAlert(notificationType::alertTankLevel);
    actionWhen(state::alertTankLevel);
  }
  else if (m_lastPercentageOfWater == 100)
  {
    sendChatAlert(notificationType::fullTank);
    actionWhen(state::fullTank);
  }
  else if (m_lastPercentageOfWater == 0)
  {
    sendChatAlert(notificationType::emptyTank);
    actionWhen(state::emptyTank);
  }
  //else is idle state which means no special action.
}

int Tank::convertDistanceToPercentage(const int &distance)
{
    if(distance >= m_maxTankDepth)
      return 100;   // There might be cases when the level of water overflows
                    // so it is better consider the tank as fully filled.
    if(distance <= m_minTankDepth)
      return 0;

    // Error handling for in case wrong depth values were given.
    if(m_maxTankDepth == m_minTankDepth)
      return 0;

    m_lastPercentageOfWater = static_cast<int>(round(((m_maxTankDepth - distance) / static_cast<double>((m_maxTankDepth - m_minTankDepth))) * 100.0)); // No floating point for simplicity.
    return m_lastPercentageOfWater;
}

int Tank::getCurrentDistanceMeasure()
{
  // Clears the m_sensorTriggerPin condition
  digitalWrite(m_sensorTriggerPin, LOW); //
  delay(1);

  // Sets the m_sensorTriggerPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(m_sensorTriggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(m_sensorTriggerPin, LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds
  unsigned long duration{};
  duration = pulseIn(m_sensorEchoPin, HIGH);

  // Calculating the distance
  m_lastDistanceMeasurement = (int) round(duration * 0.034 / 2); // Speed of sound wave divided by 2 (go and back)
                                                // round() to get rid of decimals so the result can be
                                                // storage as integer since it shall not be bigger than INT_MAX
  
  //TODO: Do I still need to reset the watchdog.
  //ESP.wdtFeed();                   // Somehow this method got Node MCU stuck resetting WDT so let's keep it calmed.

  return m_lastDistanceMeasurement;
}


// This is mainly used by analyzeWaterLevel method.
int Tank::getCurrentPercentageOfWater()
{
  return convertDistanceToPercentage(getCurrentDistanceMeasure());
}

void Tank::printWaterLevel()
{
  Serial.print("Distance measured by the sensor: ");
  Serial.print(getCurrentDistanceMeasure());
  Serial.println(" cm");
  Serial.print("Percentage of water in tank: ");
  Serial.print(convertDistanceToPercentage(m_lastDistanceMeasurement)); //This avoids double measurement in this method.
  Serial.println(" %");
}

void Tank::run()
{
  printWaterLevel();
  analyzeWaterLevel();
}