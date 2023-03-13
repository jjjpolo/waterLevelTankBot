#pragma once

#include <UniversalTelegramBot.h>

#include "Arduino.h"

class Bot
{
private:
    String m_name{};
    String m_token{};
    long m_chatID{};

public:
    Bot(const String &botName, const String &token, const long chatID);
    void sendMessage(const String &message);
};