function fetchLevels() {
  fetch('/api/levels')
    .then(response => response.json())
    .then(data => {
      document.getElementById('tank1-fill').style.height = data.tank1 + '%';
      document.getElementById('tank1-percentage').innerText = data.tank1 + '%';
      document.getElementById('tank2-fill').style.height = data.tank2 + '%';
      document.getElementById('tank2-percentage').innerText = data.tank2 + '%';
    });
}

function fetchIP() {
  fetch('/api/ip')
    .then(response => response.text())
    .then(data => {
      document.getElementById('device-ip').innerText = data;
    });
}

setInterval(fetchLevels, 2000);
fetchLevels();
fetchIP();
