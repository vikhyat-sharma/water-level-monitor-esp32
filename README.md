# Water Level Monitor ESP32

ESP32-based dual tank water-level monitoring with:
- ultrasonic sensors for level measurement
- hosted web dashboard and API
- Telegram and Blynk notifications
- ESP-NOW command signaling for pump-controller workflows

## Features

- Real-time monitoring for two tanks with configurable tank height and sensor offset
- Web dashboard served directly from the ESP32
- Captive portal fallback AP mode when Wi-Fi connection fails
- Telegram periodic updates plus approval-based subscription flow
- Blynk integration for mobile dashboard updates
- ESP-NOW support for external controller commands (`OFF` when threshold condition is met)
- New: `/api/status` endpoint for health checks and automation integrations

## Quick Start

1. Install [PlatformIO](https://platformio.org/).
2. Open this project in VS Code + PlatformIO.
3. Update credentials and device settings in `Config.h`.
4. Build and upload:
   - `pio run`
   - `pio run -t upload`
5. Open serial monitor:
   - `pio device monitor -b 115200`

## Configuration

`Config.h` is intentionally committed with placeholders for public safety.
Fill these before flashing:
- `WIFI_SSID`, `WIFI_PASSWORD`
- `TELEGRAM_TOKEN`, `TELEGRAM_CHAT_ID`
- `BLYNK_AUTH_TOKEN`, `BLYNK_TEMPLATE_ID`
- sensor pins and tank dimensions as needed

## API Endpoints

- `GET /api/levels`  
  Returns tank percentages:
  `{"tank1":78.4,"tank2":42.1}`

- `GET /api/status`  
  Returns runtime health and sensor metadata:
  `{"device":"water-level-monitor-esp32","uptimeSec":1234,"wifiConnected":true,"ipAddress":"192.168.1.50","tank1":{"levelIn":31.2,"percent":78.0,"sensorOk":true},"tank2":{"levelIn":12.1,"percent":43.2,"sensorOk":true}}`

## Documentation

- Project details and architecture: `docs/PROJECT_DETAILS.md`
- Requirements and setup checklist: `docs/REQUIREMENTS.md`

## Repository Layout

- `WaterTankMonitor.ino` - main loop, scheduling, notification orchestration
- `SensorManager.*` - ultrasonic measurement and validation
- `WiFiManager.*` - station connect + AP fallback behavior
- `WebServerManager.*` - dashboard assets and HTTP API routes
- `TelegramManager.*` - Telegram messaging and command handling
- `BlynkManager.*` - Blynk connection and virtual-pin writes
- `EspNowManager.*` - ESP-NOW peer communication
- `ConfigManager.*` - NVS-backed runtime configuration persistence

## Notes

- Notification timing uses high-frequency windows and quiet hours from stored config.
- Outside high-frequency windows, Telegram periodic interval is 30 minutes.
- Legacy commented-out code has been removed for cleaner maintenance.

## License

This project is licensed under the [MIT License](LICENSE).

