# 📋 TECHNICAL SPECIFICATIONS — EXTASIS DONKER (v3.1.0)

### 1. GENERAL INFORMATION
* **Product Name:** Extasis Donker
* **Current Version:** 3.1.0 (Production Release)
* **Lead Developer:** Lauro Robles (`@laurorobles`) / Extasis Records
* **Plugin Category:** FM Bass Workstation / Synthesizer (LatelyBass, Donk, Guaracha)
* **Core Language:** C++17 (Strict ISO compliant)
* **Audio Framework:** JUCE Framework 8.x
* **Build System:** CMake 3.15+ (Ninja / Clang / MSVC)

---

### 2. AUDIO & DSP SPECIFICATIONS
* **Polyphony:** Monophonic voice allocation with intelligent Legato return and note stack buffer.
* **Sampling Rates Supported:** 44.1 kHz, 48 kHz, 88.2 kHz, 96 kHz, 176.4 kHz, 192 kHz.
* **Internal Processing:** 32-bit and 64-bit IEEE float with hardware flush-to-zero / denormal avoidance (`juce::ScopedNoDenormals`).
* **Synthesis Engine:**
  - **Carrier:** Pure Sine with zero-crossing retrigger logic to eliminate phase clicks on fast repetitive hits.
  - **Modulator:** Multi-waveform morphing engine (Sine W1, TX81Z Half-Sine W5, Full-Wave Rectified W3).
  - **FM Envelope:** Exponential Decay envelope from 5 ms to 500 ms.
  - **Pitch Transient:** Independent Exponential Pitch Drop envelope (0 to +24 semitones over 12 ms).
* **Sub-Bass Generator:** Dedicated, independent pure sine oscillator strictly forced to 100% monaural summing with harmonic wave-shaping saturation (`SUB TONE`).
* **Filters:** Non-linear State-Variable Filter (SVF) with resonant peak feedback and real-time coefficient interpolation.
* **Dynamics & Tone Shaping:**
  - **TX Crunch:** 12-bit hardware DAC quantizer and modulator feedback saturation.
  - **Erosion:** 2.8 kHz ring-modulator carrier noise generator.
  - **Punch Slam:** Multiband upward/downward compressor with calibrated fast attack and release constants.
  - **Auto Pump:** Host tempo-synced 1/4-note quarter-cosine sidechain ducking.
  - **Top Spread:** Stereo Dimension/Chorus delay matrix applied exclusively through a high-pass crossover filter (>180 Hz).
  - **Soft Clip:** Symmetric polynomial tanh waveshaper with ceiling clamped at 0 dBFS.

---

### 3. PERFORMANCE & LATENCY
* **Plugin Latency:** 0 samples (Zero Latency processing suitable for real-time live performance and tracking).
* **CPU Efficiency:** Multi-core vector-friendly DSP loops, typically <1.5% CPU per instance on modern Apple Silicon (M1/M2/M3) and Intel Core i7/i9 processors.
* **Memory Footprint:** <45 MB RAM per loaded instance.

---

### 4. GRAPHICAL USER INTERFACE (GUI)
* **Architecture:** Custom vector-based JUCE rendering engine with zero third-party bitmap dependencies.
* **Resolution:** Dynamically scalable (350x200 to 1400x800) with locked 700:400 aspect ratio.
* **High-DPI Support:** Full native Retina / 4K support on macOS and Windows with vector anti-aliased geometry.
* **Visual Display:** Real-time phosphor vector oscilloscope (128-sample FIFO buffer with positive zero-crossing trigger).
* **Dynamic Hover Readout:** Intelligent tooltips and parameter value readout in physical units (%, Hz, ms, dB) on mouseover.

---

### 5. SUPPORTED PLATFORMS & FORMATS
* **macOS:**
  - Architectures: Universal Binary (Apple Silicon ARM64 & Intel x86_64)
  - Formats: VST3, Audio Unit (AU v2), CLAP, Standalone Application
  - Compatibility: macOS 10.13 High Sierra through macOS 15 Sequoia
* **Windows:**
  - Architecture: x64
  - Formats: VST3, CLAP, Standalone Application (.exe)
  - Compatibility: Windows 10 (64-bit) & Windows 11
* **Linux:**
  - Architecture: x86_64
  - Formats: VST3, CLAP
  - Compatibility: Ubuntu 20.04+, Debian 11+, Arch Linux

---

### 6. LICENSE & EVALUATION SYSTEM
* **Cryptographic Engine:** Custom salted 64-bit validation algorithm (`SALT_1: 0xB894E28F4A6C39D7ULL`, `SALT_2: 0x7F5D81C32E96B44BULL`).
* **Evaluation Timeout:** 10 minutes of uninterrupted full audio processing. Upon expiration, audio gracefully mutes and the registration overlay is presented. Unloading and reloading the plugin instance resets the evaluation timer.
* **License Key Format:** 16-character hexadecimal serial (`EXTD-XXXX-XXXX-XXXX-XXXX`).

> **Official Licensing:** [http://laurorobles.gumroad.com](http://laurorobles.gumroad.com)
