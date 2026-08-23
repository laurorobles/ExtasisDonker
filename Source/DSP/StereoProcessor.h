#pragma once
#include <juce_dsp/juce_dsp.h>
#include <cmath>

namespace ExtasisDSP
{

class StereoProcessor
{
public:
    StereoProcessor() = default;

    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        currentSampleRate = spec.sampleRate > 0.0 ? spec.sampleRate : 44100.0;


        // Crossover filters at 180 Hz
        auto crossoverCoeffs = juce::dsp::IIR::Coefficients<float>::makeLowPass(currentSampleRate, 180.0f);
        lowpassFilterL.coefficients = crossoverCoeffs;
        lowpassFilterR.coefficients = crossoverCoeffs;

        auto highpassCoeffs = juce::dsp::IIR::Coefficients<float>::makeHighPass(currentSampleRate, 180.0f);
        highpassFilterL.coefficients = highpassCoeffs;
        highpassFilterR.coefficients = highpassCoeffs;

        // Sub 35Hz DC Blocker / High-Pass
        auto dcBlockCoeffs = juce::dsp::IIR::Coefficients<float>::makeHighPass(currentSampleRate, 35.0f);
        dcBlockerL.coefficients = dcBlockCoeffs;
        dcBlockerR.coefficients = dcBlockCoeffs;

        // 1.5kHz Presence / Click EQ Boost (+4dB, Q=1.0)
        auto presenceCoeffs = juce::dsp::IIR::Coefficients<float>::makePeakFilter(currentSampleRate, 1500.0f, 1.0f, juce::Decibels::decibelsToGain(4.0f));
        presenceEqL.coefficients = presenceCoeffs;
        presenceEqR.coefficients = presenceCoeffs;


        lowpassFilterL.reset();
        lowpassFilterR.reset();
        highpassFilterL.reset();
        highpassFilterR.reset();
        dcBlockerL.reset();
        dcBlockerR.reset();
        presenceEqL.reset();
        presenceEqR.reset();

        dcBlockerL.reset();
        dcBlockerR.reset();
        presenceEqL.reset();
        presenceEqR.reset();


        delayBufferL.resize(4096, 0.0f);
        delayBufferR.resize(4096, 0.0f);
        writePos = 0;
        lfoPhase = 0.0f;
        erosionPhase = 0.0f;
        envelopeFollower = 0.0f;
    }

    void reset()
    {

        lowpassFilterL.reset();
        lowpassFilterR.reset();
        highpassFilterL.reset();
        highpassFilterR.reset();
        dcBlockerL.reset();
        dcBlockerR.reset();
        presenceEqL.reset();
        presenceEqR.reset();

        std::fill(delayBufferL.begin(), delayBufferL.end(), 0.0f);
        std::fill(delayBufferR.begin(), delayBufferR.end(), 0.0f);
        writePos = 0;
        lfoPhase = 0.0f;
        erosionPhase = 0.0f;
        envelopeFollower = 0.0f;
    }

