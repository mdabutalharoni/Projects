# Line Following Robot

A competition-grade **maze-solving line follower** built on a Raspberry Pi Pico (RP2040), featuring a 14-sensor multiplexed line array, adaptive PID control, sonar-based wall-following and obstacle avoidance, an OLED menu system, and EEPROM-backed parameter storage/calibration — no need to reflash firmware to retune the robot.

---

## What's in this folder

| Path | Contents |
|---|---|
| `LFRcode/` | Full Arduino firmware (RP2040/Pi Pico), split across multiple `.ino` files that share global state (standard multi-tab Arduino sketch structure) |
| `1000057982-01.jpeg`, `20240704_224917.jpg`, `photo_2_2025-12-28_03-31-19.jpg` | Photos of the physical robot |
| `20241218_201541.mp4`, `Speed bot run.mp4`, `WhatsApp Video 2025-03-13 at 16.05.31_999fa0d4.mp4` | Video runs of the robot on the track |

---

## Hardware overview

- **MCU:** Raspberry Pi Pico (RP2040) — uses `Wire.setSDA/setSCL` and `EEPROM.begin()`, both Pico/Earle Philhower-core-specific APIs
- **Line sensing:** 14-channel analog sensor array read through a **4-bit multiplexer** (S0–S3 address lines + single SIG analog input) — lets 14 sensors share one ADC pin
- **Obstacle/wall sensing:** 3x ultrasonic sensors (`NewPing` library) — left, front, right
- **Display:** SSD1306 128×64 OLED (I²C) for a live menu system, sensor readouts, and calibration display
- **Storage:** Onboard EEPROM (emulated flash on Pico) stores all tunable parameters and sensor calibration — persists across power cycles
- **Controls:** 3 buttons (up / select / down) for on-device menu navigation — no laptop/serial connection needed to tune or run the robot
- **Motors:** 2x DC motors via H-bridge, driven by 4 PWM pins (`LF`/`LB`/`RF`/`RB` — forward/backward pins per side, PWM-based direction control rather than a single direction pin + PWM)

---

## Firmware structure (`LFRcode/`)

This is a standard multi-tab Arduino sketch — all `.ino` files in the folder are compiled together and share the global variables declared in the main file. Here's what each tab does:

| File | Responsibility |
|---|---|
| **`LFRcode.ino`** | Main entry point — pin definitions, global state, `setup()` (init display/EEPROM/sonar), and `loop()` (button-driven menu dispatch to the other modes) |
| **`menu.ino`** | On-device scrollable OLED menu (Line Follow / Adjust / Calibration / Analog Reading / Digital Reading / Motor Test / Rotation Test) |
| **`reading.ino`** | Reads all 14 sensors through the mux, normalizes each against its calibrated min/max, applies per-sensor thresholds to get a binary line pattern, and computes a weighted average line position (`avg`) used by the PID controller. Also drives the various on-screen sensor visualizations (bar/radial/digital displays) |
| **`line_follow.ino`** | **The core control loop.** Adaptive PID line following (see below), plus a full maze-navigation state machine: T-junctions, cross intersections, left/right turns, dead-end U-turns, "inverse line" mode toggle, and finish/stop detection |
| **`sonar.ino`** | Ultrasonic obstacle detection (triggers a lane-change maneuver around an obstacle in front) and wall-following mode (keeps a fixed distance from a side wall using proportional control) |
| **`motor_drive.ino`** | Low-level motor driver (`motor(left, right)`, `brake()`, `turn_90()`) |
| **`calibration.ino`** | Runs the robot in place (spinning) for a fixed window to record min/max sensor readings, computes per-sensor thresholds, and saves them to EEPROM; also drives the OLED calibration-results display |
| **`adjustment.ino`** | On-device menu for editing every tunable parameter (speeds, PID gains, delays, wall-following parameters) directly via the buttons, saved live to EEPROM |
| **`eeprom.ino`** | Loads all calibration values and tunable parameters from EEPROM at startup |
| **`button.ino`** | Debounced button handling — short press, long press, and continuous/held-press detection, used throughout the menu system |
| **`ini_display.ino`** | Small OLED text-drawing helper used across the other files |

---

## The adaptive control algorithm

The heart of this project is in `line_follow.ino`'s straight-line-following branch — instead of fixed PID gains, **Kp and Kd scale with how far off-center the robot currently is**:

```cpp
Ap = Kpmax - Kpmin;
Ad = Kdmax - Kdmin;
error = 6.5 - avg;                      // 6.5 = center of the 14-sensor array (0-13)

Kp = Kpmin + Ap * abs(error) / 6.5;      // gain increases with error magnitude
Kd = Kdmin + Ad * abs(error) / 6.5;

PID = error * Kp + (error - last_error) * Kd + (error + last_error) * Ki;
lmotor = baseSpeed + PID;
rmotor = baseSpeed - PID;
```

In plain terms: when the robot is well-centered on the line (small error), it uses gentle gains and holds close to full base speed; as it drifts further off-center, both the proportional and derivative gains ramp up smoothly toward `Kpmax`/`Kdmax`, giving a sharper, faster correction exactly when it's needed most — rather than either being too twitchy on straight sections or too sluggish in sharp curves with a single fixed gain. Motor output is still capped (`constrain(..., -30, maxSpeed)`, where `maxSpeed = 1.5 * baseSpeed`) so corrections don't exceed a safe speed envelope.

This is the adaptive-gain approach behind the "adaptive speed control for line-following robots" work.

---

## Maze/track features handled by `line_follow.ino`

- **Line loss recovery** — spins in place toward the last known turn direction, or performs a timed U-turn if no line is reacquired
- **T-junctions and cross intersections** — detected via sensor sum thresholds (`sum` = number of active sensors) and edge-sensor patterns; direction (`side`) determines whether to treat a junction as a turn or pass straight through
- **"Inverse line" mode** — toggled automatically when both outer sensors and no center sensors are active, flips sensor polarity for tracks with inverted line color
- **Full-stop detection** — sustained full-width sensor activation beyond `stop_time` halts the robot (finish line detection)
- **Obstacle avoidance** (`sonar.ino`'s `obstacle()`) — front sonar under `obstacle_distance` triggers a lane-change maneuver: 90° turn, offset drive around the obstacle, reacquire the line, turn back
- **Wall following** (`sonar.ino`'s `wall_follow()`) — proportional control keeping a fixed distance (`wall_mid`) from a side wall using the left/right sonars, for sections without a line to follow

---

## Tunable parameters (via on-device "Adjust" menu, stored in EEPROM)

`baseSpeed`, `turn_speed`, `Kpmin`/`Kpmax`, `Kdmin`/`Kdmax`, `Ki`, `turn_brake`, `node_delay`, `u_turn_delay`, `brake_time`, `stop_time`, `turn90_delay`, `i_timer`, `capacitor_delay`, `wall_mid`, `wallp`, `wall_limit`, `wall_distance`, `obstacle_distance` — all editable live via buttons + OLED without reflashing.

---

## Dependencies (Arduino IDE)

- Raspberry Pi Pico board support (Earle Philhower `arduino-pico` core — required for `Wire.setSDA`/`setSCL` and Pico-style `EEPROM`)
- `NewPing` (ultrasonic sensors)
- `Adafruit_SSD1306` + `Adafruit_GFX` (OLED display)

## Building/flashing

Open `LFRcode.ino` in Arduino IDE with all sibling `.ino` files in the same folder (Arduino automatically compiles multi-tab sketches together), select your Pico board, and upload.

## Photos & videos

See the included photos for the physical build, and the video files for the robot completing runs on a track.
