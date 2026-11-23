#pragma once
#include <juce_dsp/juce_dsp.h>
#include <cmath>
#include <vector>

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
        auto presenceCoeffs = juce::dsp::IIR::Coefficients<float>::makePeakFilter(
            currentSampleRate, 1500.0f, 1.0f, juce::Decibels::decibelsToGain(4.0f));
        presenceEqL.coefficients = presenceCoeffs;
        presenceEqR.coefficients = presenceCoeffs;

        // Sample-rate independent envelope follower coefficients for Punch Slam
        punchAttackCoeff  = 1.0f - std::exp(-1.0f / (float)(0.0005 * currentSampleRate)); // ~0.5 ms
        punchReleaseCoeff = 1.0f - std::exp(-1.0f / (float)(0.025  * currentSampleRate)); // ~25 ms

        // Dynamically size delay buffer to safely support up to 192 kHz (>50 ms)
        delayBufferSize = juce::jmax(4096, (int)(currentSampleRate * 0.06) + 64);
        delayBufferL.assign((size_t)delayBufferSize, 0.0f);
        delayBufferR.assign((size_t)delayBufferSize, 0.0f);

        reset();
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
    // 0. Base EQ (35Hz HPF + 1.5kHz Presence)
    // 1. Erosion / DAC Sizzle
    // 2. Punch / Transient Slam
    // 3. 180Hz Crossover (Sub Mono Lock + Interpolated Top Spread Chorus)
    // 4. Drive Saturation
    // 5. Dedicated Analog Soft-Clip
    inline void processSample(float& left,
                              float& right,
                              float spaceAmount,
                              float driveAmount,
                              float erosionAmount,
                              float punchSlamAmount,
                              bool enableSoftClip)
    {
        // 0. Base EQ (35Hz HPF and 1.5kHz Presence Boost)
        left  = dcBlockerL.processSample(left);
        right = dcBlockerR.processSample(right);
        left  = presenceEqL.processSample(left);
        right = presenceEqR.processSample(right);

        // 1. Erosion / 12-Bit DAC Grit (applied before crossover so highs get the metallic sizzle)
        if (erosionAmount > 0.001f)
        {
            // Modulate with high-frequency sine ring mod around 2.8 kHz
            float modSine = std::sin(erosionPhase);
            erosionPhase += (juce::MathConstants<float>::twoPi * 2800.0f) / (float)currentSampleRate;
            if (erosionPhase >= juce::MathConstants<float>::twoPi)
                erosionPhase -= juce::MathConstants<float>::twoPi;

            float sizzleL = left  * (1.0f + modSine * erosionAmount * 0.4f);
            float sizzleR = right * (1.0f + modSine * erosionAmount * 0.4f);

            // Subtle 12-bit quantization effect
            if (erosionAmount > 0.3f)
            {
                float steps = juce::jmax(16.0f, 128.0f - (erosionAmount - 0.3f) * 100.0f);
                sizzleL = std::round(sizzleL * steps) / steps;
                sizzleR = std::round(sizzleR * steps) / steps;
            }

            float wetMix = erosionAmount * 0.6f;
            left  = left  * (1.0f - wetMix) + sizzleL * wetMix;
            right = right * (1.0f - wetMix) + sizzleR * wetMix;
        }

        // 2. Punch Slam / Transient Compressor (OTT Style Snap)
        if (punchSlamAmount > 0.001f)
        {
            float inputLevel = (std::abs(left) + std::abs(right)) * 0.5f;
            float coeff = (inputLevel > envelopeFollower) ? punchAttackCoeff : punchReleaseCoeff;
            envelopeFollower += coeff * (inputLevel - envelopeFollower);

            // Boost transient peaks while compressing body
            float punchGain = 1.0f + (punchSlamAmount * 1.5f) / (1.0f + envelopeFollower * 2.0f);
            left  *= punchGain;
            right *= punchGain;
        }

        // 3. Crossover split at 180 Hz
        float lowL  = lowpassFilterL.processSample(left);
        float lowR  = lowpassFilterR.processSample(right);

        float highL = highpassFilterL.processSample(left);
        float highR = highpassFilterR.processSample(right);

        // Sub 100% Mono Lock
        float monoLow = (lowL + lowR) * 0.5f;

        // High band: Space / Stereo Chorus with Linear Interpolation
        float processedHighL = highL;
        float processedHighR = highR;

        if (spaceAmount > 0.001f && delayBufferSize > 0)
        {
            float lfo = std::sin(lfoPhase);
            lfoPhase += (juce::MathConstants<float>::twoPi * 0.8f) / (float)currentSampleRate;
            if (lfoPhase >= juce::MathConstants<float>::twoPi)
                lfoPhase -= juce::MathConstants<float>::twoPi;

            float delaySamplesL = (12.0f + lfo * 2.0f) * 0.001f * (float)currentSampleRate;
            float delaySamplesR = (21.0f - lfo * 2.0f) * 0.001f * (float)currentSampleRate;

            delaySamplesL = juce::jlimit(1.0f, (float)(delayBufferSize - 2), delaySamplesL);
            delaySamplesR = juce::jlimit(1.0f, (float)(delayBufferSize - 2), delaySamplesR);

            auto readInterpolated = [this](const std::vector<float>& buf, float delaySamples) -> float {
                float readPosFloat = (float)writePos - delaySamples;
                while (readPosFloat < 0.0f)
                    readPosFloat += (float)delayBufferSize;

                int idx0 = (int)readPosFloat % delayBufferSize;
                int idx1 = (idx0 + 1) % delayBufferSize;
                float frac = readPosFloat - std::floor(readPosFloat);
                return buf[(size_t)idx0] * (1.0f - frac) + buf[(size_t)idx1] * frac;
            };

            float delayedL = readInterpolated(delayBufferL, delaySamplesL);
            float delayedR = readInterpolated(delayBufferR, delaySamplesR);

            delayBufferL[(size_t)writePos] = highL + delayedL * 0.35f * spaceAmount;
            delayBufferR[(size_t)writePos] = highR + delayedR * 0.35f * spaceAmount;
            writePos = (writePos + 1) % delayBufferSize;

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
            float normFactor = 1.0f / std::sqrt(driveGain);
            outL = std::tanh(outL * driveGain) * normFactor;
            outR = std::tanh(outR * driveGain) * normFactor;
        }

        // 5. Dedicated Soft-Clip Stage (Tape/Diode Limiting with 0dBFS Ceiling)
        if (enableSoftClip)
        {
            auto softClipFunc = [](float x) -> float {
                constexpr float threshold = 0.7f;
                float absX = std::abs(x);
                if (absX < threshold)
                    return x;
                float sign = (x > 0.0f) ? 1.0f : -1.0f;
                return sign * (threshold + (1.0f - threshold) * std::tanh((absX - threshold) / (1.0f - threshold)));
            };

            outL = softClipFunc(outL * 1.15f);
            outR = softClipFunc(outR * 1.15f);
        }

        left  = outL;
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
    int delayBufferSize = 4096;
    int writePos = 0;
    float lfoPhase = 0.0f;
    float erosionPhase = 0.0f;
    float envelopeFollower = 0.0f;
    float punchAttackCoeff = 0.05f;
    float punchReleaseCoeff = 0.001f;
};

} // namespace ExtasisDSP
