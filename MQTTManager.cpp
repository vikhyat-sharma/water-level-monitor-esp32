#include <Arduino.h>
#include "MQTTManager.h"
#include <ArduinoJson.h>

MQTTManager mqttManager;

void MQTTManager::begin() {
  #ifdef MQTT_ENABLED
  #ifdef MQTT_SERVER
  mqttClient.setClient(wifiClient);
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  mqttClient.setKeepAlive(60);
  
  Serial.println("MQTT Manager initialized");
  #endif
  #endif
}

void MQTTManager::loop() {
  #ifdef MQTT_ENABLED
  #ifdef MQTT_SERVER
  if (!mqttClient.connected()) {
    unsigned long now = millis();
    if (now - lastReconnectAttempt > reconnectInterval) {
      lastReconnectAttempt = now;
      if (reconnect()) {
        lastReconnectAttempt = 0;
      }
    }
  } else {
    mqttClient.loop();
  }
  #endif
  #endif
}

bool MQTTManager::reconnect() {
  #ifdef MQTT_ENABLED
  #ifdef MQTT_SERVER
  String clientId = "WaterTankMonitor-";
  clientId += String(random(0xffff), HEX);
  
  #ifdef MQTT_USER
  if (mqttClient.connect(clientId.c_str(), MQTT_USER, MQTT_PASSWORD)) {
  #else
  if (mqttClient.connect(clientId.c_str())) {
  #endif
    Serial.println("MQTT Connected");
    
    // Publish online status
    String statusTopic = getBaseTopic() + "/status";
    mqttClient.publish(statusTopic.c_str(), "online", true);
    
    return true;
  } else {
    Serial.print("MQTT connection failed, rc=");
    Serial.println(mqttClient.state());
    return false;
  }
  #endif
  #endif
  return false;
}

String MQTTManager::getBaseTopic() {
  #ifdef MQTT_BASE_TOPIC
  return String(MQTT_BASE_TOPIC);
  #else
  return "homeassistant/sensor/water_tank";
  #endif
}

void MQTTManager::publishTankLevels(float tank1Percent, float tank2Percent, bool sensor1Ok, bool sensor2Ok) {
  #ifdef MQTT_ENABLED
  #ifdef MQTT_SERVER
  if (!mqttClient.connected()) return;

  DynamicJsonDocument doc(512);
  doc["tank1_percent"] = round(tank1Percent * 10) / 10.0;
  doc["tank2_percent"] = round(tank2Percent * 10) / 10.0;
  doc["tank1_sensor_ok"] = sensor1Ok;
  doc["tank2_sensor_ok"] = sensor2Ok;
  doc["timestamp"] = millis() / 1000;

  String payload;
  serializeJson(doc, payload);

  String topic = getBaseTopic() + "/levels";
  mqttClient.publish(topic.c_str(), payload.c_str());

  // Publish individual topics for Home Assistant auto-discovery
  String tank1Topic = getBaseTopic() + "/tank1/percent";
  String tank2Topic = getBaseTopic() + "/tank2/percent";
  
  mqttClient.publish(tank1Topic.c_str(), String(tank1Percent, 1).c_str());
  mqttClient.publish(tank2Topic.c_str(), String(tank2Percent, 1).c_str());
  #endif
  #endif
}

void MQTTManager::publishStatus(bool wifiConnected, const String& ipAddress, unsigned long uptime) {
  #ifdef MQTT_ENABLED
  #ifdef MQTT_SERVER
  if (!mqttClient.connected()) return;

  DynamicJsonDocument doc(512);
  doc["wifi_connected"] = wifiConnected;
  doc["ip_address"] = ipAddress;
  doc["uptime_seconds"] = uptime;
  doc["free_heap"] = ESP.getFreeHeap();

  String payload;
  serializeJson(doc, payload);

  String topic = getBaseTopic() + "/device_status";
  mqttClient.publish(topic.c_str(), payload.c_str());
  #endif
  #endif
}

bool MQTTManager::isConnected() {
  #ifdef MQTT_ENABLED
  #ifdef MQTT_SERVER
  return mqttClient.connected();
  #endif
  #endif
  return false;
}
