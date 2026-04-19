#ifndef BLYNKMANAGER_H
#define BLYNKMANAGER_H

#include "Config.h"
#include "ConfigManager.h"

class BlynkManager {
public:
  void begin();
  void run();
  void sendTankLevels(float level1, float level2);
  void updateBlynkInterval();
};

extern BlynkManager blynkManager;

#endif
