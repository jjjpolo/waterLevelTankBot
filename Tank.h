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

    enum class stateV2
    {
        EMPTY_TANK,
        ALERT_LEVEL,
        TEST_BOT,
        FULL_TANK,
        IDLE_STATE
    };

    int m_sensorTriggerPin{};
    int m_sensorEchoPin{};
    
    // For calculating percentage of water in Tank
    //  Default values in case configManager is a brand new instance:
    const int m_defaultMaxTankDepth {100};
    const int m_defaultMinTankDepth {20};
    const int m_defaultPercentageAlarmTrigger {75};
    int m_maxTankDepth{};
    int m_minTankDepth{};
    int m_percentageAlarmTrigger{};
    int m_lastDistanceMeasurement{};
    int m_lastPercentageOfWater {};

    stateV2 m_currentState = stateV2::IDLE_STATE;
    stateV2 m_lastState = stateV2::IDLE_STATE;

    bool m_testBotConfig = false; // This can make the state machine to test bot configuration.

    // For Async Web server POST request
    uint8_t *m_asyncWebRequestPostBuffer = nullptr;
    size_t m_asyncWebRequestDataSizeFirstPart = 0;

    Bot *m_tankBot = nullptr;
    ConfigManager *m_configManager = nullptr;
    AsyncWebServer *m_TankWebServer = nullptr;

    void printWaterLevel();
    int getCurrentDistanceMeasure();
    int getCurrentPercentageOfWater();
    int getFilteredDistance();
    int getMode(int *array, int size);
    int convertDistanceToPercentage(const int &distance);
    void analyzeWaterLevel();
    void actionWhen(const state &currentSate);
    void sendChatAlert(const notificationType &currentNotification);
    void handleGetParameters(AsyncWebServerRequest *request);
    void handlePostParameters(AsyncWebServerRequest *request, uint8_t * data, size_t len);

public:
    Tank(int sensorTriggerPin, int sensorEchoPin, Bot *botReference);
    void run();
    void runV2();
    ~Tank();
};