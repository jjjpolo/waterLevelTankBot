#pragma once

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include "Mutex.h"
#include "ConfigManager.h"

class Bot
{
private:
    String m_name{};
    String m_token{};
    String m_chatID{};
    UniversalTelegramBot *m_bot;
    WiFiClientSecure *m_wifiClient;
    Mutex m_mutex;
    ConfigManager *m_configManager = nullptr;

public:
    Bot(const String &botName, WiFiClientSecure *wifiClient);
    Bot(const String &botName, WiFiClientSecure *wifiClient, const String &telegramToken, const String &telegramChatID);
    void sendMessage(const String &message);
    void setToken(const String &newToken);
    void setChatID(const String &newChatID);
    ~Bot();
};