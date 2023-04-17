#include "ConfigManager.h"

ConfigManager* ConfigManager::instance = nullptr;

ConfigManager::ConfigManager()
{
    LittleFS.begin();
    loadConfig();
}

ConfigManager *ConfigManager::getInstance()
{
    if (instance == nullptr)
    {
        instance = new ConfigManager();
    }
    return instance;
}

void ConfigManager::loadConfig()
{
    // Open the configuration file in read mode
    File file = LittleFS.open(CONFIG_FILE_PATH, "r");

    if (!file)
    {
        Serial.println("Config file not found. Creating new file with default values.");

        // Create a new configuration file with default values
        configObject["demoParam"] = "ok";

        // saveConfig() will create a new configuration file.
        saveConfig(); 

        return;
    }

    configObject.clear(); // Clearing in case we are reloading the config.
    DeserializationError error = deserializeJson(configObject, file);

    if (error)
    {
        Serial.println("Failed to parse config file.");
        return;
    }

    Serial.println("Configuration loaded from file successfully.");
}

String ConfigManager::getParameter(const String &paramName, const String &defaultValue="")
{
    if (configObject.containsKey(paramName))
    {
        return configObject[paramName];
    }
    else
    {
        setParameter(paramName, defaultValue);
    }
    return defaultValue;
}

void ConfigManager::setParameter(const String &paramName, const String &value)
{
    configObject[paramName] = value;
    
    // TODO: Evaluate if this is very high resource consumption.
    // Keep in mind that relying on the destructor to save the configuration
    // might result in data loss.
    saveConfig();
}

void ConfigManager::saveConfig()
{
    // Opens (creates if it does not exists) the configuration file 
    // in write mode, "w" mode overwrites the content of the file.
    File configFile = LittleFS.open(CONFIG_FILE_PATH, "w");

    if (!configFile)
    {
        Serial.println("Failed to open config file for writing.");
        return;
    }

    // Serialize the JSON object to a string and write it to the file
    // optin 1:
    /*
    String output;
    serializeJson(configObject, output);
    file.print(output);
    */
    // option 2:
    serializeJson(configObject, configFile);

    // Close the file
    configFile.close();

    Serial.println("Configuration saved.");
}

ConfigManager::~ConfigManager()
{
    saveConfig();
    delete instance;
}