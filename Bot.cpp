#include "Bot.h"

Bot::Bot(const String &botName, const String &token, const String &chatID, WiFiClientSecure *wifiClient) : m_name(botName),
                                                                                                        m_token(token),
                                                                                                        m_chatID(chatID),
                                                                                                        m_wifiClient(wifiClient)
{
    Serial.println("Bot constructor");
    m_bot = new UniversalTelegramBot(m_token, *m_wifiClient);
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
    m_bot->sendMessage(m_chatID, message );
    Serial.print("--------> [TELEGRAM] Att: ");
    Serial.println(m_name);
}