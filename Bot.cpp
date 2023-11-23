#include "Bot.h"

Bot::Bot(const String &botName, WiFiClientSecure *wifiClient) : m_name(botName), m_wifiClient(wifiClient)
{
    Serial.println("BOT - INFO - Basic Constructor");
    m_configManager = ConfigManager::getInstance();
    m_token = m_configManager->getParameter("telegramToken", "Please set the token");
    m_chatID = m_configManager->getParameter("telegramChatID", "Please set the chat ID");
    m_bot = new UniversalTelegramBot(m_token, *m_wifiClient);
    m_wifiClient->setInsecure(); // wifiClient needs to be insecure to allow sending telegram msgs.
}

Bot::Bot(const String &botName, WiFiClientSecure *wifiClient, const String &telegramToken, const String &telegramChatID) : m_name(botName), m_wifiClient(wifiClient)
{
    Serial.println("BOT - INFO - Constructor with token and chat ID");
    m_configManager = ConfigManager::getInstance();
    m_bot = new UniversalTelegramBot(m_token, *m_wifiClient);
    setToken(telegramToken);
    setChatID(telegramChatID);
    m_wifiClient->setInsecure(); // wifiClient needs to be insecure to allow sending telegram msgs.
}

Bot::~Bot()
{
    delete m_bot;
}

void Bot::sendMessage(const String &message)
{
    Serial.println("--------> [TELEGRAM] sending message: ");
    Serial.println(message);
    Serial.println("BOT - INFO - Using token: " + m_token);
    Serial.println("BOT - INFO - Using chat ID: " + m_chatID);
    m_mutex.lock();
    if (m_bot->sendMessage(m_chatID, message ))
    {
        Serial.println("BOT - INFO  - Message sent successfully");
    } 
    else 
    {
        Serial.println("BOT - ERROR - Error sending message, check that token and chat ID are valid");
    }
    m_mutex.unlock();
    Serial.print("--------> [TELEGRAM] Att: ");
    Serial.println(m_name);
}

void Bot::setToken(const String &newToken) 
{
    //m_mutex.lock();
    m_token = newToken;
    m_bot->updateToken(m_token);
    //m_mutex.unlock();
    Serial.printf("BOT - INFO - Replacing telegram token: %s with: %s \n", (m_configManager->getParameter("telegramToken", "Error")).c_str(), newToken.c_str());
    m_configManager->setParameter("telegramToken", newToken);
    Serial.printf("BOT - INFO - Setting new telegram token %s \n", (m_configManager->getParameter("telegramToken", "Error")).c_str());
}

void Bot::setChatID(const String &newChatID)
{
    //m_mutex.lock();
    m_chatID = newChatID;
    //m_mutex.unlock();
    m_configManager->setParameter("telegramChatID", newChatID);
    Serial.printf("BOT - INFO - Setting new telegram chat ID %s \n", (m_configManager->getParameter("telegramChatID", "Error")).c_str());
}