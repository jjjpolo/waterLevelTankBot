#pragma once

#include <ArduinoJson.h>
#include <FS.h>

class ConfigManager
{
public:
    ConfigManager();
    static ConfigManager* getInstance();
    void setParameter(const String &paramName, const String &value);
    String getParameter(const String &paramName, const String &defaultValue);
    void saveConfig();
    void loadConfig();
    ~ConfigManager();
private:
    static ConfigManager* instance;
    static StaticJsonDocument<256> configObject;
    const String CONFIG_FILE_PATH = "config.json";
};