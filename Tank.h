#pragma once

#include <Arduino.h>

#include "Bot.h"
#include "WebServerContent.h"
#include "ConfigManager.h"

// Libs for async web server
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebSrv.h>
#include <ArduinoJson.h>

class Tank
{
private:
    enum class notificationType
    {
        emptyTank,
        alertTankLevel,
        fullTank
    };
    enum class state
    {
        alertTankLevel,
        levelKeepsDecreasing,
        emptyTank,
        refillingDetected,
        fullTank
    };

    int m_sensorTriggerPin{};
    int m_sensorEchoPin{};
    
    // For calculating percentage of water in Tank
    //  Default values in case configManager is a brand new instance:
    const int m_defaultMaxTankDepth {100};
    const int m_defaultMinTankDepth {20};
    const int m_defaultPercentageAlarmTrigger {77};
    int m_maxTankDepth{};
    int m_minTankDepth{};
    int m_percentageAlarmTrigger{};
    int m_lastDistanceMeasurement{};
    int m_lastPercentageOfWater {};

    // For Async Web server POST request
    uint8_t *m_asyncWebRequestPostBuffer = nullptr;
    size_t m_asyncWebRequestDataSizeFirstPart = 0;

    Bot *m_tankBot = nullptr;
    AsyncWebServer *m_TankWebServer = nullptr;
    ConfigManager *m_configManager = nullptr;

    void printWaterLevel();
    int getCurrentDistanceMeasure();
    int getCurrentPercentageOfWater();
    int convertDistanceToPercentage(const int &distance);
    void analyzeWaterLevel();
    void actionWhen(const state &currentSate);
    void sendChatAlert(const notificationType &currentNotification);
    void handleGetParameters(AsyncWebServerRequest *request);
    void handlePostParameters(AsyncWebServerRequest *request, uint8_t * data, size_t len);

public:
    Tank(int sensorTriggerPin, int sensorEchoPin, Bot *botReference, AsyncWebServer *serverReference);
    void run();
};