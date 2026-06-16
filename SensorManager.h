#ifndef SENSORMANAGER_H
#define SENSORMANAGER_H

#include "Config.h"

class SensorManager {
public:
  void setupSensors();
  float readTankLevel(int trigPin, int echoPin, float tankHeight, float offset);
  float readTankLevelAverage(int trigPin, int echoPin, float tankHeight, float offset, int samples = 3);

private:
  float readDistance(int trigPin, int echoPin);
};

extern SensorManager sensorManager;

#endif
