#ifndef ESPNOW_MANAGER_H
#define ESPNOW_MANAGER_H

#include <esp_now.h>
#include <WiFi.h>

struct MessagePacket {
  char type[16];    // "LEVEL" or "CMD"
  char payload[128];
};

class EspNowManager {
public:
  void begin();
  void sendCommand(const char* command);
  void sendLevels(int tank1, int tank2);
  void sendTelegramLevels(const String& message);
};

extern EspNowManager espNowManager;

#endif
