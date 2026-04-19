#include <ArduinoJson.h>
#include "WebServerManager.h"
#include "WebUIData.h"
#include "WiFiManager.h"

WebServerManager webServerManager;

void WebServerManager::startServer() {
  server.on("/", HTTP_GET, [this]() {
    server.send_P(200, "text/html", index_html);
  });

  server.on("/style.css", HTTP_GET, [this]() {
    server.send_P(200, "text/css", style_css);
  });

  server.on("/script.js", HTTP_GET, [this]() {
    server.send_P(200, "application/javascript", script_js);
  });

  server.on("/manifest.json", HTTP_GET, [this]() {
    server.send_P(200, "application/manifest+json", manifest_json);
  });

  server.on("/api/levels", HTTP_GET, [this]() {
    float level1 = sensorManager.readTankLevel(TRIG_PIN_1, ECHO_PIN_1, configManager.config.tank1Height, configManager.config.tank1Offset);
    float level2 = sensorManager.readTankLevel(TRIG_PIN_2, ECHO_PIN_2, configManager.config.tank2Height, configManager.config.tank2Offset);

    float percentage1 = (level1 / configManager.config.tank1Height) * 100;
    float percentage2 = (level2 / configManager.config.tank2Height) * 100;
    String json = "{\"tank1\":" + String(percentage1, 1) + ",\"tank2\":" + String(percentage2, 1) + "}";
    server.send(200, "application/json", json);
  });

  // Public status endpoint for quick health checks and automation.
  server.on("/api/status", HTTP_GET, [this]() {
    float level1 = sensorManager.readTankLevel(TRIG_PIN_1, ECHO_PIN_1, configManager.config.tank1Height, configManager.config.tank1Offset);
    float level2 = sensorManager.readTankLevel(TRIG_PIN_2, ECHO_PIN_2, configManager.config.tank2Height, configManager.config.tank2Offset);

    float percentage1 = (level1 < 0) ? -1 : (level1 / configManager.config.tank1Height) * 100.0f;
    float percentage2 = (level2 < 0) ? -1 : (level2 / configManager.config.tank2Height) * 100.0f;

    DynamicJsonDocument doc(768);
    doc["device"] = "water-level-monitor-esp32";
    doc["uptimeSec"] = millis() / 1000;
    doc["wifiConnected"] = WiFi.status() == WL_CONNECTED;
    doc["ipAddress"] = wifiManager.getIPAddress();

    JsonObject tank1 = doc.createNestedObject("tank1");
    tank1["levelIn"] = (level1 < 0) ? 0 : level1;
    tank1["percent"] = percentage1;
    tank1["sensorOk"] = level1 >= 0;

    JsonObject tank2 = doc.createNestedObject("tank2");
    tank2["levelIn"] = (level2 < 0) ? 0 : level2;
    tank2["percent"] = percentage2;
    tank2["sensorOk"] = level2 >= 0;

    String json;
    serializeJson(doc, json);
    server.send(200, "application/json", json);
  });

  server.on("/getConfig", HTTP_GET, [this]() {
    DynamicJsonDocument doc(512);

    doc["hfSlot1Start"]     = configManager.config.highFreqStart1;
    doc["hfSlot1End"]       = configManager.config.highFreqEnd1;
    doc["hfSlot2Start"]     = configManager.config.highFreqStart2;
    doc["hfSlot2End"]       = configManager.config.highFreqEnd2;
    doc["quietHoursStart"]  = configManager.config.quietHoursStart;
    doc["quietHoursEnd"]    = configManager.config.quietHoursEnd;

    String jsonString;
    serializeJson(doc, jsonString);

    server.send(200, "application/json", jsonString);
  });

  server.on("/saveConfig", HTTP_POST, [this]() {
    if (server.hasArg("hfSlot1Start")) {
      configManager.config.highFreqStart1 = server.arg("hfSlot1Start").toInt();
    }
    if (server.hasArg("hfSlot1End")) {
      configManager.config.highFreqEnd1 = server.arg("hfSlot1End").toInt();
    }
    if (server.hasArg("hfSlot2Start")) {
      configManager.config.highFreqStart2 = server.arg("hfSlot2Start").toInt();
    }
    if (server.hasArg("hfSlot2End")) {
      configManager.config.highFreqEnd2 = server.arg("hfSlot2End").toInt();
    }
    if (server.hasArg("quietHoursStart")) {
      configManager.config.quietHoursStart = server.arg("quietHoursStart").toInt();
    }
    if (server.hasArg("quietHoursEnd")) {
      configManager.config.quietHoursEnd = server.arg("quietHoursEnd").toInt();
    }

    configManager.saveConfig();
    server.send(200, "text/plain", "Config saved.");
  });

  server.onNotFound([this]() {
    server.send(404, "text/plain", "Not Found");
  });

  server.begin();
  Serial.println("Web Server started.");
}

void WebServerManager::handleClient() {
  server.handleClient();
}
