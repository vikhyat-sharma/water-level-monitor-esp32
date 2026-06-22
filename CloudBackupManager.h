#pragma once

#include <Arduino.h>
#include "Config.h"
#include <FS.h>

// Cloud backup implementation using HTTPS PUT to a pre-signed Google Cloud Storage URL.

class CloudBackupManager {
public:
    void begin();
    bool backupNow();
    bool restoreLast();
    bool uploadFile(const char *spiffsPath, const char *signedUrl);
};
