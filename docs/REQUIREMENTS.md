# Requirements

## Hardware

- ESP32 development board (`esp32dev` profile in `platformio.ini`)
- 2x ultrasonic sensors (for tank-level measurement)
- Stable 5V/USB power supply
- Wiring and connectors suitable for your enclosure

## Software

- [VS Code](https://code.visualstudio.com/)
- [PlatformIO extension](https://platformio.org/platformio-ide)
- Arduino framework for ESP32 (managed by PlatformIO)

## Cloud / Integration Accounts

- Wi-Fi network credentials
- Telegram bot token and admin chat ID
- Blynk template + auth token (optional but supported)

## Library Dependencies

Defined in `platformio.ini`:
- `blynkkk/Blynk`

Used by source code (bundled with ESP32 Arduino core / common libs):
- `ArduinoJson`
- `Preferences`
- `WiFi`, `WebServer`, `DNSServer`
- `esp_now`

## Setup Checklist

1. Configure `Config.h` with your credentials and pin mapping.
2. Verify sensor wiring matches:
   - `TRIG_PIN_1`, `ECHO_PIN_1`
   - `TRIG_PIN_2`, `ECHO_PIN_2`
3. Build and flash firmware:
   - `pio run`
   - `pio run -t upload`
4. Open serial monitor (`115200`) and confirm:
   - Wi-Fi connected, or
   - AP fallback started as expected.
5. Open dashboard in browser using the device IP.
6. Validate API:
   - `/api/levels`
   - `/api/status`
7. Validate Telegram and Blynk notifications.

## Optional Production Hardening

- Add a dedicated `LICENSE` file.
- Add hardware and UI screenshots to repository docs.
- Rotate any previously exposed credentials/tokens before publishing.
