#pragma once

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>


class Bot
{
private:
    String m_name{};
    String m_token{};
    String m_chatID{};
    UniversalTelegramBot *m_bot;
    WiFiClientSecure *m_wifiClient;

public:
    Bot(const String &botName, const String &token, const String &chatID, WiFiClientSecure *wifiClient);
    void sendMessage(const String &message);
    ~Bot();
};