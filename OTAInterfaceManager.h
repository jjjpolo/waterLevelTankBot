#pragma once
#include <ArduinoOTA.h>
#include <Arduino.h>

class OTAInterfaceManager
{
public:
    OTAInterfaceManager();
    OTAInterfaceManager(const String &hostname, const String &password);
    OTAInterfaceManager(const String &hostname, const String &password, const int &port);
    void run();    
};