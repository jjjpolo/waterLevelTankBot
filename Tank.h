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
        tankLevelIsTheSame,
        levelKeepsDecreasing,
        tankKeepsEmpty,
        refillingDetected,
        startUsingFullTank
    };

    int m_sensorTriggerPin{};
    int m_sensorEchoPin{};
    
    // For calculating percentage of water in Tank
    int m_maxTankDepth{};
    int m_minTankDepth{};
    float m_lastPercentageOfWaterInTank {};

    long m_maxTankLevel{};
    long m_minTankLevel{};
    long m_triggerAlertTankLevel{};
    Bot *m_tankBot{};
    AsyncWebServer *m_TankWebServer;

    void sendChatAlert(const notificationType &currentNotification);
    void actionWhile(const state &currentSate);
    void analyzeLastDistance();

public:
    long m_lastDistanceMeasurement;

    Tank(int sensorTriggerPin, int sensorEchoPin, long maxTankLevel, long minTankLevel, long triggerAlertTankLevel, Bot *botReference, AsyncWebServer *serverReference);
    void smartJobRoutine();
    long getCurrentDistanceMeasure();
    void printCurrentDistance();
    String getLastMeasure();
    float convertDistanceToPercentage(const int &distance);
};