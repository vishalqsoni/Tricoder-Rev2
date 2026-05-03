<img width="3024" height="3024" alt="D898B357-BA0E-4ABB-AE65-7AF81F2D287C_1_201_a" src="https://github.com/user-attachments/assets/74bc5125-23a0-4944-839c-8b98aed30f8c" /># Tricorder — Rev 2

> A functional Star Trek Tricorder replica powered by the Seeed Studio XIAO ESP32S3. Features a full LCARS-style UI, multi-sensor environmental monitoring, animated GIF playback, capacitive touch navigation, WS2812B LED control, and synchronized audio — all running on a compact embedded system.

![Work In Progress](https://img.shields.io/badge/status-work%20in%20progress-orange)
![Platform](https://img.shields.io/badge/platform-ESP32--S3-blue)
![IDE](https://img.shields.io/badge/IDE-Arduino-teal)
![License](https://img.shields.io/badge/license-MIT-green)

---
<img width="3024" height="3024" alt="C9D3B1C0-E8B0-46D9-8A76-95833387351D_1_201_a" src="https://github.com/user-attachments/assets/f29d8313-310d-4ffa-adbf-023179df9996" />
<img width="3024" height="3024" alt="D898B357-BA0E-4ABB-AE65-7AF81F2D287C_1_201_a" src="https://github.com/user-attachments/assets/b97bac55-f910-4ba2-be66-8e120b2faad0" />
<img width="3024" height="3024" alt="0B5817C7-A3F8-4416-9027-DED657F4F8F7_1_201_a" src="https://github.com/user-attachments/assets/8cc987ec-9525-48df-8e26-ff7271fb0db2" />
<img width="3024" height="3024" alt="FD04805C-4006-4418-B474-EAECC4CE09D8_1_201_a" src="https://github.com/user-attachments/assets/3bcfa466-2c80-4e9c-91ed-f495ec9bb657" />


## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Hardware](#hardware)
- [Firmware Architecture](#firmware-architecture)
- [Firmware Workflow](#firmware-workflow)
- [Getting Started](#getting-started)
- [Required Libraries](#required-libraries)
- [Touch Navigation](#touch-navigation)
- [Operating Modes](#operating-modes)
- [Related Repositories](#related-repositories)
- [Credits](#credits)

---

## Overview

Rev 2 is a significant upgrade over the [original Tricorder project](#), moving beyond a visual prop into a device with real functionality. The firmware drives a full interactive system — environmental sensing, power monitoring, animated UI, sound effects, and capacitive touch input — all coordinated through a modular codebase running on the XIAO ESP32S3.

---

## Features

| Feature | Description |
|---|---|
| LCARS UI | Authentic Star Trek interface rendered on a 1.8" ST7735 TFT |
| Startup Animation | Frame-by-frame GIF animation with synchronized audio on boot |
| Environmental Monitoring | Real-time temperature, pressure, and altitude via BME280 |
| Power Monitoring | Voltage, current, and power consumption via INA219 |
| Touch Navigation | Capacitive touch controls via TTP223 + MCP23008 GPIO expander |
| LED Animations | Fully customizable WS2812B patterns and effects |
| Audio Feedback | Sound effects and ambient audio via JQ6500 module |
| Scanning Mode | Animated scanning visuals with sound effects |
| Alert System | On-screen warning display with synchronized alarm audio |
| Funny Mode | Star Trek audio clip with matching display graphic |
| Auto Power | Reed switch detects open/close state to power on/off |

---

## Hardware

| Component | Part |
|---|---|
| Microcontroller | Seeed Studio XIAO ESP32S3 Sense |
| Display | ST7735 1.8" TFT |
| Environmental Sensor | DFRobot BME280 (I²C) |
| Power Monitor | INA219 (I²C) |
| GPIO Expander | MCP23008 (I²C) |
| Touch Inputs | TTP223 Capacitive Touch ×6 |
| LEDs | WS2812B 2020 package |
| Audio Module | JQ6500 |
| Battery | Li-Ion 1000mAh |
| Boost Converter | 3.7V → 5V |
| Interconnect | 20-pin 1mm pitch FFC cable + connectors ×2 |

> **Note:** Schematics and PCB layout are maintained in a [separate repository](#related-repositories).

---

## Firmware Architecture

```
main4/
│
├── main4.ino                     # Entry point — initializes and coordinates all modules
│
├── core/
│   ├── ui.cpp / ui.h             # UI state machine and screen rendering
│   └── camera_tft.cpp / .h       # TFT driver abstraction
│
├── drivers/
│   ├── sensors/
│   │   ├── bme.cpp / bme.h       # BME280 temperature, pressure, altitude
│   │   └── ina.cpp / ina.h       # INA219 voltage, current, power
│   ├── input/
│   │   └── mcp23008.cpp / .h     # MCP23008 I²C GPIO expander for touch inputs
│   └── output/
│       ├── led.cpp / led.h       # WS2812B LED animations via FastLED
│       └── audio.cpp / audio.h   # JQ6500 audio playback over UART
│
├── features/
│   ├── startup/                  # Boot animation + Section 31 splash
│   ├── scanning/                 # Scanning mode visuals and audio
│   ├── alert/                    # Alert/alarm mode
│   └── funny/                    # Funny mode GIF + audio
│
├── graphics/
│   ├── asset_png/                # LCARS frames, backgrounds, UI elements
│   │   ├── lcarsburgerframe.h
│   │   ├── lcarssplitframe.h
│   │   ├── section_31.h
│   │   ├── space_ship.h
│   │   ├── mainmenu.h
│   │   └── ...
│   ├── fonts/                    # Custom bitmap and TrueType fonts
│   │   ├── Font7srle, Font16, Font32rle, Font64rle ...
│   │   └── FreeSans20pt7b.h
│   └── gif_engine/
│       ├── gif_frames.cpp / .h   # Startup GIF (31 frames)
│       ├── funny_gif/            # Funny mode frames (1–12)
│       ├── scanning_gif/         # Scanning animation set 1 (1–15)
│       ├── scanning_gif2/        # Scanning animation set 2 (1–15)
│       └── scanning_gif3/        # Scanning animation set 3 (1–15)
│
└── utils/                        # Shared helpers and constants
```

---

## Firmware Workflow

```
Boot
 └── System Initialization
      └── Startup Animation (GIF + Audio)
           └── Main Loop
                ├── Read Inputs         ← MCP23008 (TTP223 touch)
                ├── Update UI State     ← ui.cpp
                ├── Execute Feature     ← scanning / alert / env / funny
                ├── Render Display      ← camera_tft + Adafruit GFX
                ├── Update LEDs         ← FastLED (WS2812B)
                └── Play Audio          ← JQ6500 via UART
```

---

## Getting Started

### 1. Clone the repository

```bash
https://github.com/vishalqsoni/Tricoder-Rev2
cd Tricorder-rev2
```

### 2. Install the Arduino board package

Add the ESP32 board URL in Arduino IDE preferences:

```
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
```

Then install: **Boards Manager → ESP32 by Espressif**

### 3. Select board

**Tools → Board → XIAO ESP32S3**

### 4. Install required libraries

See [Required Libraries](#required-libraries) below.

### 5. Flash

Open `main4/main4.ino` and upload.

---

## Required Libraries

Install all libraries via **Arduino IDE → Library Manager**:

| Library | Purpose |
|---|---|
| Adafruit GFX Library | Core graphics rendering |
| Adafruit ST7735 and ST7789 Library | TFT display driver |
| Adafruit BME280 Library | Environmental sensor |
| Adafruit INA219 | Power monitoring |
| FastLED | WS2812B LED animations |

---

## Touch Navigation

The device uses six TTP223 capacitive touch sensors routed through an MCP23008 I²C GPIO expander.

| Touch Input | Action |
|---|---|
| `BIO` | Scroll Down |
| `GEO` | Scroll Up |
| `MET` | Select / Enter |
| `BIO + GEO` (in submenu) | Back |
| `E` | Toggle LEDs ON/OFF |
| `F1F2` | Trigger Alert Mode |

---

## Operating Modes

### Startup
Displays a Section 31 splash screen followed by a 31-frame animated GIF of a spaceship with ambient cockpit audio.

### Environment Monitoring
Reads BME280 for live temperature, pressure, and calculated altitude using the barometric formula:

$$h = 44330 \left(1 - \left(\frac{P}{P_0}\right)^{\frac{1}{5.255}}\right)$$

### Power Monitoring
Reads INA219 for real-time voltage, current draw, and power consumption — useful for observing load behavior (e.g., LEDs on vs. off).

### Scanning Mode
Plays a scanning sound effect while cycling through three sets of animated scanning GIFs, with dynamic data overlaid on screen.

### Alert Mode
Triggered via F1F2 touch. Displays an on-screen warning and plays a Star Trek-style alarm sound.

### Funny Mode
Displays a spaceship image and plays a humorous audio clip from a Star Trek scene.

### LED Control
Toggle WS2812B animations on or off via the `E` touch input. Disabling LEDs reduces current draw by approximately 350 mA, significantly extending battery life.

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
