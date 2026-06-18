#include <Arduino.h>
#include "DataLogger.h"

DataLogger dataLogger;

void DataLogger::begin() {
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS Mount Failed");
    initialized = false;
    return;
  }
  Serial.println("SPIFFS Mounted Successfully");
  initialized = true;
  
  // Create log file if it doesn't exist
  if (!SPIFFS.exists(logFile)) {
    File file = SPIFFS.open(logFile, FILE_WRITE);
    if (file) {
      file.println("[]");
      file.close();
      Serial.println("Created new log file");
    }
  }
}

bool DataLogger::isInitialized() {
  return initialized;
}

void DataLogger::logData(float tank1Percent, float tank2Percent) {
  if (!initialized) return;

  File file = SPIFFS.open(logFile, FILE_READ);
  if (!file) {
    Serial.println("Failed to open log file for reading");
    return;
  }

  DynamicJsonDocument doc(16384);
  DeserializationError error = deserializeJson(doc, file);
  file.close();

  if (error) {
    Serial.println("Failed to parse log file, creating new");
    doc.to<JsonArray>();
  }

  JsonArray array = doc.as<JsonArray>();
  
  // Add new entry
  JsonObject entry = array.createNestedObject();
  entry["ts"] = millis() / 1000;  // timestamp in seconds
  entry["t1"] = round(tank1Percent * 10) / 10.0;  // Round to 1 decimal
  entry["t2"] = round(tank2Percent * 10) / 10.0;

  // Prune if too many entries
  while (array.size() > maxEntries) {
    array.remove(0);
  }

  // Write back to file
  file = SPIFFS.open(logFile, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open log file for writing");
    return;
  }

  serializeJson(doc, file);
  file.close();
}

String DataLogger::getHistoricalData(int hours) {
  if (!initialized) return "[]";

  File file = SPIFFS.open(logFile, FILE_READ);
  if (!file) {
    return "[]";
  }

  String content = file.readString();
  file.close();

  // Filter data based on hours requested
  DynamicJsonDocument doc(16384);
  DeserializationError error = deserializeJson(doc, content);
  
  if (error) {
    return "[]";
  }

  JsonArray array = doc.as<JsonArray>();
  DynamicJsonDocument filtered(16384);
  JsonArray filteredArray = filtered.to<JsonArray>();
  
  unsigned long currentTime = millis() / 1000;
  unsigned long cutoffTime = currentTime - (hours * 3600);

  for (JsonVariant v : array) {
    JsonObject obj = v.as<JsonObject>();
    unsigned long ts = obj["ts"];
    if (ts >= cutoffTime) {
      filteredArray.add(obj);
    }
  }

  String result;
  serializeJson(filtered, result);
  return result;
}

void DataLogger::clearOldData(int daysToKeep) {
  if (!initialized) return;

  File file = SPIFFS.open(logFile, FILE_READ);
  if (!file) return;

  DynamicJsonDocument doc(16384);
  DeserializationError error = deserializeJson(doc, file);
  file.close();

  if (error) return;

  JsonArray array = doc.as<JsonArray>();
  DynamicJsonDocument filtered(16384);
  JsonArray filteredArray = filtered.to<JsonArray>();
  
  unsigned long currentTime = millis() / 1000;
  unsigned long cutoffTime = currentTime - (daysToKeep * 86400);

  for (JsonVariant v : array) {
    JsonObject obj = v.as<JsonObject>();
    unsigned long ts = obj["ts"];
    if (ts >= cutoffTime) {
      filteredArray.add(obj);
    }
  }

  file = SPIFFS.open(logFile, FILE_WRITE);
  if (file) {
    serializeJson(filtered, file);
    file.close();
  }
}

void DataLogger::pruneOldEntries() {
  clearOldData(7);
}
