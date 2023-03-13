#pragma once

#include "Arduino.h"

class Tank
{
    #define maxTankLevel 20
    #define minTankLevel 100
    #define triggerAlertTankLevel 30

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

    Tank(int sensorTriggerPin, int sensorEchoPin);

    long getCurrentDistanceMeasure();

    void printDistance(const long &distance);

    void sendChatAlert(const notificationType &currentNotification);

    void actionWhile(const state &currentSate);

    void analyzeDistance(const long &distance);
};