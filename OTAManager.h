#ifndef OTAMANAGER_H
#define OTAMANAGER_H

#include <ArduinoOTA.h>

class OTAManager {
public:
  void begin(const char* hostname = "water-tank-monitor");
  void handle();
  bool isUpdating();

private:
  bool updating = false;
};

extern OTAManager otaManager;

#endif
