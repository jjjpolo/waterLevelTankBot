#pragma once

#include "Arduino.h"
#include "Bot.h"

class Tank
{
private:
    enum class notificationType { emptyTank,
                                alertTankLevel,
                                fullTank };
    enum class state { tankLevelIsTheSame,
                    levelKeepsDecreasing,
                    tankKeepsEmpty,
                    refillingDetected,
                    startUsingFullTank };

    int m_sensorTriggerPin{};
    int m_sensorEchoPin{};
    long m_maxTankLevel {};
    long m_minTankLevel {};
    long m_triggerAlertTankLevel {};
    long m_lastDistanceMeasurement{};
    Bot *m_tankBot{};

    void sendChatAlert(const notificationType &currentNotification);
    void actionWhile(const state &currentSate);
    void analyzeLastDistance();
    
public:
    Tank(int sensorTriggerPin, int sensorEchoPin, long maxTankLevel, long minTankLevel, long triggerAlertTankLevel, Bot* tankBot);
    void smartJobRoutine();
    long getCurrentDistanceMeasure();
    void printCurrentDistance();
};