// === RUET Fire Alarm Dashboard — Live SSE Wiring ===
// Uses the same communication model as the previous dashboard:
//   - EventSource('/events')
//   - Custom event names carrying JSON payloads
// Works with the existing DOM in index.html (gas1,temp1,battery1,status1, etc).

class FireAlarmDashboard {
  constructor() {
    this.sensors = {
      1: { gasDetected: false, gasPPM: 0, temperature: 24, batteryVoltage: 12.4, lastTs: Date.now() },
      2: { gasDetected: false, gasPPM: 0, temperature: 22, batteryVoltage: 11.8, lastTs: Date.now() },
    };

    this._initUI();
    this._attachSimulationShortcuts();
    this._startTimestampTicker();
    this._maybeStartSSE();
  }

  _initUI() {
    this.updateDisplay();
    this._setHeaderTime(new Date());
  }

  _attachSimulationShortcuts() {
    document.addEventListener('keydown', (e) => {
      if (e.key === '1') this.simulateGasAlert(1);
      else if (e.key === '2') this.simulateGasAlert(2);
      else if (e.key.toLowerCase() === 'r') this.resetAlerts();
    });
    // keep your existing buttons wired from index.html
    window.simulateGasAlert = (id) => this.simulateGasAlert(id);
    window.resetAlerts = () => this.resetAlerts();
  }

  _startTimestampTicker() {
    setInterval(() => this._updateTimestamps(), 1000);
  }

  _maybeStartSSE() {
    if (!window.EventSource) return; // fallback stays simulation
    const source = new EventSource('/events');

    source.addEventListener('open', () => console.log('SSE connected'), false);
    source.addEventListener('error', (e) => {
      if (e.target.readyState !== EventSource.OPEN) console.log('SSE disconnected');
    }, false);

    // === Event names from ESP32 (see ESP32_code.ino below) ===
    // 1) unit1_readings / unit2_readings   -> per-unit JSON payloads
    // 2) heartbeat                         -> time tick for header

    const onUnit = (unitId) => (e) => {
      try {
        const obj = JSON.parse(e.data);
        const s = this.sensors[unitId];
        s.gasPPM = obj.gasPPM;
        s.gasDetected = !!obj.gasDetected;
        s.temperature = obj.temperatureC;
        s.batteryVoltage = obj.batteryV;
        s.lastTs = Date.now();
        this.updateDisplay();
      } catch (err) {
        console.warn('Bad JSON payload for unit ' + unitId, err, e.data);
      }
    };

    source.addEventListener('unit1_readings', onUnit(1), false);
    source.addEventListener('unit2_readings', onUnit(2), false);

    source.addEventListener('heartbeat', (e) => {
      // Optional: server sends {"now": "<ISO>"} or just a tick
      this._setHeaderTime(new Date());
    }, false);
  }

  _setHeaderTime(d) {
    const el = document.getElementById('headerTime');
    if (el) el.textContent = d.toLocaleTimeString();
  }

  _updateTimestamps() {
    const setAgo = (id, ts) => {
      const el = document.getElementById(id);
      if (!el) return;
      const secs = Math.max(0, Math.round((Date.now() - ts) / 1000));
      el.textContent = `${secs} seconds ago`;
    };
    setAgo('lastUpdate1', this.sensors[1].lastTs);
    setAgo('lastUpdate2', this.sensors[2].lastTs);
    this._setHeaderTime(new Date());
  }

  updateDisplay() {
    [1,2].forEach((sensorId) => {
      const s = this.sensors[sensorId];
      const card = document.getElementById(`sensorCard${sensorId}`);
      const gasElement = document.getElementById(`gas${sensorId}`);
      const tempElement = document.getElementById(`temp${sensorId}`);
      const batteryElement = document.getElementById(`battery${sensorId}`);
      const statusElement = document.getElementById(`status${sensorId}`);

      if (!card || !gasElement || !tempElement || !batteryElement || !statusElement) return;

      // Gas
      gasElement.textContent = s.gasDetected ? 'GAS DETECTED!' : 'CLEAR';
      gasElement.classList.toggle('danger', s.gasDetected);
      card.classList.toggle('alert', s.gasDetected);

      // Status pill
      statusElement.textContent = s.gasDetected ? 'ALERT' : 'NORMAL';
      if (s.gasDetected) {
        statusElement.style.background = '#fff';
        statusElement.style.color = '#ff4757';
      } else {
        statusElement.style.background = '#10b981';
        statusElement.style.color = '#fff';
      }

      // Temp
      tempElement.textContent = `${(s.temperature ?? 0).toFixed(1)}°C`;

      const v = s.batteryVoltage ?? 0;
      batteryElement.textContent = `${v.toFixed(2)}V`;
      batteryElement.classList.toggle('danger', v < 11.0);
    });
  }

  // ==== Simulation utilities (kept as manual demo/fallback) ====
  simulateGasAlert(sensorId) {
    const s = this.sensors[sensorId];
    s.gasDetected = true;
    s.lastTs = Date.now();
    this.updateDisplay();
    this._beep();

    setTimeout(() => {
      if (s.gasDetected) {
        s.gasDetected = false;
        s.lastTs = Date.now();
        this.updateDisplay();
      }
    }, 10000);
  }

  resetAlerts() {
    [1,2].forEach(id => { this.sensors[id].gasDetected = false; this.sensors[id].lastTs = Date.now(); });
    this.updateDisplay();
  }

  _beep() {
    try {
      const Ctx = window.AudioContext || window.webkitAudioContext;
      const ac = new Ctx();
      const osc = ac.createOscillator();
      const gain = ac.createGain();
      osc.connect(gain); gain.connect(ac.destination);
      osc.frequency.setValueAtTime(800, ac.currentTime);
      gain.gain.setValueAtTime(0.3, ac.currentTime);
      gain.gain.exponentialRampToValueAtTime(0.01, ac.currentTime + 0.5);
      osc.start(); osc.stop(ac.currentTime + 0.5);
    } catch {}
  }
}

// bootstrap
let dashboard;
document.addEventListener('DOMContentLoaded', () => { dashboard = new FireAlarmDashboard(); });
