#pragma once

#include "Arduino.h"
#include "Bot.h"
#include "WebServerContent.h"

// Libs for async web server
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebSrv.h>

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
    int m_maxTankDepth{};
    int m_minTankDepth{};
    int m_percentageAlarmTrigger{};
    int m_lastDistanceMeasurement{};
    int m_lastPercentageOfWater {};

    Bot *m_tankBot{};
    AsyncWebServer *m_TankWebServer;

    void printWaterLevel();
    int getCurrentDistanceMeasure();
    int getCurrentPercentageOfWater();
    int convertDistanceToPercentage(const int &distance);
    void analyzeWaterLevel();
    void actionWhen(const state &currentSate);
    void sendChatAlert(const notificationType &currentNotification);

public:
    Tank(int sensorTriggerPin, int sensorEchoPin, int maxTankDepth, int minTankDepth, int percentageAlarmTrigger, Bot *botReference, AsyncWebServer *serverReference);
    void run();
};