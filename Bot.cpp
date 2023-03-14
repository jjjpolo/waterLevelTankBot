#include "Bot.h"

Bot::Bot(const String &botName, const String &token, const long chatID, WiFiClientSecure *wifiClient) : m_name(botName),
                                                                                                        m_token(token),
                                                                                                        m_chatID(chatID),
                                                                                                        m_wifiClient(wifiClient)
{
    Serial.println("Bot constructor");
    m_bot = new UniversalTelegramBot(m_token, *m_wifiClient);
}

Bot::~Bot()
{
    // delete m_bot;
}

void Bot::sendMessage(const String &message)
{
    Serial.print("--------> [TELEGRAM][WIP] sending message: ");
    Serial.println(message);
    Serial.print("--------> Att: ");
    Serial.println(m_name);
    // m_bot->sendMessage(m_chatID, message );
    delay(500);
}