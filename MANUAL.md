# EXTASIS DONKER — User Manual 📖

Welcome to **Extasis Donker**, the definitive bass synthesizer workstation engineered specifically to deliver the punchiest, club-ready **DONK** and LatelyBass sounds (FM Bass, House Organ, Russian Hardbass, UK Bounce, Speed Garage, and Colombian Guaracha).

---

## 1. Concept and Design Philosophy

Standard FM synthesizers (such as the Yamaha DX7, Dexed, FM8, or Sytrus) feature dozens of envelopes, matrices, and operators. This extreme complexity often causes producers to get lost, resulting in harsh, out-of-tune metallic noises instead of solid, punchy bass.

**Extasis Donker** was created around the **"Sweet Spot" principle**:
- All mathematical algorithm curves and envelope ranges are strictly locked to musically functional ranges for bass.
- Controls respond quickly, predictably, and musically.
- A dedicated, strictly mono-locked sub-oscillator ensures immovable low-end punch on high-power club sound systems without stereo phase cancellation.
- All primary parameters feature labeled **MIDI CC** controls for seamless integration with DAWs and hardware MIDI controllers.
- An interactive **TX81Z-style Phosphor LCD** provides real-time oscilloscope feedback and mouseover tooltips.
- Features an **Interactive Audition / Trigger Pad** with vertical drag semitone transposition (±12 st).
- Fully resizable vector UI that adapts smoothly to high-DPI and 4K displays while maintaining its 700:400 aspect ratio.

---

## 2. Interface Overview & Navigation

The interface is structured in a classic 19" rackmount chassis divided into 4 primary functional sections, topped by the header display, preset navigation, and audition pad:

1. **Header Left**: Extasis Donker brand badge and discreet developer credits.
2. **Header Center**: Retro green/yellow phosphor dot-matrix LCD display with live oscilloscope and dynamic parameter readouts.
3. **Header Right**: Preset selector (40 factory presets), previous/next buttons, native Save Preset button (`SAVE`), and the Audition Pad.
4. **Bottom Grid (Sections 1–4)**:
   - **Section 1: FM Synthesis Core** (Donk Punch, FM Ratio, TX Wave, Vel Sens, Donk Time, Auto Pump, TX Crunch, Pitch Drop).
   - **Section 2: Filter** (LP Filter, Resonance).
   - **Section 3: Tone & FX** (Glide, Erosion, Punch Slam, Soft Clip).
   - **Section 4: Sub & Space** (Sub Gain, Sub Tone, Top Spread, Master Out).

---

## 3. Interactive Audition / Trigger Pad

Located in the top right, directly below the preset selection bar:
- **Click (Tap)**: Fires the audition note (`Note On` on press, `Note Off` on release).
- **Click & Drag (Up/Down)**: Transposes pitch in real-time from **-12 to +12 semitones** (one full octave up or down). This lets you test the donk tone across different musical keys without touching an external MIDI keyboard.
- **Mouseover / Tooltip**: The LCD screen displays the current audition note and semitone offset (e.g., `TRIGGER NOTE: C1 (+0 st)`).

---

## 4. Detailed Control Guide

### Section 1: FM Synthesis Core (The Donk Heart)
- **DONK PUNCH [CC 1]**: Sets the FM modulation intensity (Modulation Index). Increasing this knob injects aggressive metallic bite, clank, and transient snap.
- **FM RATIO [CC 14]**: Adjusts the harmonic frequency ratio of the modulator operator relative to the carrier:
  - `1.00x`: Warm, round, subby tone.
  - `2.00x`: The gold standard Donk & LatelyBass harmonic relationship.
  - `3.00x`: Hollow, aggressive UK Bounce / Makina timbre.
  - `4.00x - 6.00x`: Bright, bell-like attack ideal for Speed Garage and organ drops.
- **DONK TIME [CC 73]**: Decay time of the FM modulation envelope (5 ms to 500 ms). Short times create a dry percussive "thud"; medium times produce the classic bouncy rubber donk.
- **TX WAVE [CC 71]**: Continuous waveform morphing of the modulator operator:
  - `0% - 25%`: Pure Sine wave (`W1`) for clean FM.
  - `25% - 75%`: Yamaha **TX81Z Half-Sine (`W5`)** — the acoustic secret behind the legendary LatelyBass.
  - `75% - 100%`: Full-wave rectified / Saw-Sine (`W3`) for aggressive, hollow club attacks.
