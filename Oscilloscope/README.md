# Oscilloscope — ESP32 Mini Oscilloscope

A pocket-sized, single-channel digital oscilloscope built on an ESP32 with a 2.8" TFT display — samples an analog signal via the onboard ADC and renders a real-time waveform with adjustable volts/div and time/div, similar in spirit to a "Wavepeek"-style handheld scope. Includes a custom enclosure (SolidWorks) and three firmware iterations of increasing capability.

---

## Project structure

| Folder | Contents |
|---|---|
| `Code/` | Three firmware iterations (see comparison below) |
| `3D Design/` | SolidWorks enclosure — body, back panel, toggle switch, and component models (TFT module, BNC connector) |
| `Oscilloscope.jpg` | Photo of the assembled device |
| `Wavepeek_Oscilloscope.png` | Reference image (inspiration/comparison device) |
| `Comparison.png` | Comparison image (likely benchmark vs. a reference scope or signal generator) |

---

## Hardware

- **MCU:** ESP32 (analog input via ADC1 channel)
- **Display:** 2.8" TFT (SPI, driven via the `TFT_eSPI` library), 320×240, landscape
- **Input:** Analog signal on **GPIO 33** (ADC pin)
- **Controls:** Push-buttons on GPIO 14 / 27 (and GPIO 26 / 25 in the latest version) for on-device scale/hold/reset control, no PC required

| Signal | GPIO |
|---|---|
| ADC input | 33 |
| Volts/div button | 14 |
| Time/div button | 27 |
| Hold button *(v3 only)* | 26 |
| Reset button *(v3 only)* | 25 |

---

## Firmware versions — what changed between them

| Version | Adds |
|---|---|
| **`Oscilloscope_ESP32/`** (v1) | Base implementation: samples 320 points from the ADC, draws the waveform against a fixed-position axis, with buttons to cycle voltage scale (0.5–5 V/div) and time scale (50–500 µs/sample). |
| **`Oscilloscope_ESP32_2/`** (v2) | Adds a proper **grid overlay** (like a real scope's graticule), **4x oversampling per point** for noise reduction, an on-screen info bar (V/div, time/µs, measured frequency), and a **zero-crossing-based frequency counter** that estimates the input signal's frequency in real time. Also starts up showing a generated test sine wave before live sampling begins. |
| **`Oscilloscope_ESP32_3/`** (v3) | Adds **Hold** (freeze the current trace) and **Reset** (restore default V/div, time/div, and un-pause) buttons, plus a labeled "PAUSED" indicator on-screen. Slightly wider voltage-scale range (0.2–5 V/div) and simpler direct rendering (no double-buffering/averaging like v2). |

None of the three is strictly an upgrade of the others — v2 trades button-based hold/reset for frequency measurement and cleaner rendering, while v3 trades frequency measurement for pause/reset control. Worth deciding which feature set you actually want on the final build, or merging the best of both (grid + averaging + frequency count from v2, hold/reset from v3) into one sketch.

---

## Dependencies (Arduino IDE)

- ESP32 board support package
- `TFT_eSPI` library — **note:** this library requires manual pin/display configuration in its `User_Setup.h` (or a custom setup file) to match your specific TFT wiring (MOSI/SCK/CS/DC/RST pins) before it will display anything. This isn't something the `.ino` files configure themselves.

---

## Building/flashing

1. Install the `TFT_eSPI` library and configure `User_Setup.h` for your 2.8" TFT module's wiring.
2. Wire the analog signal source to GPIO 33 (through appropriate signal conditioning/attenuation — the ESP32 ADC only reads 0–3.3 V safely, so any signal outside that range needs a voltage divider or op-amp buffer first).
3. Wire push-buttons to the GPIO pins listed above, each to GND (they're configured `INPUT_PULLUP`, so no external resistor needed).
4. Flash whichever version fits your needs — `Oscilloscope_ESP32_3` is the most feature-complete for interactive use (hold/reset), `Oscilloscope_ESP32_2` if you specifically want the live frequency readout.

---

## Mechanical design (`3D Design/`)

SolidWorks parts and assembly for a handheld enclosure:
- `Oscilloscope.SLDASM` — full assembly
- `Body.STL`, `back.SLDPRT`/`.STL` — main enclosure body and back panel
- `toggle.SLDPRT` — power/mode toggle switch
- `2.8 TFT Module.stp.SLDPRT`, `BNC Connector (Male).step.SLDPRT` — reference models of the display and input connector, imported for fit-checking within the assembly

## Photos

See `Oscilloscope.jpg` for the assembled device, and `Wavepeek_Oscilloscope.png` / `Comparison.png` for reference/comparison images.
