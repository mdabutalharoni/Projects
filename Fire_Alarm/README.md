# Fire Alarm — RUET EEE Smoke/Gas Detection System

A wireless, battery-powered fire/smoke detection system built for RUET EEE, using an **ESP-NOW mesh of sensor nodes** reporting to a central **ESP32 hub** that serves a real-time web dashboard. Mechanical design (enclosures, mounts, battery holders) is done in SolidWorks.

---

## Project structure

| Folder | Contents |
|---|---|
| `Fire Detection/` | All firmware (Arduino/ESP32) and the web dashboard |
| `Design/` | SolidWorks part/assembly files (`.SLDPRT`/`.SLDASM`), STEP files, and exported STL meshes for the enclosure, hub, mounts, and battery holders |
| `IMG_*.JPG`, `IMG_*.MOV` | Photos and video of the physical build |

---

## How the system works

```
 Sensor Node (Slave_Initiator)         Hub (Master_Responder / Code_ESPNOW)
 ─────────────────────────────         ──────────────────────────────────────
  Gas / temp / battery reading           Receives sensor data over ESP-NOW
        │                                        │
        └── ESP-NOW (broadcast) ────────────────►│
                                                  ├─► Serial log (MAC, values)
                                                  └─► Web dashboard via SSE
                                                       (WiFi AP/STA + LittleFS)
```

- **Sensor nodes** run `Slave_Initiator.ino`: read gas/temperature/battery values and broadcast them every 500 ms over **ESP-NOW** (no WiFi router needed — direct ESP32-to-ESP32) to the hub's MAC address.
- **The hub** runs one of two firmware variants (see below) and re-publishes incoming readings to a browser dashboard over **Server-Sent Events (SSE)**, so the dashboard updates live without polling.
- The dashboard (`data/index.html`, `style.css`, `script.js`) is served from the ESP32's onboard flash via **LittleFS**, so no external web server is needed — just connect to the hub's WiFi and open its IP in a browser.

---

## Firmware variants — which file does what

| File | Role |
|---|---|
| **`Fire Detection/Slave_Initiator/Slave_Initiator.ino`** | Sensor-node firmware. Sends simulated gas/temp/battery readings over ESP-NOW every 500 ms. **Replace the `broadcastAddress[]` MAC with your hub's actual MAC address**, and swap the `random(...)` calls for real sensor readings (e.g. MQ-series gas sensor + thermistor/DHT ADC reads) when wiring up real hardware. |
| **`Fire Detection/Master_Responder/Master_Responder.ino`** | Minimal hub firmware — receives ESP-NOW packets and prints sensor name, MAC, gas/temp/battery to Serial. Good for debugging a new sensor node before wiring it into the full dashboard. |
| **`Fire Detection/Code/Code.ino`** | Standalone dashboard demo — serves the full web UI and pushes **simulated** two-zone data over SSE. No ESP-NOW involved; useful for developing/testing the dashboard UI on its own. |
| **`Fire Detection/Code_ESPNOW/Code_ESPNOW.ino`** | **The real integration.** Combines `Master_Responder`'s ESP-NOW receive logic with `Code.ino`'s dashboard: Zone A (`unit1`) is driven by real incoming ESP-NOW sensor data, Zone B (`unit2`) is still simulated as a placeholder for a second sensor node. This is the file to build on for a multi-zone real deployment. |

> **Known thing to fix:** in `Code_ESPNOW.ino`, `OnDataRecv()` calls `sendUnitEvent(...)` before it's defined further down the file. Arduino IDE auto-generates function prototypes so this compiles fine as-is — but if you ever move this logic into a `.cpp`/`.h` file pair, you'll need to add an explicit forward declaration.

---

## Building/flashing the firmware

Each `.ino` folder is a self-contained Arduino sketch. Using Arduino IDE (or `arduino-cli`):

1. Install board support for **ESP32** (Boards Manager → `esp32` by Espressif, tested against Arduino core 3.3.0 per the code comments).
2. Install libraries: `ESPAsyncWebServer`, `AsyncTCP`, `ArduinoJson`.
3. For `Code.ino` / `Code_ESPNOW.ino`: edit the `WIFI_SSID` / `WIFI_PASS` constants near the top, and upload the `data/` folder to LittleFS (Arduino IDE → **ESP32 Sketch Data Upload** tool, or `arduino-cli` LittleFS upload) so the dashboard files are available on the device.
4. For `Slave_Initiator.ino`: set `broadcastAddress[]` to the hub ESP32's MAC address (find it by running any sketch with `WiFi.macAddress()` printed to Serial).
5. Flash each `.ino` to its respective ESP32 board and power on — the hub will start its own WiFi AP/STA (per your SSID config) and print its IP to Serial; connect and open that IP in a browser for the dashboard.

---

## Mechanical design (`Design/`)

SolidWorks assemblies and parts for the physical enclosure:
- **Hub enclosure** — `Hub_enclosure`, `Hub_top_cover`, `Hub_bottom_cover` (and v2 iterations)
- **Ceiling-mounted sensor unit** — `Ceilling_mount`, `Chamber`, `Chamber_top`, `Sensor_extension`
- **Battery system** — `18650 Battery Holder v5`, `1s_Liion`, `Porta baterias 2x18650`
- **`New Hub/`** — a newer revision of the hub assembly with an integrated on/off switch and circuit mount
- `final_product*` — full assembled product, with multiple STL export versions (v1–v3) reflecting design iterations

STL files are ready for 3D printing; `.SLDPRT`/`.SLDASM` are the editable SolidWorks source files (requires SolidWorks to open); `.step`/`.stp` files are neutral-format exports usable in other CAD tools.

---

## Photos & video

See `IMG_6471.JPG` through `IMG_6738.JPG` and `IMG_6398.MOV` for photos and a video of the assembled physical prototype.
