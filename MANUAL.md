# EXTASIS DONKER — User Manual (v4.0.0) 📖

Welcome to **Extasis Donker**, the **Free & Open Source (GPLv3)** bass synthesizer workstation engineered specifically to deliver the punchiest, club-ready **DONK** and LatelyBass sounds (FM Bass, House Organ, Russian Hardbass, UK Bounce, Speed Garage, and Colombian Guaracha).

---

## 1. Concept and Design Philosophy

Standard FM synthesizers (such as the Yamaha DX7, Dexed, FM8, or Sytrus) feature dozens of envelopes, matrices, and operators. This extreme complexity often causes producers to get lost, resulting in harsh, out-of-tune metallic noises instead of solid, punchy bass.

**Extasis Donker** was created around the **"Sweet Spot" principle**:
- All mathematical algorithm curves and envelope ranges are strictly locked to musically functional ranges for bass.
- Controls respond quickly, predictably, and musically. Double-click any knob to reset it to its default value.
- A dedicated, strictly mono-locked sub-oscillator ensures immovable low-end punch on high-power club sound systems without stereo phase cancellation.
- All primary parameters feature labeled **MIDI CC** controls for seamless integration with DAWs and hardware MIDI controllers.
- An interactive **TX81Z-style Phosphor LCD** provides a real-time lock-free oscilloscope, output peak meter, MIDI activity LED, and mouseover tooltips.
- Features an **Interactive Audition / Trigger Pad** with vertical drag semitone transposition (±12 st).
- Fully resizable vector UI (350x200 to 1400x800) with locked 700:400 aspect ratio.

---

## 2. Interface Overview & Navigation

The interface is structured in a classic 19" rackmount chassis divided into 4 primary functional sections, topped by the header display, preset navigation, and audition pad:

1. **Header Left**: Extasis Donker brand badge and Open Source v4.0 decal.
2. **Header Center**: Retro green/yellow phosphor dot-matrix LCD display with live oscilloscope, integrated vertical peak meter, MIDI activity LED, and dynamic parameter readouts.
3. **Header Right**: Preset selector (40 factory presets + user presets), previous/next buttons (`<` / `>`), native Save Preset button (`SAVE`), and the Audition Pad.
4. **Bottom Grid (Sections 1–4)**:
   - **Section 1: FM Synthesis Core** (Donk Punch, FM Ratio, TX Wave, Vel Sens, Donk Time, Auto Pump, TX Crunch, Pitch Drop).
   - **Section 2: Filter** (LP Filter, Resonance).
   - **Section 3: Tone & FX** (Glide, Erosion, Punch Slam, Soft Clip).
   - **Section 4: Sub & Space** (Sub Gain, Sub Tone, Top Spread, Master Out).

---

## 3. Interactive Audition / Trigger Pad

Located in the top right, directly below the preset selection bar:
- **Click (Tap)**: Fires the audition note (`Note On` on press, `Note Off` on release).
- **Click & Drag (Up/Down)**: Transposes pitch in real-time from **-12 to +12 semitones** (one full octave up or down). Test the donk tone across different keys without touching an external MIDI keyboard.
- **Mouseover / Tooltip**: The LCD screen displays the current audition note and semitone offset (e.g., `EXTASIS TRIGGER [C1 / +0 st]`).

---

## 4. Detailed Control Guide & MIDI CC Map

### Section 1: FM Synthesis Core (The Donk Heart)
- **DONK PUNCH [CC 13]**: Sets the FM modulation intensity (Modulation Index). Increasing this knob injects aggressive metallic bite, clank, and transient snap.
- **FM RATIO [CC 14]**: Adjusts the harmonic frequency ratio of the modulator operator relative to the carrier (`0.5x` to `8.0x`):
  - `1.00x`: Warm, round, subby tone.
  - `2.00x`: The gold standard Donk & LatelyBass harmonic relationship.
  - `3.00x`: Hollow, aggressive UK Bounce / Guaracha timbre.
  - `4.00x - 6.00x`: Bright, bell-like attack ideal for Speed Garage and organ drops.
- **TX WAVE [CC 71]**: Continuous waveform morphing of the modulator operator:
  - `0% - 25%`: Pure Sine wave (`W1`) for clean FM.
  - `25% - 75%`: Yamaha **TX81Z Half-Sine (`W5`)** — the acoustic secret behind the legendary LatelyBass.
  - `75% - 100%`: Full-wave rectified (`W3`) for aggressive, hollow club attacks.
