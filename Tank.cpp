#include "Tank.h"
Tank::Tank(int sensorTriggerPin, int sensorEchoPin, Bot *botReference, AsyncWebServer *serverReference) : m_sensorTriggerPin(sensorTriggerPin),
                                                                                                          m_sensorEchoPin(sensorEchoPin),
                                                                                                          m_tankBot(botReference),
                                                                                                          m_TankWebServer(serverReference)
{
  pinMode(m_sensorTriggerPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(m_sensorEchoPin, INPUT);

  m_configManager = ConfigManager::getInstance();
  m_maxTankDepth = m_configManager->getParameter("tank_maxTankDepth", String(m_defaultMaxTankDepth)).toInt();
  m_minTankDepth = m_configManager->getParameter("tank_minTankDepth", String(m_defaultMinTankDepth)).toInt();
  m_percentageAlarmTrigger = m_configManager->getParameter("tank_triggerAlarm", String(m_defaultPercentageAlarmTrigger)).toInt();

  m_TankWebServer->on("/", HTTP_GET, [&](AsyncWebServerRequest *request)
                      { request->send_P(200, "text/html", WebServerContent::index_html); });

  m_TankWebServer->on("/settings", HTTP_GET, [&](AsyncWebServerRequest *request)
                      { request->send_P(200, "text/html", WebServerContent::settings_html); });

  m_TankWebServer->on("/level", HTTP_GET, [&](AsyncWebServerRequest *request)
                      { request->send_P(200, "text/plain", String(m_lastPercentageOfWater).c_str()); });

  m_TankWebServer->on("/restart", HTTP_GET, [&](AsyncWebServerRequest *request)
                      {
    request->send_P(200, "text/plain", String("Restart").c_str());
    ESP.restart(); });

  m_TankWebServer->on("/getParameters", HTTP_GET, [&](AsyncWebServerRequest *request)
                      { handleGetParameters(request); });

  m_TankWebServer->on(
      "/setParameters", HTTP_POST,
      [&](AsyncWebServerRequest *request)
      {
        Serial.println("1.- Callback when no body detected");
      },
      [&](AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final)
      {
        Serial.println("2.- Call back when file to be uploaded is included");
      },
      [&](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
      {
        // TODO: Check/improve this code.
        Serial.println("3.- Callback when body detected");
        Serial.println(String("Request data: ") + (char *)data);
        Serial.println(String("Request len: ") + String(len));
        Serial.println(String("Request index: ") + String(index));
        Serial.println(String("Request total: ") + String(total));
        if (len == total)
        {
          Serial.println("Data does not seem to be splitted, processing it as it is.");
          handlePostParameters(request, data, len);
        }
        else
        {
          if ((len + index) == total)
          {
            Serial.println("Data would need post processing, this is the 2nd part: ");
            memcpy(m_asyncWebRequestPostBuffer + m_asyncWebRequestDataSizeFirstPart, data, len);
            handlePostParameters(request, m_asyncWebRequestPostBuffer, m_asyncWebRequestDataSizeFirstPart + len);
            delete[] m_asyncWebRequestPostBuffer;
            m_asyncWebRequestPostBuffer = nullptr; // This prevents undefined behaviour in case deleting twice.
          }
          else
          {
            Serial.println("Data would need more processing, this is the 1st part: ");
            m_asyncWebRequestPostBuffer = new uint8_t[total];
            m_asyncWebRequestDataSizeFirstPart = len;
            memcpy(m_asyncWebRequestPostBuffer, data, len);
          }
        }
      });

  m_TankWebServer->begin();
}

void Tank::handlePostParameters(AsyncWebServerRequest *request, uint8_t *data, size_t len)
{
  Serial.println("handlePostParameters");
  if (request->contentType() != "application/json")
  {
    request->send(400, "text/plain", "Invalid content type");
    return;
  }
  Serial.printf("The received data is json with len: %d \n", len);

  char *charData = new char[len + 1]; // allocate a buffer for the converted data
  memcpy(charData, data, len);        // copy the data to the buffer
  charData[len] = '\0';               // add a null terminator to the end
  Serial.printf("after memcpy: %s \n", charData);
  DynamicJsonDocument json_obj(250); // Size calculated with https://arduinojson.org/v6/assistant/#/step1
  // Deserialize the json object
  // Why are we casting to const char*? https://arduinojson.org/v6/issues/garbage-out/
  DeserializationError error = deserializeJson(json_obj, (const char *)charData); // deserialize the JSON
  delete[] charData;                                                              // free the buffer

  if (error)
  {
    String errorMessage = "Invalid JSON. Deserialization error: " + String(error.c_str());
    Serial.println(errorMessage);
    request->send(400, "text/plain", errorMessage);
    return;
  }

  m_maxTankDepth = json_obj["maxDepth"].as<int>();
  m_minTankDepth = json_obj["minDepth"].as<int>();
  m_percentageAlarmTrigger = json_obj["alarmTrigger"].as<int>();

  // Store new values in the config manager.
  m_configManager->setParameter(String("tank_maxTankDepth"), String(m_maxTankDepth));
  m_configManager->setParameter(String("tank_minTankDepth"), String(m_minTankDepth));
  m_configManager->setParameter(String("tank_triggerAlarm"), String(m_percentageAlarmTrigger));

  if (json_obj.containsKey("telegramToken"))
  {
    String newBotTokenFromJSON = json_obj["telegramToken"].as<String>();
    Serial.printf("New telegram token received: %s \n", newBotTokenFromJSON.c_str());
    m_tankBot->setToken(newBotTokenFromJSON);
  }

  if (json_obj.containsKey("telegramChatID"))
  {
    // Serial.printf("New telegram ChatID received: %s", json_obj["telegramChatID"].as<String>()); 
    //This is wrong due to the data type of the origin (charData)
    //  I'll keep this for future reference.
    //Serial.printf("New telegram ChatID received: %s \n", json_obj["telegramChatID"].as<const char *>());
    String newChatIdFromJSON = json_obj["telegramChatID"].as<String>();
    Serial.printf("New telegram ChatID received: %s \n", newChatIdFromJSON.c_str());
    m_tankBot->setChatID(newChatIdFromJSON);
  }

  // Serial.print("Config manager raw json contains: ");
  // Serial.println(m_configManager->getRawJsonContent());

  handleGetParameters(request); // Thi is just an echo response.
  //m_tankBot->sendMessage("Testing new config");
}

void Tank::handleGetParameters(AsyncWebServerRequest *request)
{
  DynamicJsonDocument response(256);
  response["status"] = "ok";
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
    while (getCurrentPercentageOfWater() == percentageOfWaterCheckpoint) // Has not changed
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
  // else is idle state which means no special action.
}

int Tank::convertDistanceToPercentage(const int &distance)
{
  if (distance >= m_maxTankDepth)
    return 100; // There might be cases when the level of water overflows
                // so it is better consider the tank as fully filled.
  if (distance <= m_minTankDepth)
    return 0;

  // Error handling for in case wrong depth values were given.
  if (m_maxTankDepth == m_minTankDepth)
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
  m_lastDistanceMeasurement = (int)round(duration * 0.034 / 2); // Speed of sound wave divided by 2 (go and back)
                                                                // round() to get rid of decimals so the result can be
                                                                // storage as integer since it shall not be bigger than INT_MAX

  // TODO: Do I still need to reset the watchdog.
  // ESP.wdtFeed();                   // Somehow this method got Node MCU stuck resetting WDT so let's keep it calmed.

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
  Serial.print(convertDistanceToPercentage(m_lastDistanceMeasurement)); // This avoids double measurement in this method.
  Serial.println(" %");
}

void Tank::run()
{
  printWaterLevel();
  analyzeWaterLevel();
}