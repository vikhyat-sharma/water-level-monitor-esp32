#include "WiFiManager.h"
#include "ConfigManager.h"
#include "SensorManager.h"
#include "WebServerManager.h"
#include "BlynkManager.h"
#include "TelegramManager.h"
#include "ArduinoJson.h"
#include "WiFiClientSecure.h"
#include "HTTPClient.h"
#include "EspNowManager.h"

unsigned long previousMillis = 0;
unsigned long intervalMillis = 0;
unsigned long previousNotificationMillis = 0;
unsigned long notificationIntervalMillis = 2 * 60 * 1000;  // 2 minutes
unsigned long previousTelegramMillis = 0;
unsigned long telegramIntervalMillis = 30 * 60 * 1000;  // 30 minutes

void setup() {
  Serial.begin(115200);
  configManager.loadConfig();
  wifiManager.connectWiFi();
  sensorManager.setupSensors();
  webServerManager.startServer();
  blynkManager.begin();
  telegramManager.sendMessage("Tank Monitor started. IP: " + wifiManager.getIPAddress());
  telegramManager.sendToApproved("Tank Monitor started. IP: " + wifiManager.getIPAddress());
  intervalMillis = configManager.config.updateInterval * 1000;
  configTime(19800, 0, "pool.ntp.org");  // 19800 = GMT+5:30 for IST
  espNowManager.begin();

  sendNotification(true);
}

void loop() {
  // Check WiFi status and reconnect if disconnected
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected, reconnecting...");
    WiFi.disconnect();
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    unsigned long startAttemptTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
      delay(500);
      Serial.print(".");
    }
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\nReconnected to WiFi!");
    } else {
      Serial.println("\nReconnect failed, keeping AP mode available.");
    }
  }

  unsigned long currentMillis = millis();

  webServerManager.handleClient();
  wifiManager.handleDNS();
  blynkManager.run();

  if (currentMillis - previousMillis >= intervalMillis) {
    previousMillis = currentMillis;

    float tank1Level = sensorManager.readTankLevel(TRIG_PIN_1, ECHO_PIN_1, configManager.config.tank1Height, configManager.config.tank1Offset);
    float tank2Level = sensorManager.readTankLevel(TRIG_PIN_2, ECHO_PIN_2, configManager.config.tank2Height, configManager.config.tank2Offset);

    Serial.print("Tank 1: "); Serial.print(tank1Level); Serial.print(" in  |  ");
    Serial.print("Tank 2: "); Serial.print(tank2Level); Serial.println(" in");

    if (tank1Level < 0 || tank2Level < 0) {
      Serial.println("Tank 1/2 sensor error");
    } else {
      float tank1Percent = (tank1Level / configManager.config.tank1Height) * 100;
      float tank2Percent = (tank2Level / configManager.config.tank2Height) * 100;
      if (tank2Percent > 95.0) {
        espNowManager.sendCommand("OFF");
      }
    }

  }

  int minutesNow = getMinutesSinceMidnight();
  if (minutesNow == -1) {
    // time fetch failed, skip adjustment
  } else {
    if ((minutesNow >= configManager.config.highFreqStart1 && minutesNow <= configManager.config.highFreqEnd1) ||
        (minutesNow >= configManager.config.highFreqStart2 && minutesNow <= configManager.config.highFreqEnd2)) {
      telegramIntervalMillis = 2 * 60 * 1000;  // 2 minutes
    } else {
      telegramIntervalMillis = 30 * 60 * 1000;  // 30 minutes
    }
  }

  // Check quiet hours — suppress notification if within quiet period
  bool inQuietHours = false;
  if (minutesNow >= 0) {
    if (configManager.config.quietHoursStart < configManager.config.quietHoursEnd) {
      // Normal case, e.g. 360 to 420
      inQuietHours = (minutesNow >= configManager.config.quietHoursStart && minutesNow < configManager.config.quietHoursEnd);
    } else {
      // Spans over midnight
      inQuietHours = (minutesNow >= configManager.config.quietHoursStart || minutesNow < configManager.config.quietHoursEnd);
    }
  }

  if (!inQuietHours) {
    // Every 2 mins
    if (currentMillis - previousNotificationMillis >= notificationIntervalMillis) {
      previousNotificationMillis = currentMillis;

      // Every 30 mins
      bool telegramNotification = (currentMillis - previousTelegramMillis >= telegramIntervalMillis);
      if (telegramNotification) {
        previousTelegramMillis = currentMillis;
      }

      sendNotification(telegramNotification);
    }
  }

  blynkManager.updateBlynkInterval();

}

void sendNotification(bool sendTelegram) {
    float tank1Level = sensorManager.readTankLevel(TRIG_PIN_1, ECHO_PIN_1, configManager.config.tank1Height, configManager.config.tank1Offset);
    float tank2Level = sensorManager.readTankLevel(TRIG_PIN_2, ECHO_PIN_2, configManager.config.tank2Height, configManager.config.tank2Offset);

    String message = "📊 Water Tank Levels:\n";
    float tank1Percent = -1;
    float tank2Percent = -1;

    if (tank1Level < 0) {
      Serial.println("Tank 1 sensor error");
      message += "Badi Tanki: Sensor Error\n";
    } else {
      Serial.print("Tank 1 Level: "); Serial.println(tank1Level);
      tank1Percent = (tank1Level / configManager.config.tank1Height) * 100;
      message += "Badi Tanki: " + String(tank1Percent, 1) + "%\n";
    }
    if (tank2Level < 0) {
      Serial.println("Tank 2 sensor error");
      message += "Choti Tanki: Sensor Error\n";
    } else {
      Serial.print("Tank 2 Level: "); Serial.println(tank2Level);
      tank2Percent = (tank2Level / configManager.config.tank2Height) * 100;
      message += "Choti Tanki: " + String(tank2Percent, 1) + "%";
    }

    if (sendTelegram) {
      telegramManager.sendMessage(message);
      telegramManager.sendToApproved(message);
    }
    blynkManager.sendTankLevels(tank1Percent, tank2Percent);

    pollTelegramUpdates();
}

int getMinutesSinceMidnight() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return -1;
  }
  return timeinfo.tm_hour * 60 + timeinfo.tm_min;
}

unsigned long lastUpdateId = 0;

void pollTelegramUpdates() {
  if (WiFi.status() != WL_CONNECTED) return;

  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient https;
  String url = "https://api.telegram.org/bot" + String(TELEGRAM_TOKEN) + "/getUpdates?offset=" + String(lastUpdateId + 1);

  if (https.begin(client, url)) {
    int httpCode = https.GET();
    if (httpCode == 200) {
      String payload = https.getString();

      const size_t capacity = 30 * JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(3) + 2048;
      DynamicJsonDocument doc(capacity);
      DeserializationError error = deserializeJson(doc, payload);

      if (!error && doc["ok"]) {
        JsonArray result = doc["result"].as<JsonArray>();

        for (JsonVariant update : result) {
          JsonObject obj = update.as<JsonObject>();
          lastUpdateId = obj["update_id"].as<unsigned long>();

          if (obj.containsKey("message")) {
            String chatId = obj["message"]["chat"]["id"].as<String>();
            String text = obj["message"]["text"].as<String>();
            Serial.println("Received: " + text + " from " + chatId);
            telegramManager.handleCommand(chatId, text);
          }
        }
      }
    }
    https.end();
  }
}