- **AUTO PUMP [CC 2]**: BPM-synced 1/4-note volume sidechain ducking. Instantly produces the pump and bounce essential for Guaracha, Aleteo, and UK Hard House without needing external kick-routing plugins.
- **VEL SENS [CC 70]**: Velocity sensitivity. Controls how dynamically incoming MIDI velocity drives the FM modulation depth and filter opening.
- **TX CRUNCH [CC 11]**: Simulates vintage 12-bit hardware DAC conversion artifacts, quantization distortion, and analog feedback saturation.
- **PITCH DROP [CC 12]**: Micro-pitch drop transient depth (0 to +24 semitones over 12 ms). Creates the physical downward laser zap and kick-punch attack.

---

### Section 2: Filter
- **LP FILTER [CC 74]**: Non-linear State-Variable Lowpass Filter sweeping from 20 Hz to 20,000 Hz.
- **RESONANCE [CC 71]**: Emphasizes the cutoff peak to create squelchy, acidic, and sharp synth textures.

---

### Section 3: Tone & FX
- **GLIDE [CC 5]**: Monophonic portamento glide time between legato notes.
- **EROSION [CC 13]**: Injects a calibrated 2.8 kHz ring-modulator digital noise signal. This psychoacoustic layer guarantees that low-end lines cut through clearly on smartphone and laptop speakers.
- **PUNCH SLAM [CC 3]**: Upward/downward OTT-style multiband transient compressor that glues the transient snap to the bass sustain.
- **SOFT CLIP [CC 23]**: Pre-master analog tape-style soft clipper at 0 dBFS that prevents harsh digital clipping while adding warm harmonic density.

---

### Section 4: Sub & Space
- **SUB GAIN [CC 20]**: Output level of the independent, pure sine sub-oscillator (-36 dB to 0 dB). **Strictly locked to 100% mono** to avoid club subwoofer phase cancellations.
- **SUB TONE [CC 21]**: Saturates the sub-oscillator with warm even-order harmonics for extra audible weight.
- **TOP SPREAD [CC 91]**: Modulated dimensional stereo chorus **applied exclusively to frequencies above 180 Hz**. Keeps the fundamental sub-bass rock-solid in mono while spreading the metallic donk clank across the stereo field.
- **MASTER OUT [CC 7]**: Global output volume (-24 dB to +6 dB).

---

## 5. MIDI Control & Real-Time Performance

- **Pitch Bend**: Supports standard pitch bend wheels with a calibrated range of **±2 semitones** (ideal for authentic pitch slides).
- **Mod Wheel (CC 1)**: Dynamically opens the lowpass filter cutoff frequency up to **+200%**, providing immediate organic expression during live performances.
- **Retrigger Anti-Click Algorithm**: An intelligent phase-alignment circuit prevents pops and clicks during rapid 1/16th and 1/32nd note retriggers.

---

## 6. Preset Management

Extasis Donker includes **40 factory presets** covering classic and modern club genres:
- **1–10 (Classic & Club)**: Blackout, Bamboo, Klubb, Medallo, Zapateo, Russian Thud, Hardbass, Pumping, Boiler Room, Tech Slap.
- **11–20 (UK & Bounce)**: Wigan, Scouse, Speed Rip, Kolbaser, PXS Donk, Makina, NRG, Ibiza, Bouncy, Pier.
- **21–30 (Vintage & Latin)**: Pulse, Subby, 1987 FM, TX Pure, Lately, Solid, Deep Guaracha, Aleteo King, Bogota, Anthem.
- **31–40 (Modern Underground)**: Laser Punch, Club Destroyer, Acid Rub, UK Garage, Nightcore, Bassline, Dirty Sine, Polarity, Resonance, Masterpiece.

### Saving Custom Presets:
Click the **SAVE** button in the header to open the native **Save Preset Overlay**. Type a name and hit **SAVE**. Your preset is saved as an `.edpreset` XML file in:
- **macOS**: `~/Library/Audio/Presets/Extasis Records/ExtasisDonker/`
- **Windows**: `%APPDATA%/Extasis Records/ExtasisDonker/`

---

## 7. Demo Mode & License Activation

- **Evaluation Mode**: In demo mode, Extasis Donker allows unrestricted creative work for **10 continuous minutes**.
- After 10 minutes, audio output will gracefully mute, and the activation screen will appear.
- **Session Reset**: Reloading the plugin resets the 10-minute evaluation timer.
- **Unlocking Full Version**: Click **ACTIVATE**, enter your official 16-character license key (`EXTD-XXXX-XXXX-XXXX-XXXX`). Once validated, the license badge disappears permanently and full unrestricted operation is enabled.

> **Get your official license key at:** [http://laurorobles.gumroad.com](http://laurorobles.gumroad.com)
