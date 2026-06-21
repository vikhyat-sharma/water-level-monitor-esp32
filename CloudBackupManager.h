#pragma once

#include <Arduino.h>
#include "Config.h"

class CloudBackupManager {
public:
    void begin();
    bool backupNow();
    bool restoreLast();
};