- **VEL SENS [CC 11]**: Velocity sensitivity. Controls how dynamically incoming MIDI velocity drives the FM modulation depth and filter opening.
- **DONK TIME [CC 73]**: Decay time of the FM modulation envelope (5 ms to 500 ms). Short times create a dry percussive "thud"; medium times produce the classic bouncy rubber donk.
- **AUTO PUMP [CC 12]**: BPM-synced 1/4-note volume sidechain ducking. Automatically active while DAW transport is playing, and bypassed when stopped so you can audition presets freely.
- **TX CRUNCH [CC 16]**: Modulator feedback loop and analog drive saturation for gritty 80s hardware character.
- **PITCH DROP [CC 15]**: Micro-pitch drop transient depth (0 to +24 semitones over 12 ms). Creates the physical downward laser zap and kick-punch attack.

---

### Section 2: Filter
- **LP FILTER [CC 74]**: Non-linear State-Variable Lowpass Filter sweeping from 50 Hz to 20,000 Hz (smoothed for zipper-free automation).
- **RESONANCE [CC 75]**: Emphasizes the cutoff peak to create squelchy, acidic, and sharp synth textures.

---

### Section 3: Tone & FX
- **GLIDE [CC 22]**: Monophonic portamento glide time (0 to 200 ms) between legato notes.
- **EROSION [CC 17]**: Injects a calibrated 2.8 kHz ring-modulator + 12-bit quantization layer so low-end lines cut through clearly on smartphone and laptop speakers.
- **PUNCH SLAM [CC 18]**: Sample-rate independent upward/downward OTT-style transient compressor that glues the transient snap to the bass sustain.
- **SOFT CLIP [CC 23]**: Pre-master analog tape-style soft clipper at 0 dBFS that prevents harsh digital clipping while adding warm harmonic density.

---

### Section 4: Sub & Space
- **SUB GAIN [CC 19]**: Output level of the independent, pure sine sub-oscillator (-36 dB to 0 dB). **Strictly locked to 100% mono** to avoid club subwoofer phase cancellations.
- **SUB TONE [CC 20]**: Saturates the sub-oscillator with warm harmonics for extra audible weight.
- **TOP SPREAD [CC 21]**: Linearly interpolated modulated dimensional stereo chorus **applied exclusively to frequencies above 180 Hz**. Keeps the fundamental sub-bass rock-solid in mono while spreading the metallic donk clank across the stereo field.
- **MASTER OUT [CC 7]**: Global output volume (-36 dB to +6 dB).

---

## 5. MIDI Control & Real-Time Performance

- **Pitch Bend**: Standard pitch bend wheel support with a calibrated range of **±2 semitones**.
- **Mod Wheel (CC 1)**: Dynamically opens the lowpass filter cutoff frequency up to **+200%**, providing immediate organic expression during live performances.
- **Retrigger Anti-Click Algorithm**: Phase-alignment logic prevents pops and clicks during rapid 1/16th and 1/32nd note retriggers.

---

## 6. Preset Management

Extasis Donker includes **40 distinct factory presets** covering classic and modern club genres:
- **1–10 (Classic & Club)**: Blackout, Bamboo, Wigan, Kolbaser, Medallo, Robin Organ, Lately 1987, 303 Squelch, Speed Rip, Sub Punch.
- **11–20 (UK Bounce, Scouse & Hardbass)**: Bounce 90, Fumaratto, Hardbass, Scouse House, Deep Tube, Zapateo, 4-Op Classic, Square Sub, Dirty Tape, Hyper Donk.
- **21–30 (Vintage FM, Garage & Latin)**: Galop, Tech Slap, Sine Mod, Bell Top, Half Sine, Rectified, 12-Bit, Overdrive, Laser Drop, Anthem.
- **31–40 (Modern Underground & Acid)**: Laser Punch, Club Destroyer, Acid Rub, UK Garage, Nightcore, Bassline, Dirty Sine, Polarity, Resonance, Masterpiece.

### Saving Custom Presets:
Click the **SAVE** button in the header to open the native **Save Preset Overlay**. Type a name and press **Enter** (or click **SAVE**; press **Escape** to cancel). Presets are stored as `.edpreset` XML files in:
- **macOS**: `~/Library/Audio/Presets/Extasis Records/ExtasisDonker/`
- **Windows**: `%APPDATA%\Audio\Presets\Extasis Records\ExtasisDonker\`
- **Linux**: `~/.config/Audio/Presets/Extasis Records/ExtasisDonker/`

---

## 7. Open Source License

**Extasis Donker** is 100% free and open source under the **GNU General Public License v3.0 (GPLv3)**. See [LICENSE](LICENSE) for full terms.
