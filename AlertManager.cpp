#include "AlertManager.h"
#include "TelegramManager.h"

AlertManager alertManager;

void AlertManager::begin() {
  Serial.println("Alert Manager initialized");
}

void AlertManager::setAlertConfig(AlertConfig newConfig) {
  config = newConfig;
}

AlertConfig AlertManager::getAlertConfig() {
  return config;
}

bool AlertManager::shouldSendAlert(unsigned long lastAlertTime) {
  return (millis() - lastAlertTime) >= config.minAlertInterval;
}

String AlertManager::getAlertMessage(AlertType type, int tankNumber, float percent) {
  String tankName = (tankNumber == 1) ? "Badi Tanki" : "Choti Tanki";
  
  switch (type) {
    case ALERT_LOW_LEVEL:
      return "⚠️ LOW WATER ALERT\n" + tankName + ": " + String(percent, 1) + "%\nPlease refill soon.";
    
    case ALERT_CRITICAL_LOW:
      return "🚨 CRITICAL LOW WATER!\n" + tankName + ": " + String(percent, 1) + "%\nImmediate attention required!";
    
    case ALERT_HIGH_LEVEL:
      return "✅ TANK FULL\n" + tankName + ": " + String(percent, 1) + "%\nConsider turning off pump.";
    
    case ALERT_SENSOR_ERROR:
      return "❌ SENSOR ERROR\n" + tankName + " sensor not responding.\nCheck connections.";
    
    default:
      return "";
  }
}

void AlertManager::checkTankAlerts(int tankNum, float percent, bool sensorOk) {
  bool& lowAlertActive = (tankNum == 1) ? tank1LowAlertActive : tank2LowAlertActive;
  bool& highAlertActive = (tankNum == 1) ? tank1HighAlertActive : tank2HighAlertActive;
  bool& criticalAlertActive = (tankNum == 1) ? tank1CriticalAlertActive : tank2CriticalAlertActive;
  
  unsigned long& lastLowAlert = (tankNum == 1) ? lastTank1LowAlert : lastTank2LowAlert;
  unsigned long& lastHighAlert = (tankNum == 1) ? lastTank1HighAlert : lastTank2HighAlert;
  unsigned long& lastCriticalAlert = (tankNum == 1) ? lastTank1CriticalAlert : lastTank2CriticalAlert;
  unsigned long& lastSensorError = (tankNum == 1) ? lastSensor1ErrorAlert : lastSensor2ErrorAlert;
  
  // Check sensor error
  if (!sensorOk) {
    if (shouldSendAlert(lastSensorError)) {
      String message = getAlertMessage(ALERT_SENSOR_ERROR, tankNum, 0);
      telegramManager.sendMessage(message);
      telegramManager.sendToApproved(message);
      lastSensorError = millis();
    }
    return;
  }
  
  // Check critical low level
  if (percent <= config.criticalLowThreshold) {
    if (!criticalAlertActive && shouldSendAlert(lastCriticalAlert)) {
      String message = getAlertMessage(ALERT_CRITICAL_LOW, tankNum, percent);
      telegramManager.sendMessage(message);
      telegramManager.sendToApproved(message);
      lastCriticalAlert = millis();
      criticalAlertActive = true;
    }
  } else if (percent > config.criticalLowThreshold + config.hysteresis) {
    criticalAlertActive = false;
  }
  
  // Check low level
  if (percent <= config.lowThreshold && percent > config.criticalLowThreshold) {
    if (!lowAlertActive && shouldSendAlert(lastLowAlert)) {
      String message = getAlertMessage(ALERT_LOW_LEVEL, tankNum, percent);
      telegramManager.sendMessage(message);
      telegramManager.sendToApproved(message);
      lastLowAlert = millis();
      lowAlertActive = true;
    }
  } else if (percent > config.lowThreshold + config.hysteresis) {
    lowAlertActive = false;
  }
  
  // Check high level
  if (percent >= config.highThreshold) {
    if (!highAlertActive && shouldSendAlert(lastHighAlert)) {
      String message = getAlertMessage(ALERT_HIGH_LEVEL, tankNum, percent);
      telegramManager.sendMessage(message);
      telegramManager.sendToApproved(message);
      lastHighAlert = millis();
      highAlertActive = true;
    }
  } else if (percent < config.highThreshold - config.hysteresis) {
    highAlertActive = false;
  }
}

void AlertManager::checkAlerts(float tank1Percent, float tank2Percent, bool sensor1Ok, bool sensor2Ok) {
  checkTankAlerts(1, tank1Percent, sensor1Ok);
  checkTankAlerts(2, tank2Percent, sensor2Ok);
}
