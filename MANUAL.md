# EXTASIS DONKER - Manual de Usuario 📖

Bienvenido a **ExtasisDonker**, el plugin sintetizador definitivo diseñado específicamente para crear bajos con el característico sonido **DONK** (FM Bass, LatelyBass, House Organ Bass, Russian Hardbass, UK Bounce).

---

## 1. Concepto y Filosofía de Diseño

Los sintetizadores FM generales (como DX7, Dexed, FM8, Sytrus) cuentan con decenas de envolventes y operadores que hacen muy fácil perderse y obtener timbres metálicos desafinados o ruidos ásperos. 

**ExtasisDonker** está diseñado con el principio del **"Sweet Spot"**:
- Las curvas de los algoritmos y envolventes están acotadas dentro de rangos musicalmente útiles para el bajo Donk.
- Los controles reaccionan de manera rápida e intuitiva.
- Cuenta con un subgrave dedicado en **mono estricto** para garantizar un peso sólido en sistemas de sonido de club.
- Cada perilla tiene asignado y etiquetado su control **MIDI CC** para integración inmediata con DAWs y controladores de hardware.
- Cuenta con **lectura instantánea por Mouseover (Hover)** en la pantalla LCD al pasar el ratón por encima de cualquier perilla.
- Botón de **Trigger / Audition interactivo** con desplazamiento vertical de semitonos (+/-12 st).

---

## 2. Botón de Trigger / Audition Interactivo

Ubicado en la parte superior derecha, debajo del selector de presets:
- **Click (Tap):** Dispara la nota actual (*Note On* al presionar, *Note Off* al soltar).
- **Click & Hold + Arrastrar Arriba / Abajo (Drag):** Sube o baja el tono en tiempo real de **-12 a +12 semitonos** (una octava arriba o abajo). Permite audicionar el bajo Donk en distintas notas y tonalidades sin necesidad de tocar un teclado MIDI externo.
- **Mouseover:** La pantalla LCD muestra inmediatamente la nota y el desplazamiento configurado (ej. `TRIGGER NOTE: C1 (+0 st)`).

---

## 3. Descripción Detallada de los Controles

### Sección 1: FM Synthesis Core (The Donk Heart)
- **DONK PUNCH [CC 1 / CC 13]:** Controla la intensidad de la modulación FM (Modulation Index). Aumentar este valor añade el brillo metálico y la pegada agresiva del bajo.
- **FM RATIO [CC 14]:** Ajusta la relación armónica entre el operador modulador y la portadora.
  - `1.00x`: Tono redondo y gordo.
  - `2.00x`: El ratio clásico y estándar del Donk / LatelyBass.
  - `3.00x`: Timbre hueco y agresivo tipo UK Bounce.
  - `4.00x / 5.00x`: Timbre brillante y acampanado para Speed Garage.
- **DONK TIME [CC 73]:** Duración de la caída exponencial de la modulación FM (20 ms a 350 ms). Valores cortos dan un "thud" percusivo seco; valores medios dan el "donk" clásico elástico.
- **TX WAVE [CC 71]:** Morfología continua de la forma de onda del modulador:
  - `0% - 25%`: Onda Seno pura (`W1`).
  - `25% - 75%`: Media-onda **TX81Z Half-Sine (`W5`)** (el secreto acústico del LatelyBass).
  - `75% - 100%`: Onda rectificada completa / Saw-Sine (`W3`) para ataques acampanados.
- **TIME SCALE [CC 12]:** Escala todas las envolventes simultáneamente (-100% a +100%) para ajustar la velocidad del bajo al BPM del proyecto.
- **VEL SENS [CC 11]:** Sensibilidad dinámica de la velocidad MIDI. Permite que notas tocadas con más fuerza tengan más brillo FM y apertura de filtro.
- **TX CRUNCH [CC 16]:** Cantidad de realimentación (*feedback loop*) del modulador y saturación analógica para añadir armónicos sucios de hardware de los 80s.
- **CLICK / SNAP [CC 15]:** Profundidad de la micro-envolvente de pitch transient (0 a 24 semitonos en 12 ms) para simular el golpe percusivo tipo láser al inicio de cada nota.

---

### Sección 2: Transient & Tone
- **LP FILTER [CC 74]:** Filtro Lowpass de estado variable con respuesta cálida analógica para recortar frecuencias agudas innecesarias.
- **GLIDE [CC 22]:** Tiempo de portamento monofónico legato para deslizar notas en líneas de bajo rápidas.

---

### Sección 3: Pre-Master FX & Shaping
- **EROSION [CC 17]:** Algoritmo de modulación de ruido y cuantización a 12 bits inspirado en samplers y unidades de hardware clásicas. Añade presencia crujiente en altavoces pequeños.
- **PUNCH SLAM [CC 18]:** Compresor y enfatizador de transientes estilo OTT que realza el ataque inicial del bajo y mantiene el cuerpo denso.
- **SOFT CLIP [CC 23]:** Botón conmutador con LED rojo que activa una etapa de saturación y recorte suave analógico con techo en 0 dBFS.

---

### Sección 4: Sub & Space
- **SUB GAIN [CC 19]:** Nivel de salida del oscilador subgrave seno puro independiente (-36 dB a 0 dB). **Bloqueado 100% en mono** para evitar cancelaciones de fase.
- **SUB TONE [CC 20]:** Saturador de armónicos cálidos en el subgrave para mayor definición.
- **TOP SPREAD [CC 21]:** Chorus y apertura estéreo dimensional aplicada **únicamente a frecuencias >180 Hz**.
- **MASTER OUT [CC 7]:** Ganancia general de salida del sintetizador.

---

## 4. Pantalla LCD Neon TX81Z y Mouseover Inteligente

La pantalla LCD en verde y amarillo neón reproduce el aspecto de las pantallas de matriz de puntos de los 80s:
1. **Línea Superior:** Nombre del preset actual.
2. **Línea Intermedia con Mouseover:** Al pasar el ratón por encima de cualquier perilla (incluso sin hacer clic), la pantalla muestra inmediatamente su nombre, su valor actual con unidades precisas y su código de MIDI CC.
3. **Osciloscopio Vectorial en Tiempo Real:** Monitor de fósforo que dibuja la forma de onda exacta del bajo reaccionando dinámicamente a cada nota.
