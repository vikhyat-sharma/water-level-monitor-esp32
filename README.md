# 💧 Water Level Monitor (ESP32)

A robust, real-time **dual tank water level monitoring system** built on ESP32.
Designed for reliability and automation, it combines sensor-based measurement, web monitoring, and smart notifications with seamless integration into IoT ecosystems.

---

## ✨ Features

* 📊 **Dual Tank Monitoring**
  Accurate level tracking using ultrasonic sensors with configurable tank height and offset
  - Multi-sample averaging for improved accuracy
  - Automatic sensor error detection and reporting

* 🌐 **Built-in Web Dashboard**
  Responsive UI hosted directly on the ESP32
  - Real-time tank level visualization
  - Historical data charts (24h/7d)
  - System status and diagnostics

* 📡 **Captive Portal Mode**
  Automatic fallback Access Point when Wi-Fi is unavailable

* 🔔 **Smart Notifications & Alerts**
  - Telegram alerts with approval-based subscription
  - Periodic updates with configurable intervals
  - Blynk mobile dashboard integration
  - **NEW:** Advanced alert system with configurable thresholds
  - **NEW:** Low/Critical/High level alerts with hysteresis
  - **NEW:** Sensor error notifications

* 🔗 **ESP-NOW Integration**
  Low-latency communication with external controllers
  (e.g., automatic `OFF` command for pump systems)

* 🧠 **Automation-Friendly API**
  REST endpoints for monitoring, health checks, and integrations

* 🔄 **OTA Updates** ⭐ NEW
  Over-the-air firmware updates for remote maintenance

* 📈 **Historical Data Logging** ⭐ NEW
  - SPIFFS-based data storage
  - Configurable logging intervals
  - Up to 7 days of historical data
  - API endpoint for data retrieval

* 🏠 **MQTT/Home Assistant Support** ⭐ NEW
  - Optional MQTT integration
  - Home Assistant auto-discovery compatible
  - Real-time status publishing

---

## 🚀 Quick Start

### 1. Setup Environment

* Install [PlatformIO](https://platformio.org/)
* Open project in **VS Code + PlatformIO**

### 2. Configure Project

Update credentials in:

```cpp
Config.h
```

### 3. Build & Upload

```bash
pio run
pio run -t upload
```

### 4. Monitor Output

```bash
pio device monitor -b 115200
```

---

## ⚙️ Configuration

All configuration is managed via:

```cpp
Config.h
```

Update the following before flashing:

* **Wi-Fi**

  * `WIFI_SSID`
  * `WIFI_PASSWORD`
  * `USE_STATIC_IP` (set to `true` to enable configured static IP values)

* **Telegram**

  * `TELEGRAM_TOKEN`
  * `TELEGRAM_CHAT_ID`

* **Blynk**

  * `BLYNK_AUTH_TOKEN`
  * `BLYNK_TEMPLATE_ID`

* **Hardware**

  * Sensor GPIO pins
  * Tank height and calibration values

---

## 🔌 API Reference

### `GET /api/levels`

Returns tank fill percentages:

```json
{"tank1":78.4,"tank2":42.1}
```

### `GET /api/status`

Returns system health and sensor data:

```json
{
  "device":"water-level-monitor-esp32",
  "uptimeSec":1234,
  "wifiConnected":true,
  "ipAddress":"192.168.1.50",
  "tank1":{"levelIn":31.2,"percent":78.0,"sensorOk":true},
  "tank2":{"levelIn":12.1,"percent":43.2,"sensorOk":true}
}
```

### `GET /api/system` ⭐ NEW

Returns detailed system information:

```json
{
  "device":"water-level-monitor-esp32",
  "version":"2.0.0",
  "uptime_seconds":12345,
  "free_heap":123456,
  "wifi_rssi":-45,
  "wifi_connected":true,
  "ip_address":"192.168.1.50",
  "data_logging":true,
  "mqtt_enabled":false,
  "mqtt_connected":false,
  "ota_enabled":true
}
```

### `GET /api/history?hours=24` ⭐ NEW

Returns historical data (default 24 hours, max 168 hours):

```json
[
  {"ts":1234567890,"t1":78.5,"t2":42.3},
  {"ts":1234568790,"t1":77.2,"t2":43.1}
]
```

### `POST /api/clearLogs?days=7` ⭐ NEW

Clears historical data older than specified days

---

## 🗂️ Project Structure

```
WaterTankMonitor.ino   # Main loop and orchestration
SensorManager.*        # Ultrasonic measurement logic with averaging
WiFiManager.*          # Wi-Fi + AP fallback handling
WebServerManager.*     # Dashboard UI and API routes
TelegramManager.*      # Telegram bot integration
BlynkManager.*         # Blynk communication
EspNowManager.*        # ESP-NOW messaging
ConfigManager.*        # Persistent configuration (NVS)
OTAManager.*           # ⭐ NEW: Over-the-air update handling
DataLogger.*           # ⭐ NEW: SPIFFS-based historical data logging
MQTTManager.*          # ⭐ NEW: MQTT/Home Assistant integration
AlertManager.*         # ⭐ NEW: Advanced alert system with thresholds
```

---

## 📚 Documentation

* `docs/PROJECT_DETAILS.md` — Architecture and design decisions
* `docs/REQUIREMENTS.md` — Setup checklist and requirements

---

## ⚠️ Notes

* Notification logic supports:

  * High-frequency alert windows
  * Quiet hours configuration

* Default Telegram update interval:

  * **30 minutes** (outside alert windows)

* Codebase is optimized for maintainability:

  * Legacy and unused code removed
  * Modular architecture for easy extension

---

## 🔒 Security

* Credentials are stored in `Config.h` (use placeholders in public repos)
* Avoid exposing the device directly to the public internet
* Consider firewall or VPN for remote access

---

## 🛠️ Roadmap

* [x] OTA firmware updates ✅
* [x] Historical data logging ✅
* [x] MQTT / Home Assistant integration ✅
* [x] Advanced alert rules ✅
* [ ] Web-based configuration UI
* [ ] Mobile app (React Native)
* [ ] Multi-language support
* [ ] Cloud backup integration

---

## 🤝 Contributing

Contributions are welcome:

1. Fork the repository
2. Create a feature branch
3. Submit a pull request

---

## 📄 License

Licensed under the MIT License. See [`LICENSE`](LICENSE) for details.

---