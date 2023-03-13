#pragma once

#include "Arduino.h"

class Bot
{
    private:
    String m_name{};

    public:
    Bot(const String &botName);
    void sendMessage(const String &message);
};