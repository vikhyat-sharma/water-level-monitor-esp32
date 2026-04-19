#define BLYNK_TEMPLATE_ID "YOUR_BLYNK_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "Water Level Monitor"
#include <BlynkSimpleEsp32.h>
#include <Arduino.h>
#include <TimeLib.h>
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
  int minutesNow = (hour() * 60) + minute();
  int interval = 150000;

  if ((minutesNow >= configManager.config.highFreqStart1 && minutesNow <= configManager.config.highFreqEnd1)
      || (minutesNow >= configManager.config.highFreqStart2 && minutesNow <= configManager.config.highFreqEnd2)) {
    interval = 50000;
  }

  Blynk.setProperty(V2, "interval", interval * 1000);
}
