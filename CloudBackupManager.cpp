#include "CloudBackupManager.h"
#include <SPIFFS.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

#include <Arduino.h>

void CloudBackupManager::begin() {
    // Initialize SPIFFS (if not already mounted elsewhere)
    if (!SPIFFS.begin(true)) {
        Serial.println("CloudBackup: SPIFFS mount failed");
    }
}

bool CloudBackupManager::backupNow() {
    if (!CLOUD_BACKUP_ENABLED) {
        Serial.println("CloudBackup: disabled in Config.h");
        return false;
    }

    const char *path = CLOUD_BACKUP_SPIFFS_PATH;
    const char *url = CLOUD_BACKUP_SIGNED_URL;
    if (strlen(url) == 0) {
        Serial.println("CloudBackup: signed URL not configured");
        return false;
    }

    return uploadFile(path, url);
}

bool CloudBackupManager::restoreLast() {
    // Restore is not implemented for GCS signed URL flow in this minimal scaffold.
    Serial.println("CloudBackup: restore not implemented");
    return false;
}

bool CloudBackupManager::uploadFile(const char *spiffsPath, const char *signedUrl) {
    if (!SPIFFS.exists(spiffsPath)) {
        Serial.printf("CloudBackup: file %s not found\n", spiffsPath);
        return false;
    }

    File f = SPIFFS.open(spiffsPath, "r");
    if (!f) {
        Serial.println("CloudBackup: failed to open file");
        return false;
    }

    size_t fileSize = f.size();

    WiFiClientSecure *client = new WiFiClientSecure();
    client->setInsecure(); // If you want cert validation, provide fingerprint or CA

    HTTPClient http;
    http.begin(*client, signedUrl);
    http.addHeader("Content-Type", "text/csv");

    int httpCode = http.sendRequest("PUT", &f, fileSize);
    bool success = false;
    if (httpCode >= 200 && httpCode < 300) {
        Serial.printf("CloudBackup: upload successful (%d)\n", httpCode);
        success = true;
    } else {
        Serial.printf("CloudBackup: upload failed (%d)\n", httpCode);
    }

    http.end();
    f.close();
    delete client;
    return success;
}
