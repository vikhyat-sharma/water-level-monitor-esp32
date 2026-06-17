#include <Arduino.h>
#include "OTAManager.h"
#include "Config.h"

OTAManager otaManager;

void OTAManager::begin(const char* hostname) {
  ArduinoOTA.setHostname(hostname);
  
  #ifdef OTA_PASSWORD
  ArduinoOTA.setPassword(OTA_PASSWORD);
  #endif

  ArduinoOTA.onStart([this]() {
    updating = true;
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else {
      type = "filesystem";
    }
    Serial.println("Start updating " + type);
  });

  ArduinoOTA.onEnd([this]() {
    updating = false;
    Serial.println("\nOTA Update Complete!");
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });

  ArduinoOTA.onError([this](ota_error_t error) {
    updating = false;
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });

  ArduinoOTA.begin();
  Serial.println("OTA Ready");
}

void OTAManager::handle() {
  ArduinoOTA.handle();
}

bool OTAManager::isUpdating() {
  return updating;
}
