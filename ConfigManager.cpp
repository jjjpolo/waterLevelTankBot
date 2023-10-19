#include "ConfigManager.h"

ConfigManager* ConfigManager::instance = nullptr;

ConfigManager::ConfigManager()
{
    Serial.println("ConfigManager::ConfigManager.");
    LittleFS.begin();
    loadConfig();
}

ConfigManager *ConfigManager::getInstance()
{
    Serial.println("ConfigManager::getInstance.");
    if (instance == nullptr)
    {
        instance = new ConfigManager();
        Serial.println("ConfigManager::getInstance - Creating new instance.");
    }
    return instance;
}

String ConfigManager::getRawJsonContent()
{
    String rawContent;
    
    serializeJson(configObject, rawContent);

    return (rawContent+"\n");
}

void ConfigManager::loadConfig()
{
    // Open the configuration file in read mode
    File file = LittleFS.open(CONFIG_FILE_PATH, "r");

    if (!file)
    {
        Serial.println("ConfigManager::loadConfig Config file not found. Creating new file with default values.");

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
        Serial.println("ConfigManager::loadConfig Failed to parse config file.");
        return;
    }

    Serial.println("ConfigManager::loadConfig Configuration loaded from file successfully.");
}

String ConfigManager::getParameter(const String &paramName, const String &defaultValue="")
{
    Serial.printf("ConfigManager::getParameter %s \n", paramName.c_str());
    if (configObject.containsKey(paramName))
    {
        return configObject[paramName];
    }
    else
    {
        Serial.println("ConfigManager::getParameter Parameter not found, had to create it.");
        setParameter(paramName, defaultValue);
    }
    return defaultValue;
}

void ConfigManager::setParameter(const String &paramName, const String &value)
{
    configObject[paramName] = value;
    String newValue = configObject[paramName].as<String>();

    Serial.printf("ConfigManager::setParameter: %s to %s \n", paramName.c_str(), value.c_str());
    Serial.printf("ConfigManager::setParameter: New value stored for parameter in json obj: %s \n", newValue.c_str());

    // TODO: Evaluate if this is very high resource consumption.
    // Keep in mind that relying on the destructor to save the configuration
    // might result in data loss.
    saveConfig();
}

void ConfigManager::saveConfig()
{
    Serial.printf("ConfigManager::saveConfig Raw json output before saving to file: %s", getRawJsonContent().c_str());
    // Opens (creates if it does not exists) the configuration file 
    // in write mode, "w" mode overwrites the content of the file.
    File configFile = LittleFS.open(CONFIG_FILE_PATH, "w");

    if (!configFile)
    {
        Serial.println("ConfigManager::saveConfig Failed to open config file for writing.");
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
    Serial.println("ConfigManager::saveConfig Configuration saved.");
}

ConfigManager::~ConfigManager()
{
    saveConfig();
    delete instance;
}