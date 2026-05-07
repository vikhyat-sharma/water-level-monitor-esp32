#include <Arduino.h>
#include "ConfigManager.h"
#include "Config.h"

ConfigManager configManager;

void ConfigManager::loadConfig() {
  preferences.begin("TankConfig", false);
  config.tank1Height = preferences.getFloat("t1H", TANK1_HEIGHT);
  config.tank2Height = preferences.getFloat("t2H", TANK2_HEIGHT);
  config.tank1Offset = preferences.getFloat("t1O", TANK1_OFFSET);
  config.tank2Offset = preferences.getFloat("t2O", TANK2_OFFSET);
  config.updateInterval = preferences.getInt("updInt", SENSOR_UPDATE_INTERVAL);
  config.alertThreshold = preferences.getInt("alertTh", 25);
  config.blynkInterval = preferences.getInt("blynkIntv", BLYNK_DEFAULT_INTERVAL);
  config.highFreqStart1 = preferences.getInt("hfStart1", HF_SLOT1_START);
  config.highFreqEnd1 = preferences.getInt("hfEnd1", HF_SLOT1_END);
  config.highFreqStart2 = preferences.getInt("hfStart2", HF_SLOT2_START);
  config.highFreqEnd2 = preferences.getInt("hfEnd2", HF_SLOT2_END);
  config.quietHoursStart = preferences.getInt("hqStart", QUIET_HOURS_START);
  config.quietHoursEnd = preferences.getInt("hqEnd", QUIET_HOURS_END);
  preferences.end();
}

void ConfigManager::saveConfig() {
  preferences.begin("TankConfig", false);
  preferences.putFloat("t1H", config.tank1Height);
  preferences.putFloat("t2H", config.tank2Height);
  preferences.putFloat("t1O", config.tank1Offset);
  preferences.putFloat("t2O", config.tank2Offset);
  preferences.putInt("updInt", config.updateInterval);
  preferences.putInt("alertTh", config.alertThreshold);
  preferences.putInt("blynkIntv", config.blynkInterval);
  preferences.putInt("hfStart1", config.highFreqStart1);
  preferences.putInt("hfEnd1", config.highFreqEnd1);
  preferences.putInt("hfStart2", config.highFreqStart2);
  preferences.putInt("hfEnd2", config.highFreqEnd2);
  preferences.putInt("hqStart", config.quietHoursStart);
  preferences.putInt("hqEnd", config.quietHoursEnd);
  preferences.end();
}
