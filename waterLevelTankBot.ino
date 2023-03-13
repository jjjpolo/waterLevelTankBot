#include "Tank.h"

// Hardware defines
#define echoPin D1  // attach pin D2 Arduino to pin Echo of JSN-SR04T
#define trigPin D2  //attach pin D3 Arduino to pin Trig of JSN-SR04T

// Software defines
// Tank level 0 (it is actually 20cm) means full tank
#define maxTankLevel 20
#define minTankLevel 100
#define triggerAlertTankLevel 30

//global variables
long currentDistance{};
enum class notificationType { emptyTank,
                              alertTankLevel,
                              fullTank };
enum class state { tankLevelIsTheSame,
                   levelKeepsDecreasing,
                   tankKeepsEmpty,
                   refillingDetected,
                   startUsingFullTank };

void setup() {
  pinMode(trigPin, OUTPUT);                          // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT);                           // Sets the echoPin as an INPUT
  ESP.wdtDisable();                                  // Disabling watchdog to prevent NodeMCU from reseting due to not visiting loop function that often.
  Serial.begin(115200);                              // Serial Communication is starting with 9600 of baud rate speed
  Serial.println("Ultrasonic Sensor HC-SR04 Test");  // print some text in Serial Monitor
  Serial.println("***************************************************************************************************************************");
  delay(2000);
}

long getCurrentDistanceMeasure() {
  long duration{};
  long distance{};

  // Clears the trigPin condition
  digitalWrite(trigPin, LOW);  //
  delay(1);

  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);

  // Calculating the distance
  distance = duration * 0.034 / 2;  // Speed of sound wave divided by 2 (go and back)
  ESP.wdtFeed();

  return distance;
}

void printDistance(const long &distance) {
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
}

void sendChatAlert(const notificationType &currentNotification) {
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

void actionWhile(const state &currentSate) {
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

void analyzeDistance(const long &distance) {
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

void loop() {
  currentDistance = getCurrentDistanceMeasure();
  printDistance(currentDistance);
  analyzeDistance(currentDistance);
  delay(250);
}