#include "Bot.h"

Bot::Bot(const String &botName):
m_name(botName)
{
    Serial.println("Bot constructor");
}

void Bot::sendMessage(const String& message)
{
    Serial.print("--------> [TELEGRAM][WIP] sending message: ");
    Serial.println(message);
    Serial.print("--------> Atte: "); 
    Serial.println(m_name);
    delay(500);
}