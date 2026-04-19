#ifndef WEB_UI_DATA_H
#define WEB_UI_DATA_H

const char index_html[] PROGMEM = R"rawliteral(
  <!DOCTYPE html>
  <html lang="en">
  <head>
    <meta charset="UTF-8">
    <title>Water Tank Monitor</title>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <link rel="stylesheet" href="/style.css">
    <link rel="manifest" href="/manifest.json">
    <link rel="icon" href="/favicon.ico" type="image/x-icon">
    <meta name="theme-color" content="#009688">
  </head>
  <body>
    <div class="container">
      <h1>💧 Water Tank Monitor</h1>
      <div class="tank">
        <h2>Badi Tanki</h2>
        <div class="tank-graphic large">
          <div id="tank1-fill" class="fill"></div>
        </div>
        <p id="tank1-percentage">0%</p>
      </div>
      <div class="tank">
        <h2>Choti Tanki</h2>
        <div class="tank-graphic small">
          <div id="tank2-fill" class="fill"></div>
        </div>
        <p id="tank2-percentage">0%</p>
      </div>
      <p class="ip-info">📶 Device IP: <span id="device-ip"></span></p>

    </div>
      <h2>📱 Notification Settings</h2>

      <h3>High Frequency Slots</h3>
      <label>Slot 1 Start (min from midnight): <input type="number" id="hfSlot1Start" value="480"></label>
      <label>Slot 1 End: <input type="number" id="hfSlot1End" value="540"></label>
      <br>
      <label>Slot 2 Start: <input type="number" id="hfSlot2Start" value="1080"></label>
      <label>Slot 2 End: <input type="number" id="hfSlot2End" value="1140"></label>

      <h3>Quiet Hours</h3>
      <label>Start: <input type="number" id="quietStart" value="1320"></label>
      <label>End: <input type="number" id="quietEnd" value="330"></label>

      <button onclick="saveConfig()">💾 Save Config</button>

      <div id="save-status"></div>
    <script src="/script.js"></script>
    <script>
    function saveConfig() {
      const params = new URLSearchParams();
      params.append("hfSlot1Start", document.getElementById("hfSlot1Start").value);
      params.append("hfSlot1End", document.getElementById("hfSlot1End").value);
      params.append("hfSlot2Start", document.getElementById("hfSlot2Start").value);
      params.append("hfSlot2End", document.getElementById("hfSlot2End").value);
      params.append("quietHoursStart", document.getElementById("quietStart").value);
      params.append("quietHoursEnd", document.getElementById("quietEnd").value);

      fetch("/saveConfig", {
        method: "POST",
        body: params
      })
      .then(response => response.text())
      .then(data => {
        document.getElementById("save-status").innerText = data;
      })
      .catch(error => {
        document.getElementById("save-status").innerText = "Error saving config.";
      });
    }
    </script>
    <script>
    function loadConfig() {
      fetch("/getConfig")
      .then(response => response.json())
      .then(data => {
        document.getElementById("hfSlot1Start").value = data.hfSlot1Start;
        document.getElementById("hfSlot1End").value = data.hfSlot1End;
        document.getElementById("hfSlot2Start").value = data.hfSlot2Start;
        document.getElementById("hfSlot2End").value = data.hfSlot2End;
        document.getElementById("quietStart").value = data.quietHoursStart;
        document.getElementById("quietEnd").value = data.quietHoursEnd;
      })
      .catch(error => {
        console.error("Error loading config:", error);
      });
    }

    // Call this when page loads
    window.onload = loadConfig;
    </script>

  </body>
  </html>
)rawliteral";

const char style_css[] PROGMEM = R"rawliteral(
  body {
    font-family: 'Arial', sans-serif;
    background: #e0f7fa;
    margin: 0;
    padding: 20px;
    text-align: center;
  }
  .container {
    max-width: 400px;
    margin: auto;
  }
  h1 {
    color: #00796b;
  }
  .tank {
    margin: 20px 0;
  }
  .tank-graphic {
    width: 100%;
    background: #cfd8dc;
    border: 3px solid #455a64;
    border-radius: 10px;
    overflow: hidden;
    position: relative;
    margin-bottom: 10px;
  }
  .tank-graphic.large { height: 200px; }
  .tank-graphic.small { height: 130px; }
  .fill {
    background: linear-gradient(to top, #009688, #4db6ac);
    width: 100%;
    height: 0%;
    position: absolute;
    bottom: 0;
    transition: height 0.5s ease;
  }
  p {
    margin: 0;
    font-size: 18px;
  }
  .ip-info {
    margin-top: 30px;
    font-size: 14px;
    color: #555;
  }
)rawliteral";

const char script_js[] PROGMEM = R"rawliteral(
function fetchWithTimeout(resource, options = {}) {
  const { timeout = 30000 } = options;

  return Promise.race([
    fetch(resource, options),
    new Promise((_, reject) =>
      setTimeout(() => reject(new Error("Timeout after 30s")), timeout)
    )
  ]);
}

function fetchLevels() {
  fetchWithTimeout("/api/levels")
    .then(response => response.json())
    .then(data => {
      const tank1Fill = document.getElementById("tank1-fill");
      const tank2Fill = document.getElementById("tank2-fill");
      const t1pct = ((data.tank1 / 100) * 100);
      const t2pct = ((data.tank2 / 100) * 100);
      tank1Fill.style.height = t1pct + "%";
      tank2Fill.style.height = t2pct + "%";
      if (t1pct < 0) {
        document.getElementById("tank1-percentage").textContent = "Sensor Error";
      } else {
        document.getElementById("tank1-percentage").textContent = t1pct.toFixed(1) + "%";
      }
      if (t2pct < 0) {
        document.getElementById("tank2-percentage").textContent = "Sensor Error";
      } else {
        document.getElementById("tank2-percentage").textContent = t2pct.toFixed(1) + "%";
      }
      document.getElementById("device-ip").textContent = window.location.hostname;
    })
    .catch(console.error)
    .finally(() => {
      setTimeout(fetchLevels, 500);
    });
}

fetchLevels();
)rawliteral";

const char manifest_json[] PROGMEM = R"rawliteral(
{
  "name": "Water Tank Monitor",
  "short_name": "TankMonitor",
  "start_url": "/",
  "display": "standalone",
  "background_color": "#e0f7fa",
  "theme_color": "#009688",
  "icons": [
    {
      "src": "/icon.png",
      "sizes": "192x192",
      "type": "image/png"
    },
    {
      "src": "/icon.png",
      "sizes": "512x512",
      "type": "image/png"
    }
  ]
}
)rawliteral";

#endif
