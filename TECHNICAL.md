# EXTASIS DONKER - Documentación Técnica y Arquitectura DSP ⚙️

## 1. Arquitectura del Motor de Síntesis

```
 [MIDI Input (Notes, Velocity, PitchBend, CC)]
                    │
                    ├───────────────────────────────────────────────────────┐
                    │                                                       │
        [Pitch Transient Generator]                                         │
        (Micro exponential sweep: +0..24 st, τ ≈ 12ms)                      │
                    │                                                       │
                    ▼                                                       ▼
        [Carrier Phase Accumulator] ◄────────────── [Modulator Phase Accumulator]
          (Freq = f0 * 2^(pitchEnv/12))               (Freq = f0 * ratio)
                    │                                         │
                    │ ◄──── [Phase Modulation Index] ─────────┤ (Feedback: TX Crunch)
                    │       (Mod = env * index * modWave)     │ (Waveform: W1/W5/W3)
                    ▼                                         ▼
           [Carrier Oscillator]                               │
                    │                                         │
                    ▼                                         │
         [Summing Stage] ◄──────── [Sub Oscillator] ──────────┘
                │                   (Freq = f0 * 0.5, Pure Sine + Drive)
                ▼
       [State-Variable Lowpass Filter]
                │
                ▼
    [Pre-Master Processing Stage]
    ├── 1. Erosion / DAC Ring-Modulator (2.8 kHz Sizzle)
    ├── 2. Punch Slam Transient Compressor
    ├── 3. Crossover Split at 180 Hz:
    │      ├── Low Band (<180 Hz): Sum to Strict 100% Mono
    │      └── High Band (>180 Hz): Modulated Stereo Haas/Chorus Spread
    └── 4. True Soft-Clipper with Hyperbolic Tangent Curve
                │
                ▼
        [Master Output Gain] ──► [Stereo Audio Buffer Out]
```

---

## 2. Modelado Matemático de Formas de Onda TX81Z

### 1. Onda W1 (Standard Sine)
\[
f_{W1}(\theta) = \sin(\theta)
\]

### 2. Onda W5 (Yamaha Half-Sine)
Utilizada en el operador modulador del preset C15 *LatelyBass*:
\[
f_{W5}(\theta) = \begin{cases} \sin(\theta) & \text{si } \sin(\theta) > 0 \\ 0 & \text{si } \sin(\theta) \le 0 \end{cases}
\]
*Nota de implementación:* Se compensa el desplazamiento de continua mediante escala dinámica \(2.0 \cdot f_{W5}(\theta) - 0.5\).

### 3. Onda W3 (Full-Wave Rectified)
\[
f_{W3}(\theta) = |\sin(\theta)|
\]

---

## 3. Algoritmo de Modulación de Fase con Realimentación

Para el modulador con feedback:
\[
\theta_{mod}[n] = (\theta_{mod}[n-1] + \Delta\theta_{mod} + \beta \cdot y_{mod}[n-1]) \pmod{2\pi}
\]
\[
y_{mod}[n] = \text{WaveMorph}(\theta_{mod}[n])
\]

Para la portadora:
\[
\theta_{car}[n] = (\theta_{car}[n-1] + \Delta\theta_{car}) \pmod{2\pi}
\]
\[
y_{car}[n] = \sin\left(\theta_{car}[n] + I_{FM}[n] \cdot y_{mod}[n]\right)
\]
Donde \(I_{FM}[n] = \text{FmAmount} \cdot \text{Env}_{FM}[n] \cdot 8.0\).

---

## 4. Curva del Soft-Clipper

La función de recorte suave analógico utiliza un umbral \(T = 0.7\):
\[
\text{SoftClip}(x) = \begin{cases}
x & \text{si } |x| < T \\
\text{sgn}(x) \left[ T + (1 - T) \tanh\left( \frac{|x| - T}{1 - T} \right) \right] & \text{si } |x| \ge T
\end{cases}
\]
Esta curva garantiza que la señal nunca sobrepase \(\pm 1.0\) (0 dBFS) y genera una distorsión armónica suave de saturación musical.

---

## 5. Estructura del Código

- `Source/PluginProcessor.h / .cpp`: Gestión de parámetros APVTS, bucle de audio, MIDI CCs y presets de fábrica.
- `Source/PluginEditor.h / .cpp`: Panel de interfaz gráfica estilo rack 1U con serigrafía retro y badges de MIDI CC.
- `Source/DSP/DonkSynth.h`: Coordinador de voz monofónica legato, portamento y velocidad.
- `Source/DSP/DonkOscillator.h`: Generadores de fase FM con formas de onda TX81Z y sub-oscilador.
- `Source/DSP/SnappyEnvelope.h`: Envolventes exponenciales de alta velocidad y micro pitch envelope.
- `Source/DSP/StereoProcessor.h`: Crossover a 180 Hz (Mono Lock + Top Spread), Erosion de 12 bits, Punch Slam y Soft Clipper.
- `Source/GUI/TX81ZLookAndFeel.h / .cpp`: Estilizado gráfico de perillas y botones rackmount vintage.
- `Source/GUI/TX81ZDisplay.h / .cpp`: Pantalla LCD retroiluminada neón con osciloscopio en tiempo real.
