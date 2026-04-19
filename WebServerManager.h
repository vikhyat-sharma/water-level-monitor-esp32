#ifndef WEBSERVERMANAGER_H
#define WEBSERVERMANAGER_H

#include <WebServer.h>
#include "Config.h"
#include "SensorManager.h"
#include "ConfigManager.h"

class WebServerManager {
public:
  void startServer();
  void handleClient();
  WebServer server;

private:
  void handleRoot();
};

extern WebServerManager webServerManager;

#endif
