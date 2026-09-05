# ⚡ Extasis Donker (v3.1.0)

<p align="center">
  <img src="assets/logo.png" alt="Extasis Donker Logo" width="220" />
</p>

<p align="center">
  <strong>The Ultimate FM Donk & Guaracha Bass Synthesizer Workstation</strong><br>
  <em>Inspired by the legendary Yamaha TX81Z (1987), 90s House, and Global Club Culture. Built with JUCE (C++17) for macOS & Windows.</em>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/Version-3.1.0-blue.svg" alt="Version 1.1" />
  <img src="https://img.shields.io/badge/Platform-macOS%20%7C%20Windows-lightgrey.svg" alt="Platform" />
  <img src="https://img.shields.io/badge/Format-VST3%20%7C%20AU%20%7C%20Standalone-orange.svg" alt="Format" />
  <img src="https://img.shields.io/badge/License-Commercial-green.svg" alt="License" />
</p>

<p align="center">
  <img src="docs/extasis_donker_ui.jpg" alt="Extasis Donker Interface" width="900" style="border-radius: 8px; box-shadow: 0 4px 16px rgba(0,0,0,0.4);" />
</p>

---

[![Build Extasis Donker (Windows & macOS)](https://github.com/laurorobles/ExtasisDonker/actions/workflows/build.yml/badge.svg)](https://github.com/laurorobles/ExtasisDonker/actions/workflows/build.yml)

## ✨ Introduction & Philosophy

**Extasis Donker** is a precision-engineered virtual instrument designed exclusively for one purpose: synthesizing the hardest-hitting, club-ready **DONK** and **GUARACHA** basslines. From the iconic *LatelyBass* of the late 80s and the *House Organ Basses* of the 90s, to modern *Guaracha Medallo Punch*, *Aleteo*, *Russian Hardbass*, *UK Bounce*, and *Tech House Slaps* — this plugin is the definitive toolkit for percussive electronic bass.

Standard 4- or 6-operator FM synthesizers are notoriously tedious to program; a slight tweak often destroys the harmonic structure, resulting in dissonant noise. **Extasis Donker solves this by locking every parameter into a mathematically calibrated "Sweet Spot"**. The architecture restricts you strictly to musically useful ranges. Every knob turn guarantees a snappy, phase-aligned, mix-ready bass that translates perfectly from festival line arrays to smartphone speakers. No menu diving, no filler — just pure, instantaneous low-end impact.

---

## 🌟 Comprehensive Feature Set

### 🎛️ 1. TX81Z-Modeled FM Engine
* **2-Operator Core:** Authentically models the gritty, aliasing-rich phase modulation of vintage 80s Yamaha chips.
* **Morphing Waveforms (TX WAVE):** Seamlessly blend between a Pure Sine (`W1`), the legendary Yamaha Half-Sine (`W5` - the secret to house organs), and an aggressive Full-Wave Rectified (`W3`) shape.
* **Velocity Sensitivity (VEL SENS):** Deep velocity-to-modulation routing for highly expressive, dynamic sequencing.

### ⚡ 2. Transient & Dynamics Processing
* **Pitch Drop Laser Envelope:** Ultra-fast, exponential pitch sweep (0 to +24 semitones) applied directly to the carrier. Perfect for generating physical "thuds" or classic laser zaps.
* **Auto Pump Sidechain:** Built-in, BPM-synced 1/4 note volume ducking. Instantly achieve the bouncing groove of Guaracha and UK Bounce without routing external kick drums.
* **Punch Slam Compression:** A specialized upward/downward transient compressor modeled after the famous OTT algorithm, heavily optimized to glue the attack phase of bass patches.

### 🎚️ 3. Filter & Harmonic Distortion
* **State-Variable Resonant Filter (LP FILTER):** Smooth lowpass filter capable of sweeping down to 20Hz for pure sub isolation, paired with a distinct, squelchy resonance control.
* **TX Crunch:** Accurately simulates the non-linear distortion and bit-reduction of vintage 12-bit hardware DACs.
* **Erosion Grit:** Injects a carefully tuned 2.8 kHz noise ring-modulator signal. This psychoacoustic trick ensures your sub-bass cuts through on small laptop and phone speakers.

### 🌌 4. Sub & Spatial Enhancement
* **Mono-Locked Sub Oscillator:** An independent pure sine sub-oscillator that completely bypasses the FM engine and FX, strictly locked in mono to prevent club phase cancellation. Features its own dedicated harmonic saturation (`SUB TONE`).
* **Top Spread (Dimension):** Modulated stereo chorus and widening that is *exclusively* applied to frequencies above 180 Hz. Widens the metallic "clank" of the donk while keeping the low-end perfectly mono and punchy.

### 🖥️ 5. Modern UX & Workflow
* **Fully Resizable UI:** Drag to scale the interface from compact setups to 4K displays while maintaining the perfect 700:400 aspect ratio.
* **Dynamic TX81Z LCD & Oscilloscope:** Features an 80s neon-green dot-matrix display with a real-time vector phosphor oscilloscope.
* **Hover Tooltips:** The LCD acts as a smart info-board, providing detailed technical descriptions of any parameter you hover over.
* **Native Preset Management:** Includes 40 strictly curated factory presets across multiple genres. Built-in, ultra-reliable custom preset saving via an elegant in-app overlay.
* **Extasis Audition Pad:** A tactile, clickable central logo pad that allows you to trigger notes and apply real-time vertical drag pitch transposition (±12 semitones).
* **MIDI Expressiveness:** Native Pitch Bend wheel support (±2 semitones) and Mod Wheel (CC1) mapping that dynamically opens the filter cutoff up to +200% for live performance.

---

## 🔄 Recent Updates (v3.1.0)

- **UI & UX Overhaul**: Fully resizable vector graphics. Symmetrical rack margins and millimeter-perfect header realignment.
- **Licensing Elegance**: The "ACTIVATE" button now intelligently vanishes from the main interface upon successful registration, keeping your workspace pristine.
- **Native Preset System**: Replaced OS-level dialogs with a beautiful, robust internal overlay for saving user presets (`SavePresetOverlay`).
- **Factory Expansion**: Expanded the core library to exactly 40 genre-defining patches.
- **DSP Refinements**: Introduced a surgical anti-click algorithm that completely eliminates unwanted pops on fast note retriggers and voice stealing. Carrier envelope attack curvature smoothed for tighter transients without digital artifacts.
- **MIDI Mod Wheel Integration**: CC1 now natively drives filter cutoff expansion for organic live performances.

---

## 📖 Official Documentation & Manual

For full technical specifications, architecture diagrams, and release rules, please consult the included documentation files:
- **[MANUAL.md](MANUAL.md)**
- **[TECHNICAL.md](TECHNICAL.md)**
- **[ARCHITECTURE.md](ARCHITECTURE.md)**

---

## 📥 Download Pre-Compiled Binaries

Official pre-compiled installers (**VST3**, **AU**, and **Standalone**) are available for macOS (Apple Silicon & Intel) and Windows (x64).

- **Releases**: Check the **[GitHub Releases](https://github.com/laurorobles/ExtasisDonker/releases)** or **[GitHub Actions](https://github.com/laurorobles/ExtasisDonker/actions)** tab.
- **Official Store & Music**: [extasisrecords.bandcamp.com](https://extasisrecords.bandcamp.com)

---

## 🛠️ Building from Source

### Prerequisites:
- **CMake 3.15+**
- **C++17 Compiler**: Xcode Clang (macOS) or Visual Studio 2022 / MSVC (Windows)
- **JUCE Framework** (automatically configured via CMake)

### Build Instructions:

```bash
# 1. Clone the repository
git clone https://github.com/laurorobles/ExtasisDonker.git
cd ExtasisDonker

# 2. Configure with CMake
cmake -B build -S .

# 3. Build VST3, AU, and Standalone targets
cmake --build build --config Release --parallel
```

### Automated Installation:
Run the included automated installer script for your OS:
- **macOS**: Double-click `installer/INSTALL_MAC.command` or run `./scripts/install.sh`
- **Windows**: Double-click `installer/INSTALL_WINDOWS.bat`

---

## 🔑 License Activation & Support

Extasis Donker features an offline cryptographic license verification system. 
Upon purchase from our official store or partner marketplaces, you will receive your personal 16-character license key (`EXTD-XXXX-XXXX-XXXX-XXXX`) to unlock full permanent access.

- **Purchase License**: [laurorobles.gumroad.com](http://laurorobles.gumroad.com)
- **Support & Inquiries**: Contact through Extasis Records Bandcamp.

---

## 👥 Credits & Contact

- **DSP Architecture & GUI Development**: Lauro Robles (`@laurorobles`)
- **Label & Releases**: [Extasis Records](https://extasisrecords.bandcamp.com)
