#ifndef DATALOGGER_H
#define DATALOGGER_H

#include <SPIFFS.h>
#include <ArduinoJson.h>

struct DataPoint {
  unsigned long timestamp;
  float tank1Percent;
  float tank2Percent;
};

class DataLogger {
public:
  void begin();
  void logData(float tank1Percent, float tank2Percent);
  String getHistoricalData(int hours = 24);
  void clearOldData(int daysToKeep = 7);
  bool isInitialized();

private:
  bool initialized = false;
  const char* logFile = "/data_log.json";
  const int maxEntries = 500;  // Keep last 500 entries
  
  void pruneOldEntries();
};

extern DataLogger dataLogger;

#endif
