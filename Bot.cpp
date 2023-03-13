#include "Bot.h"

Bot::Bot(const String &botName, const String &token, const long chatID) : m_name(botName),
                                                                          m_token(token),
                                                                          m_chatID(chatID)
{
    Serial.println("Bot constructor");
}

void Bot::sendMessage(const String &message)
{
    Serial.print("--------> [TELEGRAM][WIP] sending message: ");
    Serial.println(message);
    Serial.print("--------> Atte: ");
    Serial.println(m_name);
    delay(500);
}