#include "EspNowManager.h"
#include <WiFi.h>
#include <Arduino.h>
#include <cstring>

uint8_t controllerAddress[] = {0xF4, 0x65, 0x0B, 0x48, 0xFB, 0x10};
esp_now_peer_info_t peerInfo;

void EspNowManager::begin() {
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed!");
    return;
  }

  memcpy(peerInfo.peer_addr, controllerAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add ESP-NOW peer");
  }
}

void EspNowManager::sendCommand(const char* command) {
  MessagePacket msg;
  strcpy(msg.type, "CMD");
  strncpy(msg.payload, command, sizeof(msg.payload) - 1);
  msg.payload[sizeof(msg.payload) - 1] = '\0';

  Serial.print("Sending CMD: "); Serial.println(command);
  esp_now_send(controllerAddress, (uint8_t*)&msg, sizeof(msg));
}

void EspNowManager::sendLevels(int tank1, int tank2) {
  MessagePacket msg;
  strcpy(msg.type, "LEVEL");
  sprintf(msg.payload, "%d,%d", tank1, tank2);

  Serial.print("Sending Levels: "); Serial.println(msg.payload);
  esp_now_send(controllerAddress, (uint8_t*)&msg, sizeof(msg));
}

void EspNowManager::sendTelegramLevels(const String& message) {
  MessagePacket msg;
  strcpy(msg.type, "TELEGRAM");
  message.toCharArray(msg.payload, sizeof(msg.payload));

  Serial.print("Sending TELEGRAM Levels: "); Serial.println(msg.payload);
  esp_now_send(controllerAddress, (uint8_t*)&msg, sizeof(msg));
}

EspNowManager espNowManager;