    // Process stereo sample pair with:
    // 1. Erosion / DAC Sizzle
    // 2. Punch / Transient Slam
    // 3. 180Hz Crossover (Sub Mono Lock + Top Spread)
    // 4. Dedicated Analog Soft-Clip
    inline void processSample(float& left,
                              float& right,
                              float spaceAmount,
                              float driveAmount,
                              float erosionAmount,
                              float punchSlamAmount,
                              bool enableSoftClip)
    {

        // 0. Base EQ (35Hz HPF and 1.5kHz Presence Boost)
        left = dcBlockerL.processSample(left);
        right = dcBlockerR.processSample(right);
        left = presenceEqL.processSample(left);
        right = presenceEqR.processSample(right);

        // 1. Erosion / 12-Bit DAC Grit (applied before crossover so highs get the metallic sizzle)

        if (erosionAmount > 0.001f)
        {
            // Modulate with high-frequency sine/noise ring mod around 2.8 kHz
            float modSine = std::sin(erosionPhase);
            erosionPhase += (juce::MathConstants<float>::twoPi * 2800.0f) / (float)currentSampleRate;
            if (erosionPhase >= juce::MathConstants<float>::twoPi)
                erosionPhase -= juce::MathConstants<float>::twoPi;

            float sizzleL = left * (1.0f + modSine * erosionAmount * 0.4f);
            float sizzleR = right * (1.0f + modSine * erosionAmount * 0.4f);

            // Subtle 12-bit quantization effect
            if (erosionAmount > 0.3f)
            {
                float steps = 128.0f - (erosionAmount - 0.3f) * 100.0f;
                sizzleL = std::round(sizzleL * steps) / steps;
                sizzleR = std::round(sizzleR * steps) / steps;
            }

            left = left * (1.0f - erosionAmount * 0.6f) + sizzleL * (erosionAmount * 0.6f);
            right = right * (1.0f - erosionAmount * 0.6f) + sizzleR * (erosionAmount * 0.6f);
        }

        // 2. Punch Slam / Transient Compressor (OTT Style Snap)
        if (punchSlamAmount > 0.001f)
        {
            float inputLevel = (std::abs(left) + std::abs(right)) * 0.5f;
            float attackCoeff = 0.05f;
            float releaseCoeff = 0.001f;

            if (inputLevel > envelopeFollower)
                envelopeFollower += attackCoeff * (inputLevel - envelopeFollower);
            else
                envelopeFollower += releaseCoeff * (inputLevel - envelopeFollower);

            // Boost transient peaks while compressing body
            float punchGain = 1.0f + (punchSlamAmount * 1.5f) / (1.0f + envelopeFollower * 2.0f);
            left *= punchGain;
            right *= punchGain;
        }

        // 3. Crossover split at 180 Hz
        float lowL = lowpassFilterL.processSample(left);
        float lowR = lowpassFilterR.processSample(right);

        float highL = highpassFilterL.processSample(left);
        float highR = highpassFilterR.processSample(right);

        // Sub 100% Mono Lock
        float monoLow = (lowL + lowR) * 0.5f;

        // High band: Reverb / Space / Stereo Chorus
        float processedHighL = highL;
        float processedHighR = highR;

        if (spaceAmount > 0.001f)
        {
            float lfo = std::sin(lfoPhase);
            lfoPhase += (juce::MathConstants<float>::twoPi * 0.8f) / (float)currentSampleRate;
            if (lfoPhase >= juce::MathConstants<float>::twoPi)
                lfoPhase -= juce::MathConstants<float>::twoPi;

            float delayMsL = 12.0f + lfo * 2.0f;
            float delayMsR = 21.0f - lfo * 2.0f;

            int delaySamplesL = (int)((delayMsL * 0.001f) * currentSampleRate) % 4000;
            int delaySamplesR = (int)((delayMsR * 0.001f) * currentSampleRate) % 4000;

            int readPosL = (writePos - delaySamplesL + 4096) % 4096;
            int readPosR = (writePos - delaySamplesR + 4096) % 4096;

            float delayedL = delayBufferL[readPosL];
            float delayedR = delayBufferR[readPosR];

            delayBufferL[writePos] = highL + delayedL * 0.35f * spaceAmount;
            delayBufferR[writePos] = highR + delayedR * 0.35f * spaceAmount;
            writePos = (writePos + 1) % 4096;

            processedHighL = highL * (1.0f - spaceAmount * 0.5f) + (delayedL - delayedR * 0.4f) * spaceAmount * 0.8f;
            processedHighR = highR * (1.0f - spaceAmount * 0.5f) + (delayedR - delayedL * 0.4f) * spaceAmount * 0.8f;
        }

        // Recombine
        float outL = monoLow + processedHighL;
        float outR = monoLow + processedHighR;

        // 4. Drive Saturation
        if (driveAmount > 0.01f)
        {
            float driveGain = 1.0f + driveAmount * 2.5f;
            outL = std::tanh(outL * driveGain) / std::sqrt(driveGain);
            outR = std::tanh(outR * driveGain) / std::sqrt(driveGain);
        }

        // 5. Dedicated Soft-Clip Stage (Tape/Diode Limiting with Ceiling)
        if (enableSoftClip)
        {
            auto softClipFunc = [](float x) -> float {
                // Smooth hyperbolic tangent soft-clip with 0dBFS ceiling
                float threshold = 0.7f;
                float absX = std::abs(x);
                if (absX < threshold)
                    return x;
                float sign = (x > 0.0f) ? 1.0f : -1.0f;
                return sign * (threshold + (1.0f - threshold) * std::tanh((absX - threshold) / (1.0f - threshold)));
            };

            outL = softClipFunc(outL * 1.15f); // 1.15x push into clipper
            outR = softClipFunc(outR * 1.15f);
        }

        left = outL;
        right = outR;
    }


private:
    double currentSampleRate = 44100.0;
    juce::dsp::IIR::Filter<float> lowpassFilterL;
    juce::dsp::IIR::Filter<float> lowpassFilterR;
    juce::dsp::IIR::Filter<float> highpassFilterL;
    juce::dsp::IIR::Filter<float> highpassFilterR;
    juce::dsp::IIR::Filter<float> dcBlockerL;
    juce::dsp::IIR::Filter<float> dcBlockerR;
    juce::dsp::IIR::Filter<float> presenceEqL;
    juce::dsp::IIR::Filter<float> presenceEqR;


    std::vector<float> delayBufferL;
    std::vector<float> delayBufferR;
    int writePos = 0;
    float lfoPhase = 0.0f;
    float erosionPhase = 0.0f;
    float envelopeFollower = 0.0f;
};

} // namespace ExtasisDSP
