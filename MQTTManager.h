#ifndef MQTTMANAGER_H
#define MQTTMANAGER_H

#include <PubSubClient.h>
#include <WiFi.h>
#include "Config.h"

class MQTTManager {
public:
  void begin();
  void loop();
  void publishTankLevels(float tank1Percent, float tank2Percent, bool sensor1Ok, bool sensor2Ok);
  void publishStatus(bool wifiConnected, const String& ipAddress, unsigned long uptime);
  bool isConnected();

private:
  WiFiClient wifiClient;
  PubSubClient mqttClient;
  unsigned long lastReconnectAttempt = 0;
  const unsigned long reconnectInterval = 5000;
  
  bool reconnect();
  String getBaseTopic();
};

extern MQTTManager mqttManager;

#endif
