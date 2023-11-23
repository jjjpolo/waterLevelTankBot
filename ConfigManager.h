/*
For documentation:

Usage example:
    int testCounter = configManager->getParameter("testCounter", "0").toInt();
  Serial.println("Counter had value: " + String(testCounter));
  testCounter++;
  configManager->setParameter("testCounter", String(testCounter));

*/

#pragma once

#include <ArduinoJson.h>
#include "LittleFS.h"

class ConfigManager
{
public:
    ConfigManager();

    //No copying. 
    ConfigManager(const ConfigManager &) = delete; // no copying
    ConfigManager &operator=(const ConfigManager &) = delete;

    static ConfigManager *getInstance();

    // Business logic
    void setParameter(const String &paramName, const String &value);
    String getParameter(const String &paramName, const String &defaultValue);
    String getRawJsonContent();
    void saveConfig();
    void loadConfig();
    void eraseFlashMemory();
    ~ConfigManager();
    
private:
    static ConfigManager* instance;
    StaticJsonDocument<380> configObject; //Set this to at least twice the size of the json document you will be using so that updating values gives no problem.
    const String CONFIG_FILE_PATH = "config.json";
};