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
#include "OTAManager.h"
#include "DataLogger.h"
#include "MQTTManager.h"
#include "AlertManager.h"

struct TankLevels {
  float level1;
  float level2;
  float percent1;
  float percent2;
  bool ok1;
  bool ok2;
};

static const unsigned long kNotificationIntervalMs = 2UL * 60UL * 1000UL;
static const unsigned long kTelegramIntervalMs = 30UL * 60UL * 1000UL;
static const unsigned long kWifiReconnectTimeoutMs = 10000UL;
static const unsigned long kDataLogIntervalMs = LOG_INTERVAL_MINUTES * 60UL * 1000UL;

unsigned long previousMillis = 0;
unsigned long intervalMillis = 0;
unsigned long previousNotificationMillis = 0;
unsigned long notificationIntervalMillis = kNotificationIntervalMs;
unsigned long previousTelegramMillis = 0;
unsigned long telegramIntervalMillis = kTelegramIntervalMs;
unsigned long previousDataLogMillis = 0;
unsigned long lastUpdateId = 0;

TankLevels readTankLevels() {
  TankLevels data;
  // Use averaged readings for better accuracy
  data.level1 = sensorManager.readTankLevelAverage(TRIG_PIN_1, ECHO_PIN_1, configManager.config.tank1Height, configManager.config.tank1Offset, SENSOR_SAMPLES);
  data.level2 = sensorManager.readTankLevelAverage(TRIG_PIN_2, ECHO_PIN_2, configManager.config.tank2Height, configManager.config.tank2Offset, SENSOR_SAMPLES);
  data.ok1 = data.level1 >= 0;
  data.ok2 = data.level2 >= 0;
  data.percent1 = data.ok1 ? (data.level1 / configManager.config.tank1Height) * 100.0f : -1.0f;
  data.percent2 = data.ok2 ? (data.level2 / configManager.config.tank2Height) * 100.0f : -1.0f;
  return data;
}

bool isWithinWindow(int value, int start, int end) {
  if (start < end) {
    return value >= start && value <= end;
  }
  return value >= start || value <= end;
}

bool isHighFrequencyTime(int minutesNow) {
  return isWithinWindow(minutesNow, configManager.config.highFreqStart1, configManager.config.highFreqEnd1) ||
         isWithinWindow(minutesNow, configManager.config.highFreqStart2, configManager.config.highFreqEnd2);
}

bool isQuietHours(int minutesNow) {
  return isWithinWindow(minutesNow, configManager.config.quietHoursStart, configManager.config.quietHoursEnd);
}

int getMinutesSinceMidnight() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return -1;
  }
  return timeinfo.tm_hour * 60 + timeinfo.tm_min;
}

void reconnectWiFiIfNeeded() {
  if (WiFi.status() == WL_CONNECTED) {
    return;
  }

  Serial.println("WiFi disconnected, reconnecting...");
  WiFi.disconnect();
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  unsigned long startAttemptTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < kWifiReconnectTimeoutMs) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nReconnected to WiFi!");
  } else {
    Serial.println("\nReconnect failed, keeping AP mode available.");
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("\n\n=== Water Tank Monitor Starting ===");
  
  configManager.loadConfig();
  wifiManager.connectWiFi();
  sensorManager.setupSensors();
  
  // Initialize new managers
  otaManager.begin("water-tank-monitor");
  
  #if ENABLE_DATA_LOGGING
  dataLogger.begin();
  #endif
  
  mqttManager.begin();
  alertManager.begin();
  
  webServerManager.startServer();
  blynkManager.begin();
  
  String startMsg = "🚀 Tank Monitor started\nIP: " + wifiManager.getIPAddress() + "\nFeatures: OTA, Logging, MQTT, Alerts";
  telegramManager.sendMessage(startMsg);
  telegramManager.sendToApproved(startMsg);
  
  intervalMillis = static_cast<unsigned long>(configManager.config.updateInterval) * 1000UL;
  configTime(19800, 0, "pool.ntp.org");  // IST
  espNowManager.begin();

  Serial.println("=== Initialization Complete ===\n");
  sendNotification(true);
}

void loop() {
  reconnectWiFiIfNeeded();
  
  // Handle OTA updates
  otaManager.handle();
  
  // Handle MQTT
  mqttManager.loop();

  unsigned long currentMillis = millis();
  webServerManager.handleClient();
  wifiManager.handleDNS();
  blynkManager.run();

  if (currentMillis - previousMillis >= intervalMillis) {
    previousMillis = currentMillis;
    TankLevels tankData = readTankLevels();

    Serial.print("Tank 1: "); Serial.print(tankData.level1); Serial.print(" in (");
    Serial.print(tankData.percent1); Serial.print("%)  |  ");
    Serial.print("Tank 2: "); Serial.print(tankData.level2); Serial.print(" in (");
    Serial.print(tankData.percent2); Serial.println("%)");

    // Check alerts with new alert manager
    alertManager.checkAlerts(tankData.percent1, tankData.percent2, tankData.ok1, tankData.ok2);

    if (!tankData.ok1 || !tankData.ok2) {
      Serial.println("Tank 1/2 sensor error");
    } else if (tankData.percent2 > 95.0f) {
      espNowManager.sendCommand("OFF");
    }
    
    // Publish to MQTT
    mqttManager.publishTankLevels(tankData.percent1, tankData.percent2, tankData.ok1, tankData.ok2);
  }

  // Data logging
  #if ENABLE_DATA_LOGGING
  if (currentMillis - previousDataLogMillis >= kDataLogIntervalMs) {
    previousDataLogMillis = currentMillis;
    TankLevels tankData = readTankLevels();
    if (tankData.ok1 && tankData.ok2) {
      dataLogger.logData(tankData.percent1, tankData.percent2);
      Serial.println("Data logged to SPIFFS");
    }
  }
  #endif

  int minutesNow = getMinutesSinceMidnight();
  if (minutesNow >= 0) {
    telegramIntervalMillis = isHighFrequencyTime(minutesNow) ? (2UL * 60UL * 1000UL) : kTelegramIntervalMs;
  }

  if (!isQuietHours(minutesNow)) {
    if (currentMillis - previousNotificationMillis >= notificationIntervalMillis) {
      previousNotificationMillis = currentMillis;
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
  TankLevels tankData = readTankLevels();
  String message = "📊 Water Tank Levels:\n";

  if (!tankData.ok1) {
    Serial.println("Tank 1 sensor error");
    message += "Badi Tanki: Sensor Error\n";
  } else {
    Serial.print("Tank 1 Level: "); Serial.println(tankData.level1);
    message += "Badi Tanki: " + String(tankData.percent1, 1) + "%\n";
  }

  if (!tankData.ok2) {
    Serial.println("Tank 2 sensor error");
    message += "Choti Tanki: Sensor Error";
  } else {
    Serial.print("Tank 2 Level: "); Serial.println(tankData.level2);
    message += "Choti Tanki: " + String(tankData.percent2, 1) + "%";
  }

  if (sendTelegram) {
    telegramManager.sendMessage(message);
    telegramManager.sendToApproved(message);
  }

  blynkManager.sendTankLevels(tankData.percent1, tankData.percent2);
  pollTelegramUpdates();
}

void pollTelegramUpdates() {
  if (WiFi.status() != WL_CONNECTED) {
    return;
  }

  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient https;
  String url = "https://api.telegram.org/bot" + String(TELEGRAM_TOKEN) + "/getUpdates?offset=" + String(lastUpdateId + 1);

  if (!https.begin(client, url)) {
    return;
  }

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
