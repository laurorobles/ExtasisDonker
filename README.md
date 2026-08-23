# EXTASIS DONKER ⚡
> **Dedicated FM Donk & Guaracha Bass Synthesizer Plugin (VST3 / AU / Standalone)**  
> *Inspired by the legendary Yamaha TX81Z (1987), 90s House, UK Bounce & Colombian Guaracha / Aleteo Bass Culture.*

![ExtasisDonker Banner](docs/extasis_donker_ui.jpg)

---

## ⚡ What is ExtasisDonker?

**ExtasisDonker** is a precision-engineered virtual synthesizer designed for one singular obsession: **the legendary DONK and GUARACHA FM bass sound** (*LatelyBass*, *House Organ Bass*, *Guaracha Medallo Punch*, *Aleteo / Zapateo*, *Russian Hardbass*, *UK Bounce*, *Tech House Slap*).

Unlike generic multi-operator FM synths, **ExtasisDonker locks every parameter inside its mathematically calibrated "Sweet Spot"**. Turn any knob and you get immediate, club-ready, punchy, bouncy bass.

---

## 🎛️ Key Features

- 🎹 **TX81Z-Modeled DSP Engine:** Features Yamaha TX81Z Waveforms (`W1 Sine`, `W5 Half-Sine`, `W3 Full-Wave Rectified`) with smooth morphing.
- ⚡ **Micro Pitch-Transient Click:** Ultra-fast exponential pitch sweeps (0–24 st in 5–20 ms) that give the iconic percussive laser attack.
- 🥊 **Pre-Master FX Suite:**
  - **`EROSION`:** 12-bit DAC quantization & 2.8 kHz noise-ring sizzle for small-speaker presence.
  - **`PUNCH SLAM`:** Transient compressor / OTT-style dynamic snap.
  - **`SOFT CLIP`:** Analog tape & diode soft-clipper with 0dBFS ceiling.
- 🔊 **Mono-Locked Sub Bass:** Independent pure sine sub-oscillator with harmonic drive, strictly locked in mono to anchor club systems.
- 🌌 **High-Band Top Spread:** Stereo dimension & chorus applied exclusively to frequencies **>180 Hz**, eliminating phase cancellation in low frequencies.
- 📟 **Retro TX81Z LCD Display:** Authentic 80s neon green & yellow backlit dot-matrix screen with live phosphor vector oscilloscope and live parameter readouts.
- 🔴 **Tactile Hardware Audition Trigger:** Minimalist LED tactile button cap with click & hold drag up/down for +/- 12 semitones real-time pitch shifting.
- 🎚️ **Full MIDI CC & DAW Automation:** Real-time live parameter value readouts directly under every knob.
- 👤 **Coded by @laurorobles** with direct Bandcamp integration.

---

## 📦 30 Factory Presets (Donk, House & Guaracha)

### 1. Classic & 90s House
- `01. Lately 1987 (TX81Z)` – The definitive C15 Yamaha preset with Half-Sine and 2.0x ratio.
- `02. Basic FM House (Robin S)` – 90s House organ bass with snappy attack.
- `03. Solid Bass Legacy` – Deep punchy solid bass foundation.
- `04. StoneBridge Organ Pluck` – Bright club-cutting organ bass pluck.
- `05. Speed Garage 1998` – Bouncy metallic organ bass for UK Garage/Bassline.
- `06. Todd Edwards Cut` – Micro-sampled cut-up FM bass flavor.

### 2. Guaracha, Aleteo & Zapateo Tribal
- `07. Guaracha Medallo Punch` – Snappy 130 BPM syncopated attack, 2.0x ratio, tight sub.
- `08. Aleteo Zapateo Knock` – Ultra snappy 50ms decay, 95% click, 40% erosion.
- `09. Tribal House Donker` – Heavy punch slam, W5 Half-Sine, 2.0x ratio.
- `10. Zapateo Laser Attack` – Maximum pitch transient snap (100%), soft clipped, bright cut.
- `11. Cartagena Sunset Bass` – Warm sub with driven FM top and smooth glide.
- `12. Morro Guaracha Bounce` – Fast legato glide, ratio 2.0x, punchy transient.

### 3. UK Bounce, Donk & Russian Hardbass
- `13. Russian Hardbass Punch` – XS Project style, 95% click, high feedback crunch.
- `14. UK Bounce Donk` – Blackout Crew style, 3.0x ratio, wide top spread.
- `15. Klubbheads Hard House` – Metallic ring, snappy 65ms decay.
- `16. Scouse House Spring` – Bouncy pitch mod, 2.5x ratio.
- `17. Donk-O-Mania Banger` – Maximum bite, OTT slam at 75%.
- `18. Gopnik Street Bass` – Aggressive 12-bit erosion, 4.0x ratio.

### 4. Modern Tech House, Bass House & Brazilian Bass
- `19. Tech House Subby` – Fisher/Chris Lake style, tight sub focus.
- `20. Bass House Growler` – W3 rectified wave, heavy crunch & soft clip.
- `21. Brazilian Slap FM` – Alok/Vintage Culture Brazilian bass bounce.
- `22. Minimal Deep Donk` – Subtle FM, deep warm sub, filtered.
- `23. Acid Donk Pluck` – Resonant LP filter, snappy decay.
- `24. Dirtybird Knock` – Sub-heavy knock with top erosion.

### 5. Underground, Garage & Experimental
- `25. Metallic Monster` – Full feedback grit, 5.0x ratio.
- `26. Deep Afterhours` – Smooth, dark, 1.0x ratio, 160ms decay.
- `27. 2-Step London Garage` – Woody FM organ bass with 20ms glide.
- `28. 12-Bit Vintage Sampler` – High erosion grit, SP1200 style crunchy top.
- `29. Sub Atomic Laser` – Extreme pitch sweep, 100% click, 85% punch.
- `30. Extasis Anthem Donk` – Ultimate signature master patch.

---

## 🛠️ Building from Source

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

## 👤 Credits
- **Concept & Development:** Coded by `@laurorobles`
- **Bandcamp:** [https://laurorobles.bandcamp.com](https://laurorobles.bandcamp.com)
