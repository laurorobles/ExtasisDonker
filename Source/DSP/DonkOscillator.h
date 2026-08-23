#pragma once
#include <juce_core/juce_core.h>
#include <cmath>

namespace ExtasisDSP
{

class DonkOscillator
{
public:
    DonkOscillator() = default;

    void prepare(double sampleRate)
    {
        currentSampleRate = sampleRate > 0.0 ? sampleRate : 44100.0;
        reset();
    }

    void reset()
    {
        carrierPhase = 0.0f;
        modPhase = 0.0f;
        subPhase = 0.0f;
        prevModOutput = 0.0f;
    }

    static inline float evaluateWaveform(float phase, float waveMorph)
    {
        // Wrap phase to [0, 2pi]
        float normPhase = std::fmod(phase, juce::MathConstants<float>::twoPi);
        if (normPhase < 0.0f)
            normPhase += juce::MathConstants<float>::twoPi;

        float sineVal = std::sin(normPhase);
        
        // TX81Z W5 (Half-Sine): positive half is sine, negative half is 0
        float halfSineVal = (sineVal > 0.0f) ? sineVal : 0.0f;
        
        // TX81Z W3 (Full Rectified Sine): absolute sine
        float fullRectVal = std::abs(sineVal);

        // Continuous morph: 0.0 -> Sine, 0.5 -> HalfSine (TX W5), 1.0 -> Rectified/Sawish (TX W3)
        if (waveMorph <= 0.5f)
        {
            float t = waveMorph * 2.0f;
            return (1.0f - t) * sineVal + t * (halfSineVal * 2.0f - 0.5f); // DC compensated
        }
        else
        {
            float t = (waveMorph - 0.5f) * 2.0f;
            float baseW5 = halfSineVal * 2.0f - 0.5f;
            float baseW3 = fullRectVal * 1.4f - 0.7f;
            return (1.0f - t) * baseW5 + t * baseW3;
        }
    }

    // Process one sample of FM synthesis
    // Returns pair of {carrierOutput, subOutput}
    inline std::pair<float, float> processSample(float baseFreq,
                                                 float fmRatio,
                                                 float fmAmount,
                                                 float fmEnvValue,
                                                 float feedbackAmount,
                                                 float waveMorph,
                                                 float subTone)
    {
        if (currentSampleRate <= 0.0)
            return { 0.0f, 0.0f };

        // 1. Modulator computation with feedback (TX Crunch)
        float modFreq = baseFreq * fmRatio;
        float modPhaseInc = (juce::MathConstants<float>::twoPi * modFreq) / (float)currentSampleRate;

        // Feedback calculation (classic Yamaha feedback loop)
        float fbMod = prevModOutput * feedbackAmount * 1.8f;
        float modRaw = evaluateWaveform(modPhase + fbMod, waveMorph);
        prevModOutput = modRaw;

        // Advance modulator phase
        modPhase += modPhaseInc;
        if (modPhase >= juce::MathConstants<float>::twoPi)
            modPhase -= juce::MathConstants<float>::twoPi;

        // 2. Carrier FM modulation
        // fmAmount scaled with envelope
        float modIndex = fmAmount * fmEnvValue * 8.0f; // dynamic index up to 8.0 rad
        float phaseModulation = modIndex * modRaw;

        float carrierFreq = baseFreq;
        float carrierPhaseInc = (juce::MathConstants<float>::twoPi * carrierFreq) / (float)currentSampleRate;

        float carrierSample = std::sin(carrierPhase + phaseModulation);

        // Advance carrier phase
        carrierPhase += carrierPhaseInc;
        if (carrierPhase >= juce::MathConstants<float>::twoPi)
            carrierPhase -= juce::MathConstants<float>::twoPi;

        // 3. Sub Oscillator (1 octave down, perfectly phase aligned)
        float subFreq = baseFreq * 0.5f;
        float subPhaseInc = (juce::MathConstants<float>::twoPi * subFreq) / (float)currentSampleRate;

        float pureSub = std::sin(subPhase);
        // Sub Tone: add warm saturation/harmonics
        float drivenSub = std::tanh(pureSub * (1.0f + subTone * 2.5f));
        float subSample = (1.0f - subTone * 0.4f) * pureSub + (subTone * 0.4f) * drivenSub;

        subPhase += subPhaseInc;
        if (subPhase >= juce::MathConstants<float>::twoPi)
            subPhase -= juce::MathConstants<float>::twoPi;

        return { carrierSample, subSample };
    }

private:
    double currentSampleRate = 44100.0;
    float carrierPhase = 0.0f;
    float modPhase = 0.0f;
    float subPhase = 0.0f;
    float prevModOutput = 0.0f;
};

} // namespace ExtasisDSP
