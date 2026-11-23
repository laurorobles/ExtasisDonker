# 📋 TECHNICAL SPECIFICATIONS — EXTASIS DONKER (v4.0.0)

### 1. GENERAL INFORMATION
* **Product Name:** Extasis Donker
* **Current Version:** 4.0.0 (Free & Open Source Edition)
* **License:** GNU General Public License v3.0 (GPLv3)
* **Lead Developer:** Lauro Robles (`@laurorobles`) / Extasis Records
* **Plugin Category:** FM Bass Workstation / Synthesizer (LatelyBass, Donk, Guaracha)
* **Core Language:** C++17 (Strict ISO compliant)
* **Audio Framework:** JUCE Framework 8.x
* **Build System:** CMake 3.15+ (Ninja / Clang / MSVC / GCC)

---

### 2. AUDIO & DSP SPECIFICATIONS
* **Polyphony:** Monophonic voice allocation with intelligent Legato return and note stack buffer.
* **Sampling Rates Supported:** 44.1 kHz, 48 kHz, 88.2 kHz, 96 kHz, 176.4 kHz, 192 kHz.
* **Internal Processing:** 32-bit float with hardware flush-to-zero / denormal avoidance (`juce::ScopedNoDenormals`), cached atomic parameter pointers, and lock-free GUI telemetry (`juce::AbstractFifo`).
* **Parameter Smoothing:** Multiplicative smoothing on SVF cutoff (15 ms) and linear smoothing on Master Gain (10 ms) to prevent zipper noise.
* **Synthesis Engine:**
  - **Carrier:** Pure Sine with smart anti-click phase retrigger logic.
  - **Modulator:** Multi-waveform morphing engine (Sine W1, TX81Z Half-Sine W5, Full-Wave Rectified W3) with Yamaha-style feedback loop.
  - **FM Envelope:** Exponential Decay envelope from 5 ms to 500 ms.
  - **Pitch Transient:** Independent Exponential Pitch Drop envelope (0 to +24 semitones over 12 ms).
* **Sub-Bass Generator:** Dedicated, independent pure sine oscillator (1 octave down) strictly forced to 100% monaural summing with harmonic tanh saturation (`SUB TONE`).
* **Filters:** Non-linear Trapezoidal State-Variable Filter (`juce::dsp::StateVariableTPTFilter`) with resonant peak feedback (Q = 0.707 to 5.0).
* **Dynamics & Tone Shaping:**
  - **Base EQ:** 35 Hz high-pass DC blocker + 1.5 kHz presence peak (+4 dB, Q = 1.0).
  - **TX Crunch:** Modulator feedback loop + normalized tanh drive saturation.
  - **Erosion:** 2.8 kHz ring-modulator + 12-bit quantization grit.
  - **Punch Slam:** Sample-rate-independent upward/downward transient compressor (~0.5 ms attack / ~25 ms release).
  - **Auto Pump:** Host tempo-synced 1/4-note sidechain ducking (automatically bypassed when transport is stopped).
  - **Top Spread:** Linearly interpolated modulated stereo chorus operating exclusively above a 180 Hz crossover, dynamically sized for sample rates up to 192 kHz.
  - **Soft Clip:** Smooth tanh waveshaper with 0.7 knee threshold and 0 dBFS ceiling.

---

### 3. PERFORMANCE & LATENCY
* **Plugin Latency:** 0 samples (Zero Latency processing).
* **Real-Time Safety:** Zero heap allocations and zero mutex locks on the audio thread.
* **CPU Efficiency:** Typically <1.5% CPU per instance on modern Apple Silicon (M1/M2/M3/M4) and Intel/AMD x64 processors.

---

### 4. GRAPHICAL USER INTERFACE (GUI)
* **Architecture:** Custom vector-based JUCE rendering engine with custom `TX81ZLookAndFeel` (knobs, rubberized toggle buttons, and dark/cyan ComboBox/PopupMenu).
* **Resolution:** Dynamically scalable (350x200 to 1400x800) with locked 700:400 aspect ratio and proportional font/vector scaling.
* **High-DPI Support:** Full native Retina / 4K support on macOS, Windows, and Linux.
* **Visual Display:** Lock-free real-time phosphor oscilloscope (256-sample zero-crossing triggered), integrated vertical peak meter with peak hold, MIDI activity LED, and dynamic hover tooltips.
* **UX Features:** Double-click any knob to reset to factory default; native `SavePresetOverlay` with Return/Escape keyboard support.

---

### 5. SUPPORTED PLATFORMS & FORMATS
* **macOS:** Universal Binary (Apple Silicon ARM64 & Intel x86_64) — VST3, AU, CLAP, Standalone
* **Windows:** x64 — VST3, CLAP, Standalone (.exe)
* **Linux:** x86_64 — VST3, CLAP
