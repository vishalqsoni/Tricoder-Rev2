# Tricorder — Rev 2

> A functional Star Trek Tricorder replica powered by the Seeed Studio XIAO ESP32S3 Sense. Features a full LCARS-style UI, multi-sensor environmental monitoring, animated GIF playback, capacitive touch navigation, WS2812B LED control, and synchronized audio — all running on a compact embedded system.

![Work In Progress](https://img.shields.io/badge/status-work%20in%20progress-orange)
![Platform](https://img.shields.io/badge/platform-ESP32--S3-blue)
![IDE](https://img.shields.io/badge/IDE-Arduino-teal)
![License](https://img.shields.io/badge/license-MIT-green)

---
<img width="3024" height="3024" alt="Tricorder Rev 2" src="https://github.com/user-attachments/assets/f29d8313-310d-4ffa-adbf-023179df9996" />

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Hardware](#hardware)
- [Repository Layout](#repository-layout)
- [How the Firmware Works](#how-the-firmware-works)
  - [Boot Sequence](#boot-sequence)
  - [Main Loop](#main-loop)
  - [Priority Model](#priority-model)
- [Code Documentation — File by File](#code-documentation--file-by-file)
  - [main.ino](#mainino--entry-point)
  - [ui.cpp / ui.h](#uicpp--uih--menu-system)
  - [mcp23008.cpp / mcp23008.h](#mcp23008cpp--mcp23008h--touch-input)
  - [bme.cpp / bme.h](#bmecpp--bmeh--environmental-sensor)
  - [ina.cpp / ina.h](#inacpp--inah--power-monitor)
  - [led.cpp / led.h](#ledcpp--ledh--ws2812b-animations)
  - [audio.cpp / audio.h](#audiocpp--audioh--jq6500-sound)
  - [alert.cpp / alert.h](#alertcpp--alerth--red-alert-mode)
  - [scanning_run.cpp / scanning_frames.h](#scanning_runcpp--scanning_framesh--scanning-animation)
  - [camera_tft.cpp / camera_tft.h](#camera_tftcpp--camera_tfth--live-camera-viewfinder)
  - [startup/](#startup--boot-animation)
  - [Graphics Assets](#graphics-assets)
- [Pinout](#pinout)
- [Touch Navigation](#touch-navigation)
- [Operating Modes](#operating-modes)
- [Getting Started](#getting-started)
- [Required Libraries](#required-libraries)
- [Design Patterns Used](#design-patterns-used)
- [Related Repositories](#related-repositories)
- [Credits](#credits)
- [License](#license)

---

## Overview

Rev 2 is a significant upgrade over the original Tricorder project, moving beyond a visual prop into a device with real functionality. The firmware drives a full interactive system — environmental sensing, power monitoring, animated UI, sound effects, and capacitive touch input — all coordinated through a modular codebase running on the XIAO ESP32S3.

Every hardware feature lives in its own module with a small, consistent API (`init` → `setActive`/`start` → `run` → getters). The main sketch (`main.ino`) only wires modules together and routes touch input — it contains almost no feature logic itself.

---

## Features

| Feature | Description |
|---|---|
| LCARS UI | Star Trek interface rendered on a 1.8" ST7735 TFT (160×128) |
| Startup Animation | Section 31 splash + frame-by-frame GIF with synchronized audio on boot |
| Environmental Monitoring | Temperature, pressure, and calculated altitude via BME280 |
| Power Monitoring | Live voltage, current, power, and battery percentage via INA219 |
| Touch Navigation | 6× TTP223 capacitive pads read through an MCP23008 I²C GPIO expander |
| LED Animations | Multi-pattern WS2812B effects on two strips, fully non-blocking |
| Audio Feedback | Click, alarm, scanning, and startup sounds via JQ6500 MP3 module |
| Scanning Mode | 15-frame full-screen animation loop with sound effect |
| Alert Mode | Typewriter-style warning text with synchronized alarm audio |
| Camera | OV2640 live viewfinder decoded from JPEG to the TFT |

---

## Hardware

| Component | Part |
|---|---|
| Microcontroller | Seeed Studio XIAO ESP32S3 Sense |
| Display | ST7735 1.8" TFT (160×128, SPI) |
| Environmental Sensor | BME280 (I²C, 0x76/0x77) |
| Power Monitor | INA219 (I²C, 0x40) |
| GPIO Expander | MCP23008 (I²C, 0x20) |
| Touch Inputs | TTP223 Capacitive Touch ×6 |
| LEDs | WS2812B 2020 — strip 1: 20 px, strip 2: 15 px |
| Audio Module | JQ6500 (UART, 9600 baud) |
| Camera | OV2640 (on XIAO ESP32S3 Sense) |
| Battery | Li-Ion 1000 mAh |
| Boost Converter | 3.7 V → 5 V |
| Interconnect | 20-pin 1 mm pitch FFC cable + connectors ×2 |

> **Note:** Schematics and PCB layout are maintained in a [separate repository](#related-repositories).

---

## Repository Layout

```
main_3/
│
├── main.ino                  # Entry point — setup, main loop, input routing
│
├── ui.cpp / ui.h             # Menu state machine + screen rendering
├── mcp23008.cpp / .h         # I²C GPIO expander driver (touch input)
├── bme.cpp / bme.h           # BME280 temperature / pressure / altitude
├── ina.cpp / ina.h           # INA219 voltage / current / power
├── led.cpp / led.h           # WS2812B animations (2 strips)
├── audio.cpp / audio.h       # JQ6500 MP3 playback over UART
├── alert.cpp / alert.h       # Red-alert mode (typewriter text)
├── scanning_run.cpp / .h     # Scanning mode animation player
├── scanning_frames.h         # Frame table for scanning animation
├── camera_tft.cpp / .h       # OV2640 camera → JPEG decode → TFT
│
├── startup/                  # Boot sequence
│   ├── startup.cpp / .h      #   splash + GIF playback
│   ├── gif_frames.cpp / .h   #   startup GIF frame table
│   └── sektion.h             #   Section 31 splash bitmap
│
├── asset_png/                # LCARS frames, backgrounds, menu art (RGB565 headers)
├── font/                     # Bitmap + custom fonts
├── scanning_gif/             # Scanning animation set 1 (15 frames)
├── scanning_gif2/            # Scanning animation set 2 (15 frames — currently used)
├── scanning_gif3/            # Scanning animation set 3 (15 frames)
├── start_gif/                # Startup GIF frames
├── gif/, led_ws/, scanning/  # Earlier module revisions kept for reference
└── LICENSE / README.md
```

All images are pre-converted to `const uint16_t` RGB565 arrays in `.h` files, so they compile straight into flash — no SD card or filesystem needed.

---

## How the Firmware Works

### Boot Sequence

```
Power on
 └── setup()
      ├── Serial (115200) for debug
      ├── Wire.begin(2, 3)          → shared I²C bus (BME280, INA219, MCP23008)
      ├── SPI.begin(7, -1, 9) @ 27 MHz → ST7735 TFT
      ├── led_init()                → both WS2812B strips, animations ON
      ├── mcp_init()                → MCP23008 all pins input + pull-ups
      ├── audio.begin()             → JQ6500 reset, volume 30, single-track loop mode
      ├── bme_init()                → forced (one-shot) mode, then set inactive
      ├── ina_init()                → initialized but inactive (power saving)
      ├── ui / scanning / alert init → each module receives the TFT pointer
      ├── audio.playStartup()       → track 5
      ├── startup(tft)              → Section 31 splash (2.4 s) + GIF (100 ms/frame)
      └── draw LCARS frame + main menu
```

### Main Loop

`loop()` is fully non-blocking (except the battery screen's small 120 ms pacing delay). Every pass:

```
loop()
 ├── led_run()                     ← always animates LEDs (if enabled)
 ├── alertRun()                    ← if alert active: draw + return (highest priority)
 ├── scanningRun()                 ← if scanning active: draw frame + return
 ├── mcp_read()                    ← one I²C read gives all 6 touch states
 │    └── for each button: edge-detect (buttonLock[]) → click sound → action
 ├── ina_setActive(...)            ← INA219 powered only on BATT screen
 ├── bme_run() + updateSensorUI()  ← only in PRESS/ALT/TEMP submenus
 └── battery screen render         ← V / I / P / % + colored bar graph
```

### Priority Model

Modes are checked in strict order each loop pass:

1. **Alert** — pre-empts everything. While `alertRun()` returns true, the rest of the loop is skipped.
2. **Scanning** — same pattern, one level down.
3. **Menu / sensors** — normal operation.

When a mode ends, a `returnedFromScan` / `returnedFromAlert` flag triggers a one-time redraw of the LCARS frame and menu, so the screen never needs a full refresh during normal operation.

---

## Code Documentation — File by File

### `main.ino` — Entry Point

**291 lines. The conductor — owns hardware setup and input routing, no feature logic.**

- **Globals:** the `Adafruit_ST7735 tft` object (CS=4, DC=5, RST=6), the `Audio` object on `Serial2` (RX=44, TX=43), a `buttonLock[6]` debounce array, and `returnedFromScan` / `returnedFromAlert` redraw flags.
- **`setup()`** initializes every module in dependency order (see [Boot Sequence](#boot-sequence)). SPI runs at 27 MHz for fast full-screen bitmap pushes. Sensors start *inactive* — they only consume power when their screen is open.
- **`loop()`** routes touch input:
  - Buttons are edge-detected: `buttonLock[i]` is set on press and cleared on release, so holding a pad fires the action exactly once.
  - Every press plays a click sound (track 2).
  - Button 3 toggles LEDs. Button 5 starts alert mode + alarm sound. Buttons 0/2 scroll the menu (with wraparound), button 1 selects.
  - Selecting **SCAN** starts scanning mode + scan audio. Selecting **PRESS/ALT/TEMP** enters a submenu and activates the BME280. Selecting **BATT** draws the split LCARS frame and activates the INA219.
  - In a submenu, pressing up or down exits back to the menu and deactivates the BME280.
- **Battery screen:** reads INA219, smooths current and power with an exponential moving average (`new = raw × 0.2 + old × 0.8`) so the numbers don't jitter, maps voltage 3.0–4.2 V to 0–100 %, and draws a bar that turns green/yellow/red at 60 %/30 % thresholds.

### `ui.cpp` / `ui.h` — Menu System

**Owns all menu state and text rendering.**

- **State:** `menuItems[] = {CAMERA, SCAN, PRESS, ALT, TEMP, BATT}`, `selectedIndex`, and `inSubMenu` — shared with `main.ino` via `extern`.
- **`ui_init(tft)`** stores a pointer to the display; every draw function guards against a null pointer.
- **`drawMenu()`** renders a 3-item carousel: previous item small cyan on top, selected item large yellow in the middle, next item small cyan below. Indices wrap with modulo, so the menu scrolls forever. It clears only the text region (`fillRect`), never the LCARS frame around it — no flicker.
- **`drawSubMenu()`** picks the background per screen: graph background for PRESS, LCARS burger frame for ALT, plain background for TEMP, plus a small `A / OK / BACK` hint bar via `drawUIOverlay()`.
- **`updateSensorUI()`** implements flicker-free live values: it remembers the last drawn value per screen and only redraws when the new reading differs by more than 0.2. Values print in green size-2 text with the proper unit (`hPa`, `m`, `C`).

### `mcp23008.cpp` / `mcp23008.h` — Touch Input

**Bare-metal I²C driver for the MCP23008 GPIO expander — no library, just three register writes.**

- **`mcp_init()`** writes `0xFF` to `IODIR` (all 8 pins input) and `0xFF` to `GPPU` (pull-ups on), address `0x20`.
- **`mcp_read()`** points the register pointer at `GPIO` (0x09) and reads one byte — a single I²C transaction returns the state of all six TTP223 pads at once. That's why the main loop can poll touch every pass without slowing down.
- **`mcp_isTouched(pin)`** is a convenience bitmask check (TTP223 outputs HIGH on touch).

### `bme.cpp` / `bme.h` — Environmental Sensor

**BME280 driver wrapper tuned for battery life.**

- **`bme_init()`** tries address `0x76`, falls back to `0x77`, then configures **forced mode** with ×1 oversampling and no filter — the sensor sleeps between measurements instead of free-running.
- **`bme_setActive(state)`** is a hard gate: when inactive, `bme_run()` returns immediately and the sensor never gets triggered.
- **`bme_run()`** rate-limits to one measurement every 2 seconds. Each cycle calls `takeForcedMeasurement()` (one-shot wake → measure → sleep), then caches temperature (°C), pressure (Pa → hPa), and altitude.
- Altitude uses the international barometric formula with sea-level reference 1013.25 hPa:

$$h = 44330 \left(1 - \left(\frac{P}{P_0}\right)^{0.1903}\right)$$

- Getters return the cached values, so the UI can read them any time without touching the I²C bus.

### `ina.cpp` / `ina.h` — Power Monitor

**Same active/run/getter pattern as the BME module.**

- **`ina_init()`** starts the Adafruit INA219 at default address `0x40`.
- **`ina_setActive(state)`** — the main loop keeps this false except on the BATT screen, so the shunt is not polled during normal use.
- **`ina_run()`** reads bus voltage (V), current (mA), and power (mW) every 200 ms, non-blocking.
- Smoothing intentionally lives in `main.ino`, not here — the module reports raw values, the UI decides presentation.

### `led.cpp` / `led.h` — WS2812B Animations

**Two strips (20 px on GPIO 8, 15 px on GPIO 1) running ~9 independent animations at once, all from `millis()` timers — zero `delay()` calls.**

Each effect keeps its own timestamp and advances only when its interval has elapsed; then both strips are pushed once at the end of `led_run()`:

- **Strip 1:** red pixel ping-pong (pixels 0↔1, 1.5 s) · blue ping-pong (2↔3, 1.7 s) · two cyan chases running toward each other (pixels 4–7 and 8–11, 120 ms) · a green "comet" with a 4-step fading tail (pixels 12–17, 120 ms) · pixels 18–19 solid yellow.
- **Strip 2:** status pixels (green/yellow) · fast red blink (500 ms) · green chase (200 ms) · yellow bounce with direction flip (150 ms) · slow blue blink (1 s) · a one-way startup sequence that steps yellow → orange → red once and latches · solid red + cyan indicators.
- **`led_setEnabled(false)`** clears both strips immediately (`led_off()`) — this is the LED toggle button, saving roughly 350 mA of battery draw.
- **`led_alert()`** floods both strips solid red for alert mode.

### `audio.cpp` / `audio.h` — JQ6500 Sound

**Small class wrapping the JQ6500 MP3 module over `Serial2` at 9600 baud.**

- **`begin()`** resets the module, sets volume to max (30), and sets `MP3_LOOP_ONE_STOP` — play one track, then stop, so sound effects don't loop forever.
- Tracks are addressed by flash index; each public method maps a name to a track number:

| Method | Track | Used for |
|---|---|---|
| `playAlarm()` | 1 | Alert mode |
| `playClick()` | 2 | Every button press |
| `playFunny()` | 3 | Funny mode |
| `playScanning()` | 4 | Scanning mode |
| `playStartup()` | 5 | Boot |

- `setVolume()` clamps to the hardware max of 30.

### `alert.cpp` / `alert.h` — Red Alert Mode

**Self-contained mode with a typewriter text effect.**

- **`alertStart()`** draws the split LCARS frame and resets the animation state; `main.ino` plays the alarm sound alongside.
- **`alertRun()`** types out three lines — `CORE INSTABILITY`, `POWER LEVEL CRITICAL`, `INITIATE CONTAINMENT` — one character every 50 ms. Completed lines stay on screen; when all three finish, the text loops. Only the text rectangle is cleared each tick, keeping the frame intact.
- The mode self-terminates after 7 s (`ALERT_DURATION`). `alertRun()` returns `true` while active — that return value is what lets `main.ino` skip the rest of the loop (priority pre-emption).

### `scanning_run.cpp` / `scanning_frames.h` — Scanning Animation

**Full-screen 15-frame animation player.**

- `scanning_frames.h` includes the 15 headers from `scanning_gif2/` and builds `scanningFrames[]` — an array of pointers to 160×128 RGB565 bitmaps. Swapping the animation set means changing one include block (`scanning_gif/` and `scanning_gif3/` are the alternate sets).
- **`scanningRun()`** draws the next frame every 40 ms (~25 fps) with `drawRGBBitmap()` — the 27 MHz SPI clock makes full-screen pushes fast enough for smooth playback — and loops the frame index. Mode self-terminates after 16 s, matching the scanning audio clip length.
- Identical state machine to alert mode: `start()` sets flags, `run()` returns `true` while active, `isScanningActive()` exposes state.

### `camera_tft.cpp` / `camera_tft.h` — Live Camera Viewfinder

**Drives the XIAO ESP32S3 Sense's onboard OV2640 and paints live video to the TFT.**

- The pin map at the top matches the Sense's camera connector (8-bit parallel DVP interface, XCLK 20 MHz).
- **`camera_start()`** configures the camera for QVGA JPEG at quality 9 with 2 frame buffers, then flips the image vertically to match the display orientation. A 200 ms settle delay before init is required for reliable startup. The camera is initialized *on demand*, not at boot — it holds several large DMA buffers, so it only exists while in use.
- **`camera_loop()`** grabs a JPEG frame, decodes it with JPEGDecoder, and blits it MCU-block by MCU-block (`drawJPEG()`) centered on the display — decoding block-wise avoids needing a full-frame RAM buffer.
- **`camera_stop()`** deinitializes the camera and frees its buffers.
- The CAMERA menu entry is present in the UI; wiring `camera_start()`/`camera_loop()` into the menu selection is the next integration step (work in progress).

### `startup/` — Boot Animation

- **`startup.cpp`** — plays the boot sequence: draws the Section 31 splash (`sektion.h`) for 2.4 s, then plays every frame of the startup GIF at 100 ms per frame. This is the only intentionally blocking animation in the firmware — nothing else needs to run during boot.
- **`gif_frames.cpp` / `.h`** — includes the frame headers from `start_gif/` and exposes `gifFrames[]` + `gifFrameCount` (computed via `sizeof`, so adding a frame automatically updates the count).
- **`sektion.h`** — the 160×128 splash bitmap.

### Graphics Assets

| Directory | Contents |
|---|---|
| `asset_png/` | LCARS UI frames (`lcarsframeblank`, `lcarssplitframe`, `lcarsburgerframe`), sensor backgrounds (`backgraph`, `background`), menu and decorative art — each a `const uint16_t` RGB565 array |
| `font/` | GFX fonts (Orbitron, Roboto Thin, Satisfy, Yellowtail, FreeSans) plus classic TFT_eSPI RLE bitmap fonts |
| `scanning_gif`, `_gif2`, `_gif3/` | Three interchangeable 15-frame scanning animation sets |
| `start_gif/` | Startup GIF frames |
| `gif/`, `led_ws/`, `scanning/` | Earlier module revisions kept for reference (not compiled) |

---

## Pinout

| Signal | GPIO |
|---|---|
| TFT CS / DC / RST | 4 / 5 / 6 |
| TFT SPI SCK / MOSI | 7 / 9 (27 MHz) |
| I²C SDA / SCL | 2 / 3 (BME280 + INA219 + MCP23008) |
| WS2812B strip 1 (20 px) | 8 |
| WS2812B strip 2 (15 px) | 1 |
| JQ6500 RX / TX | 44 / 43 (Serial2, 9600 baud) |
| Camera (OV2640) | XIAO ESP32S3 Sense internal DVP pins |

---

## Touch Navigation

Six TTP223 pads → MCP23008 → one I²C byte. Bit index = pad:

| Bit | Pad | Action |
|---|---|---|
| 0 | `BIO` | Scroll down / **Back** (in submenu) |
| 1 | `GEO` | Select / Enter |
| 2 | `MET` | Scroll up / **Back** (in submenu) |
| 3 | `E` | Toggle LEDs on/off |
| 4 | — | Reserved |
| 5 | `F1F2` | Trigger Alert Mode |

---

## Operating Modes

### Startup
Section 31 splash (2.4 s) → startup GIF with cockpit ambience (audio track 5) → main menu.

### Menu
Carousel of six entries: `CAMERA · SCAN · PRESS · ALT · TEMP · BATT`. Selected item shown large in yellow, neighbors in cyan.

### Environment Monitoring (PRESS / ALT / TEMP)
BME280 wakes up only while one of these screens is open, takes a one-shot forced measurement every 2 s, and the display updates only when the value changes by more than 0.2 — no flicker, minimal power.

### Battery (BATT)
INA219 sampled every 200 ms; current and power smoothed with an exponential moving average. Shows voltage, current, power, and a battery percentage bar (3.0 V = 0 %, 4.2 V = 100 %) that changes color at 60 % and 30 %.

### Scanning Mode
15-frame full-screen animation at ~25 fps for 16 s, with scanning audio (track 4).

### Alert Mode
Triggered from pad F1F2 at any time. Alarm audio + three warning lines typed out character by character, looping for 7 s, then automatic return to the menu.

### LED Control
Pad E toggles all WS2812B animations. Off state cuts roughly 350 mA of draw, significantly extending battery life.

### Camera (work in progress)
Live OV2640 viewfinder module is complete (`camera_tft.cpp`); menu integration is the next step.

---

## Getting Started

### 1. Clone the repository

```bash
git clone https://github.com/vishalqsoni/Tricoder-Rev2.git
cd Tricoder-Rev2
```

### 2. Install the Arduino board package

Add the ESP32 board URL in Arduino IDE preferences:

```
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
```

Then install: **Boards Manager → ESP32 by Espressif**

### 3. Select board

**Tools → Board → XIAO_ESP32S3** (enable PSRAM for the camera)

### 4. Install required libraries

See [Required Libraries](#required-libraries) below.

### 5. Flash

Open `main.ino` and upload.

---

## Required Libraries

Install via **Arduino IDE → Library Manager**:

| Library | Used by | Purpose |
|---|---|---|
| Adafruit GFX Library | display | Core graphics primitives |
| Adafruit ST7735 and ST7789 Library | display | TFT driver |
| Adafruit BME280 Library | `bme.cpp` | Environmental sensor |
| Adafruit INA219 | `ina.cpp` | Power monitoring |
| Adafruit NeoPixel | `led.cpp` | WS2812B control |
| JQ6500_Serial | `audio.cpp` | MP3 module control |
| JPEGDecoder | `camera_tft.cpp` | Camera JPEG decoding |

(`esp_camera` ships with the ESP32 board package — no separate install.)

---

## Design Patterns Used

- **Module pattern:** every feature exposes `init()` → `setActive()`/`start()` → `run()` → getters. `main.ino` never touches hardware directly except the TFT.
- **Non-blocking cooperative loop:** all animation and sensor timing uses `millis()` deltas; the only `delay()` calls are in one-time boot code.
- **Priority pre-emption via return values:** `alertRun()` / `scanningRun()` return `true` while active, and `main.ino` early-returns — a simple, explicit mode stack.
- **Edge-detected input:** `buttonLock[]` turns touch level signals into single-fire events.
- **Power gating:** BME280 (forced mode), INA219, and the camera are only active while their screen is open.
- **Partial redraws:** every screen update clears only the rectangle it owns; LCARS frames are drawn once per screen change.
- **Change-detection rendering:** sensor values redraw only when they move more than 0.2 — flicker-free without double buffering.

---

## Related Repositories

| Repository | Contents |
|---|---|
| [Tricorder Rev2 — PCB & Schematics](#) | KiCad schematic, PCB layout, BOM |
| [Tricorder Rev2 — 3D Models](#) | Fusion 360 enclosure and mechanical files |

---

## Credits

- **Firmware & System Design** — [Vishal Soni](https://github.com/vishalqsoni)
- **3D Printing Service** — [JustWay](https://justway.com)

---

## License

MIT License — see [LICENSE](LICENSE) for details.
