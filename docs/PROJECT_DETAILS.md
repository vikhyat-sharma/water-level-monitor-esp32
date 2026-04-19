# Project Details

## Overview

This project monitors water levels in two tanks using ultrasonic sensors connected to an ESP32.  
It combines local display (web dashboard) and remote notifications (Telegram + Blynk), and can send control signals to another ESP32 using ESP-NOW.

## Core Flow

1. Boot and load persisted config from ESP32 NVS (`Preferences`).
2. Connect to Wi-Fi; if connection fails, switch to AP mode with captive portal handling.
3. Start local web server and API routes.
4. Read tank levels at configured interval.
5. Send updates to Telegram/Blynk and optional ESP-NOW command channel.

## Module Responsibilities

- `WaterTankMonitor.ino`  
  Main orchestrator, timing, scheduling, notifications, Telegram polling.
- `SensorManager.*`  
  Sensor pin setup and ultrasonic distance-to-water-level conversion, with basic error validation.
- `ConfigManager.*`  
  Reads/writes runtime configuration from NVS.
- `WiFiManager.*`  
  Connects ESP32 to Wi-Fi or starts AP fallback.
- `WebServerManager.*`  
  Hosts dashboard files and REST-style endpoints.
- `TelegramManager.*`  
  Sends messages and handles bot command approval flow.
- `BlynkManager.*`  
  Connects and pushes virtual pin values.
- `EspNowManager.*`  
  Sends structured ESP-NOW packets to paired peer.

## Exposed HTTP Endpoints

- `GET /` - dashboard UI
- `GET /style.css`, `GET /script.js`, `GET /manifest.json` - UI assets
- `GET /api/levels` - tank percentages
- `GET /api/status` - device + tank health (new)
- `GET /getConfig` - fetch high-frequency and quiet-hour config
- `POST /saveConfig` - save time-slot config

## Notification Strategy

- A short periodic notification timer handles frequent checks.
- Telegram broadcast interval is dynamic:
  - High-frequency slots: 2 minutes
  - Outside slots: 30 minutes
- Quiet hours suppress outgoing notifications.

## Public Release Hygiene

- Credentials and tokens are placeholders in `Config.h`.
- Build/editor artifacts are excluded via `.gitignore`.
- Add your own `LICENSE` and screenshots before publishing.
