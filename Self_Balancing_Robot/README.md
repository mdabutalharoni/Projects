# Self-Balancing Robot

A two-wheeled, MPU6050-based self-balancing robot built on ESP32 — the classic "inverted pendulum" control problem, with a PID controller keeping the robot upright by driving two DC motors in response to tilt angle. Includes mechanical design (SolidWorks), a MATLAB/Simulink control model, several firmware iterations, and a Bluetooth remote-control variant.

---

## Project structure

| Folder | Contents |
|---|---|
| `Code/` | All firmware iterations (see breakdown below) |
| `Circuit Diagram/` | Wiring/circuit reference screenshot |
| `Simulation/stabilix.slx` | MATLAB/Simulink model of the balancing control system |
| `SolidWorks Design/` | Chassis, base plates, motor mounts, and full assembly (SolidWorks + STEP) |
| `IMG20241001034244.jpg`, `Self_Balancing_Robot.mp4` | Photo and video of the finished robot balancing |

---

## How it works

The robot uses an **MPU6050** IMU (accelerometer + gyroscope) to measure tilt angle, and a **PID controller** to convert that angle error into motor speed/direction commands, driving two DC motors (via an L298N/L293 motor driver) to counteract the tilt and keep the robot upright — the same principle as a Segway.

```
MPU6050 (tilt angle) → PID controller (error → correction) → Motor driver (L298N) → DC motors
```

---

## Firmware — project evolution

The `Code/` folder contains several sketches representing different stages of development, from basic hardware bring-up to the final closed-loop controller:

| Folder | Purpose |
|---|---|
| **`motor_drive/`** (`motor_drive.ino` + `logic.ino`) | Earliest stage — pure open-loop motor test. Verifies wiring/PWM/direction control for both motors (via `ledcSetup`/`ledcWrite`) before any sensor feedback is introduced. No balancing logic yet. |
| **`MPU6050_Calibration/`** | Standalone calibration sketch. Runs the MPU6050's DMP (Digital Motion Processor) auto-calibration routine (`CalibrateAccel`, `CalibrateGyro`) and prints the resulting offsets to Serial — run this first on a new MPU6050 board and note the printed offsets for use in `After_Calibration`. |
| **`reading/`** (`reading.ino`) | First working balancing loop. Reads raw accelerometer data (no DMP), computes pitch via `atan2`, and runs a single PID loop (`kp=40, ki=0, kd=0.1`) to drive both motors, with a small left/right speed offset to counter drift. Includes bundled `PID_v1.h` and `MPU6050_9Axis_MotionApps41.h` headers. |
| **`After_Calibration/`** | Most advanced version. Uses the MPU6050's onboard DMP for filtered orientation (Quaternion → pitch), and switches between **two PID profiles** — a normal-gain PID and a higher-gain "overshoot" PID — depending on how far the robot has tilted, for more responsive correction on larger disturbances. Requires calibration offsets from `MPU6050_Calibration/` to be plugged in. |
| **`reading_and_bluetooth/`** | Adds **Bluetooth Serial** control (`BluetoothSerial`, device name `ESP32_Balancing_Robot`) on top of the basic pitch/PID loop, so the robot can be commanded remotely (e.g. from a phone Bluetooth terminal app) rather than just self-balancing in place. |

**Recommended build order if starting fresh:** `motor_drive` (verify wiring) → `MPU6050_Calibration` (get offsets) → `After_Calibration` (main balancing firmware, using those offsets) → `reading_and_bluetooth` if remote control is wanted.

---

## Hardware

- **MCU:** ESP32
- **IMU:** MPU6050 (accelerometer + gyroscope, I²C)
- **Motor driver:** L298N (or L293-family, per `MOTOR_ENA/ENB` pin naming in `reading.ino`)
- **Motors:** 2x DC gear motors with wheels
- **Pin mapping** (from `reading.ino`, may differ slightly between sketches — check each file's `#define` block):

  | Signal | GPIO |
  |---|---|
  | Motor A PWM (`ENA`) | 14 |
  | Motor A IN1 / IN2 | 26 / 27 |
  | Motor B PWM (`ENB`) | 32 |
  | Motor B IN3 / IN4 | 33 / 25 |
  | MPU6050 interrupt | 2 |

---

## Dependencies (Arduino IDE)

- ESP32 board support package
- `Adafruit_MPU6050` + `Adafruit_Sensor` (used by `reading.ino`)
- `I2Cdev` + `MPU6050_6Axis_MotionApps20` (jrowberg's i2cdevlib — used by `MPU6050_Calibration` and `After_Calibration`)
- `PID_v1` (Brett Beauregard's Arduino PID library — used by `After_Calibration`; a copy is also bundled in `reading/PID_v1.h`)
- `BluetoothSerial` (bundled with ESP32 core — used by `reading_and_bluetooth`)

---

## Mechanical design & simulation

- **`SolidWorks Design/`** — chassis base plates (`base`, `2ndbase`, `3rdbase`), motor mounts, wheel/motor assembly (`dc_motor_with_wheel`), support pillars (`pillers`, `soporte`), and the full robot assembly (`Assem.SLDASM`). Also includes a STEP export of the L298N motor driver for reference in the CAD assembly.
- **`Simulation/stabilix.slx`** — a MATLAB/Simulink model of the balancing control system, useful for tuning/testing PID behavior analytically before deploying gains to the real robot.
- **`Circuit Diagram/`** — reference wiring screenshot for the MPU6050 + motor driver + ESP32 setup.

---

## Photo & video

See `IMG20241001034244.jpg` and `Self_Balancing_Robot.mp4` for the finished, balancing robot in action.
