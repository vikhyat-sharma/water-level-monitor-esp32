#ifndef ALERTMANAGER_H
#define ALERTMANAGER_H

#include <Arduino.h>

enum AlertType {
  ALERT_NONE,
  ALERT_LOW_LEVEL,
  ALERT_HIGH_LEVEL,
  ALERT_SENSOR_ERROR,
  ALERT_CRITICAL_LOW
};

struct AlertConfig {
  float lowThreshold = 20.0;      // Alert when below 20%
  float criticalLowThreshold = 10.0;  // Critical alert below 10%
  float highThreshold = 95.0;     // Alert when above 95%
  float hysteresis = 5.0;         // 5% hysteresis to prevent alert flapping
  unsigned long minAlertInterval = 300000;  // 5 minutes between same alerts
};

class AlertManager {
public:
  void begin();
  void checkAlerts(float tank1Percent, float tank2Percent, bool sensor1Ok, bool sensor2Ok);
  String getAlertMessage(AlertType type, int tankNumber, float percent);
  void setAlertConfig(AlertConfig newConfig);
  AlertConfig getAlertConfig();

private:
  AlertConfig config;
  
  // Alert state tracking
  bool tank1LowAlertActive = false;
  bool tank2LowAlertActive = false;
  bool tank1HighAlertActive = false;
  bool tank2HighAlertActive = false;
  bool tank1CriticalAlertActive = false;
  bool tank2CriticalAlertActive = false;
  
  unsigned long lastTank1LowAlert = 0;
  unsigned long lastTank2LowAlert = 0;
  unsigned long lastTank1HighAlert = 0;
  unsigned long lastTank2HighAlert = 0;
  unsigned long lastTank1CriticalAlert = 0;
  unsigned long lastTank2CriticalAlert = 0;
  unsigned long lastSensor1ErrorAlert = 0;
  unsigned long lastSensor2ErrorAlert = 0;
  
  void checkTankAlerts(int tankNum, float percent, bool sensorOk);
  bool shouldSendAlert(unsigned long lastAlertTime);
};

extern AlertManager alertManager;

#endif
