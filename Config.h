#ifndef CONFIG_H
#define CONFIG_H

// WiFi Credentials
#define WIFI_SSID      "YOUR_WIFI_SSID"
#define WIFI_PASSWORD  "YOUR_WIFI_PASSWORD"

// Static IP Config
#define STATIC_IP      192, 168, 1, 2
#define GATEWAY_IP     192, 168, 1, 1
#define SUBNET_MASK    255, 255, 255, 0
#define PRIMARY_DNS    8, 8, 8, 8
#define SECONDARY_DNS  8, 8, 4, 4

// Telegram Bot Config
#define TELEGRAM_TOKEN "YOUR_TELEGRAM_BOT_TOKEN"
#define TELEGRAM_CHAT_ID "YOUR_TELEGRAM_CHAT_ID"

// Blynk Auth Token
#define BLYNK_AUTH_TOKEN "YOUR_BLYNK_AUTH_TOKEN"
#define BLYNK_TEMPLATE_ID "YOUR_BLYNK_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "Water Level Monitor"

// Tank Sensor Pins
#define TRIG_PIN_1 27
#define ECHO_PIN_1 26
#define TRIG_PIN_2 12
#define ECHO_PIN_2 14

// Tank Dimensions
#define TANK1_HEIGHT   40.0
#define TANK1_OFFSET   1.5
#define TANK2_HEIGHT   28.0
#define TANK2_OFFSET   5.5

// Update Intervals (seconds)
#define SENSOR_UPDATE_INTERVAL 5
#define BLYNK_DEFAULT_INTERVAL 1500

// High Frequency Time Slots (minutes from midnight)
#define HF_SLOT1_START    360   // 6:00 AM
#define HF_SLOT1_END      420   // 7:00 AM
#define HF_SLOT2_START    1020  // 5:00 PM
#define HF_SLOT2_END      1080  // 6:00 PM
#define QUITE_HOURS_START 1320 // 10:30 PM
#define QUITE_HOURS_END   345   // 5:45 AM

#endif
