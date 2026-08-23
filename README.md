# EXTASIS DONKER ⚡
> **Dedicated FM Donk Bass Synthesizer Plugin (VST3 / AU / Standalone)**  
> *Inspired by the legendary Yamaha TX81Z (1987) & 90s Club House / Donk Bass Culture.*

![ExtasisDonker Banner](docs/extasis_donker_ui.jpg)

---

## ⚡ What is ExtasisDonker?

**ExtasisDonker** is a precision-engineered virtual synthesizer designed for one singular obsession: **the legendary DONK bass sound** (*LatelyBass*, *House Organ Bass*, *Russian Hardbass Donk*, *UK Bounce*, *Tech House Slap*).

Unlike generic 6-operator FM synths (which are notoriously tedious to program and easy to make sound dissonant or harsh), **ExtasisDonker locks every parameter inside its mathematically calibrated "Sweet Spot"**. Turn any knob and you get immediate, club-ready, punchy, bouncy bass.

---

## 🎛️ Key Features

- 🎹 **TX81Z-Modeled DSP Engine:** Features Yamaha TX81Z Waveforms (`W1 Sine`, `W5 Half-Sine`, `W3 Full-Wave Rectified`) with smooth morphing.
- ⚡ **Micro Pitch-Transient Click:** Ultra-fast exponential pitch sweeps (0–24 st in 5–20 ms) that give the iconic percussive laser attack.
- 🥊 **Pre-Master FX Suite:**
  - **`EROSION / DIRT`:** 12-bit DAC quantization & noise-ring sizzle.
  - **`PUNCH SLAM`:** Transient compressor / OTT-style dynamic snap.
  - **`SOFT CLIP`:** Analog tape & diode soft-clipper with 0dBFS ceiling.
- 🔊 **Mono-Locked Sub Bass:** Independent pure sine sub-oscillator with harmonic drive, strictly locked in mono to anchor club systems.
- 🌌 **High-Band Top Spread:** Stereo dimension & chorus applied exclusively to frequencies **>180 Hz**, eliminating phase cancellation in low frequencies.
- 📟 **Retro TX81Z LCD Display:** Authentic 80s neon green & yellow backlit dot-matrix screen with live phosphor vector oscilloscope and dynamic parameter readouts.
- 🎚️ **Full MIDI CC & DAW Automation:** Every knob is labeled with its standard MIDI CC and mapped for instant hardware controller mapping.

---

## 🎛️ MIDI CC Map & Controls

| Section | Control Name | MIDI CC | Description |
| :--- | :--- | :--- | :--- |
| **FM Core** | `DONK PUNCH` | **CC 1 / CC 13** | FM Modulation Index (Punch bite) |
| | `FM RATIO` | **CC 14** | Modulator frequency multiplier (0.5x – 8.0x) |
| | `DONK TIME` | **CC 73** | Snappy exponential decay of FM envelope (20–350 ms) |
| | `TX WAVE` | **CC 71** | Waveform morph (Sine ➔ Half-Sine W5 ➔ Rectified W3) |
| | `TIME SCALE` | **CC 12** | Overall envelope scaling factor (-100% to +100%) |
| | `VEL SENS` | **CC 11** | Dynamic velocity sensitivity |
| | `TX CRUNCH` | **CC 16** | Operator feedback loop & warm drive |
| | `CLICK / SNAP`| **CC 15** | Micro pitch transient click depth |
| **Tone** | `LP FILTER` | **CC 74** | State-Variable Lowpass filter cutoff |
| | `GLIDE` | **CC 22** | Monophonic legato portamento time |
| **Pre-Master FX**| `EROSION` | **CC 17** | 12-bit DAC digital sizzle |
| | `PUNCH SLAM` | **CC 18** | Transient slam compressor |
| | `SOFT CLIP` | **CC 23** | Analog soft-clipper toggle |
| **Sub & Space** | `SUB GAIN` | **CC 19** | Pure mono sub oscillator level |
| | `SUB TONE` | **CC 20** | Warm saturation on the sub |
| | `TOP SPREAD` | **CC 21** | Stereo chorus on highs (>180 Hz) |
| | `MASTER OUT` | **CC 7** | Master output level |

---

## 📦 Factory Presets

1. **`01. Lately 1987 (TX81Z)`** – The definitive C15 Yamaha preset with Half-Sine and 2.0x ratio.
2. **`02. Basic FM House (Robin S)`** – 90s House organ bass with snappy attack.
3. **`03. Russian Hardbass Punch`** – Hyper-aggressive transient click and high feedback crunch.
4. **`04. UK Bounce Donk`** – Ratio 3:1 with wide upper spread and heavy sub.
5. **`05. Tech House Subby`** – Modern tight groove bass with warm low-end focus.
6. **`06. Speed Garage Organ`** – Bouncy metallic organ bass for UK Garage/Bassline.
7. **`07. Metallic Monster`** – Maximum feedback, W3 wave and gritty bite.
8. **`08. Deep Afterhours`** – Smooth, filtered FM bass with velvet decay.

---

## 🛠️ Building from Source

### Prerequisites
- **CMake** 3.15 or newer
- **Clang / GCC / MSVC** with C++17 support
- macOS / Windows / Linux

### Build Steps:
```bash
# Clone the repository
git clone https://github.com/your-username/ExtasisDonker.git
cd ExtasisDonker

# Configure & Build (VST3, AU, Standalone)
cmake -B build -S .
cmake --build build --config Release -j8
```

Installed formats on macOS:
- **VST3:** `~/Library/Audio/Plug-Ins/VST3/ExtasisDonker.vst3`
- **AU:** `~/Library/Audio/Plug-Ins/Components/ExtasisDonker.component`
- **Standalone:** `build/ExtasisDonker_artefacts/Standalone/ExtasisDonker.app`

---

## 📖 Documentation
- [User Manual (MANUAL.md)](MANUAL.md)
- [Technical Architecture & DSP (TECHNICAL.md)](TECHNICAL.md)

---
*Developed with JUCE & modern C++.*
