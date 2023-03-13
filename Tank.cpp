#include "Tank.h"

Tank::Tank(int sensorTriggerPin, int sensorEchoPin):
m_sensorTriggerPin(sensorTriggerPin),
m_sensorEchoPin(sensorEchoPin)
{
    Serial.println("Tank constructor");
    pinMode(m_sensorTriggerPin, OUTPUT);                          // Sets the trigPin as an OUTPUT
    pinMode(m_sensorEchoPin, INPUT); 
}

long Tank::getCurrentDistanceMeasure() {
  long duration{};
  long distance{};

  // Clears the m_sensorTriggerPin condition
  digitalWrite(m_sensorTriggerPin, LOW);  //
  delay(1);

  // Sets the m_sensorTriggerPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(m_sensorTriggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(m_sensorTriggerPin, LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(m_sensorEchoPin, HIGH);

  // Calculating the distance
  distance = duration * 0.034 / 2;  // Speed of sound wave divided by 2 (go and back)
  ESP.wdtFeed();

  return distance;
}

void Tank::printDistance(const long &distance) {
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
}

void Tank::sendChatAlert(const notificationType &currentNotification) {
  switch (currentNotification) {
    case notificationType::emptyTank:
      Serial.println("[TELEGRAM][WIP] Sending chat alert: emptyTank");
      break;
    case notificationType::alertTankLevel:
      Serial.println("[TELEGRAM][WIP] Sending chat alert: alertTankLevel");
      break;
    case notificationType::fullTank:
      Serial.println("[TELEGRAM][WIP] Sending chat alert: fullTank");
      break;
    default:
      Serial.println("[TELEGRAM][WIP] Sending chat alert: UNKNOWN ");
      break;
  }
}

void Tank::actionWhile(const state &currentSate) {
  long currentLevel = getCurrentDistanceMeasure();
  switch (currentSate) {
    case state::tankLevelIsTheSame:
      while (currentLevel == getCurrentDistanceMeasure()) {
        delay(250);
        Serial.println("[WIP] Waiting for any action...");
      }
      if (getCurrentDistanceMeasure() > currentLevel) {
        actionWhile(state::levelKeepsDecreasing);
      } else {
        actionWhile(state::refillingDetected);
      }
      break;
    case state::levelKeepsDecreasing:
      while (getCurrentDistanceMeasure() >= currentLevel) {
        Serial.println("[WIP] Tank level keeps decreasing or hasn't reached alertTankLevel");
        Serial.print("[WIP] Current level is: ");
        Serial.println(getCurrentDistanceMeasure());
        if (getCurrentDistanceMeasure() == long(minTankLevel)) {
          sendChatAlert(notificationType::emptyTank);
          actionWhile(state::tankKeepsEmpty);
          return;
        }
        delay(250);
      }
      actionWhile(state::refillingDetected);
      break;
    case state::tankKeepsEmpty:
      while (getCurrentDistanceMeasure() == minTankLevel) {
        Serial.println("[WIP] Tank is empty!");
        delay(250);
      }
      actionWhile(state::refillingDetected);
      break;
    case state::refillingDetected:
      Serial.println("[WIP] Refilling action detected, keep going...");
      break;
    case state::startUsingFullTank:
      //This state aims to prevent spam after getting the tank fulfilled
      while (getCurrentDistanceMeasure() <= long(maxTankLevel)) {
        Serial.println("Tank has not been used after refilling it. Nice!");
        delay(250);
      }
      break;
    default:
      Serial.println("[WIP] Unknown State");
      break;
  }
}

void Tank::analyzeDistance(const long &distance) {
  if (distance == long(triggerAlertTankLevel)) {
    sendChatAlert(notificationType::alertTankLevel);
    actionWhile(state::tankLevelIsTheSame);
  } else if (distance == long(maxTankLevel)) {
    sendChatAlert(notificationType::fullTank);
    actionWhile(state::startUsingFullTank);
  } else if (distance == long(minTankLevel)) {
    sendChatAlert(notificationType::emptyTank);
    actionWhile(state::tankKeepsEmpty);
  }
}