#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <Preferences.h>

struct TankConfig {
  float tank1Height;
  float tank2Height;
  float tank1Offset;
  float tank2Offset;
  int updateInterval;
  int alertThreshold;
  int blynkInterval;
  int highFreqStart1;
  int highFreqEnd1;
  int highFreqStart2;
  int highFreqEnd2;
  int quietHoursStart;  // e.g. 1320 (10:30 PM)
  int quietHoursEnd;    // e.g. 330  (5:30 AM)
};

class ConfigManager {
public:
  void loadConfig();
  void saveConfig();
  TankConfig config;

private:
  Preferences preferences;
};

extern ConfigManager configManager;

#endif
