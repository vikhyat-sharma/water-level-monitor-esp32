#include <BlynkSimpleEsp32.h>
#include <Arduino.h>
#include <time.h>
#include "BlynkManager.h"
#include "WiFiManager.h"

BlynkManager blynkManager;

void BlynkManager::begin() {
  if (wifiManager.isConnected()) {
    Blynk.config(BLYNK_AUTH_TOKEN);
    Blynk.connect();
  }
}

void BlynkManager::run() {
  if (WiFi.status() == WL_CONNECTED) {
    Blynk.run();
  }
}

void BlynkManager::sendTankLevels(float level1, float level2) {
  if (WiFi.status() == WL_CONNECTED) {
    Blynk.virtualWrite(V0, level1);
    Blynk.virtualWrite(V1, level2);
  }
}

void BlynkManager::updateBlynkInterval() {
  int minutesNow = -1;
  struct tm timeinfo;

  if (getLocalTime(&timeinfo)) {
    minutesNow = timeinfo.tm_hour * 60 + timeinfo.tm_min;
  }

  int intervalSeconds = configManager.config.blynkInterval;
  if (minutesNow >= 0 &&
      ((minutesNow >= configManager.config.highFreqStart1 && minutesNow <= configManager.config.highFreqEnd1) ||
       (minutesNow >= configManager.config.highFreqStart2 && minutesNow <= configManager.config.highFreqEnd2))) {
    intervalSeconds = 120;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Blynk.setProperty(V2, "interval", intervalSeconds * 1000);
  }
}
