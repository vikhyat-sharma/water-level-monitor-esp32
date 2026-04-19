#ifndef SENSORMANAGER_H
#define SENSORMANAGER_H

#include "Config.h"

class SensorManager {
public:
  void setupSensors();
  float readTankLevel(int trigPin, int echoPin, float tankHeight, float offset);
};

extern SensorManager sensorManager;

#endif
