#include <Arduino.h>
#include "SensorManager.h"

SensorManager sensorManager;

void SensorManager::setupSensors() {
  pinMode(TRIG_PIN_1, OUTPUT);
  pinMode(ECHO_PIN_1, INPUT);
  pinMode(TRIG_PIN_2, OUTPUT);
  pinMode(ECHO_PIN_2, INPUT);
}

float SensorManager::readTankLevel(int trigPin, int echoPin, float tankHeight, float offset) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Wait for echo or timeout (30 ms = max 5 meters)
  long duration = pulseIn(echoPin, HIGH, 30000);

  if (duration == 0) {
    Serial.println("Sensor error: No echo received");
    return -1;  // indicate sensor failure
  }

  float distance = duration * 0.0343 / 2;
  float distanceInInches = distance / 2.54;

  // Sanity check: distance should be within sensor's range
  if (distanceInInches < offset || distanceInInches > (tankHeight + 20)) { // 20" buffer for noise
    Serial.print("Sensor error: Unrealistic distance "); Serial.println(distanceInInches);
    return -1;
  }

  float waterLevel = (tankHeight + offset) - distanceInInches;

  // Constrain level between 0 and tankHeight
  if (waterLevel < 0) waterLevel = 0;
  if (waterLevel > tankHeight) waterLevel = tankHeight;

  return waterLevel;
}

