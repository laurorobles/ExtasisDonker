#pragma once
#include <juce_core/juce_core.h>
#include <cmath>

namespace ExtasisDSP
{

class SnappyEnvelope
{
public:
    enum class State
    {
        Idle,
        Attack,
        Decay,
        Sustain,
        Release
    };

    SnappyEnvelope() = default;

    void prepare(double sampleRate)
    {
        currentSampleRate = sampleRate > 0.0 ? sampleRate : 44100.0;
        reset();
    }

    void reset()
    {
        currentLevel = 0.0f;
        state = State::Idle;
    }

    void noteOn()
    {
        state = State::Attack;
        if (attackMs <= 0.1f)
        {
            currentLevel = 1.0f;
            state = State::Decay;
        }
    }

    void noteOff()
    {
        if (state != State::Idle)
            state = State::Release;
    }

    void setParameters(float attackInMs, float decayInMs, float sustainLevel, float releaseInMs, float timeScale)
    {
        // Scale times according to timeScale factor (-100% to +100% -> 0.25x to 4.0x)
        float scale = std::pow(2.0f, timeScale * 2.0f);

        attackMs = juce::jmax(0.1f, attackInMs * scale);
        decayMs = juce::jmax(1.0f, decayInMs * scale);
        sustain = juce::jlimit(0.0f, 1.0f, sustainLevel);
        releaseMs = juce::jmax(5.0f, releaseInMs * scale);

        // Compute step multipliers per sample for exponential decay
        float attackSamples = (attackMs * 0.001f) * (float)currentSampleRate;
        attackStep = attackSamples > 1.0f ? (1.0f / attackSamples) : 1.0f;

        float decaySamples = (decayMs * 0.001f) * (float)currentSampleRate;
        // Exponential decay coefficient
        decayCoeff = std::exp(-3.5f / decaySamples);

        float releaseSamples = (releaseMs * 0.001f) * (float)currentSampleRate;
        releaseCoeff = std::exp(-3.5f / releaseSamples);
    }

    inline float getNextSample()
    {
        switch (state)
        {
            case State::Attack:
                currentLevel += attackStep;
                if (currentLevel >= 1.0f)
                {
                    currentLevel = 1.0f;
                    state = State::Decay;
                }
                break;

            case State::Decay:
                currentLevel = sustain + (currentLevel - sustain) * decayCoeff;
                if (std::abs(currentLevel - sustain) < 0.0005f)
                {
                    currentLevel = sustain;
                    state = State::Sustain;
                }
                break;

            case State::Sustain:
                currentLevel = sustain;
                break;

            case State::Release:
                currentLevel *= releaseCoeff;
                if (currentLevel < 0.0001f)
                {
                    currentLevel = 0.0f;
                    state = State::Idle;
                }
                break;

            case State::Idle:
            default:
                currentLevel = 0.0f;
                break;
        }

        return currentLevel;
    }

    bool isActive() const { return state != State::Idle; }
    float getCurrentLevel() const { return currentLevel; }

private:
    double currentSampleRate = 44100.0;
    State state = State::Idle;
    float currentLevel = 0.0f;

    float attackMs = 0.5f;
    float decayMs = 120.0f;
    float sustain = 0.0f;
    float releaseMs = 50.0f;

    float attackStep = 0.05f;
    float decayCoeff = 0.999f;
    float releaseCoeff = 0.999f;
};

// Micro pitch envelope for the transient click
class PitchTransientEnvelope
{
public:
    PitchTransientEnvelope() = default;

    void prepare(double sampleRate)
    {
        currentSampleRate = sampleRate > 0.0 ? sampleRate : 44100.0;
        currentSemitoneOffset = 0.0f;
        active = false;
    }

    void trigger(float semitoneAmount, float decayMs = 12.0f)
    {
        if (semitoneAmount <= 0.01f)
        {
            active = false;
            currentSemitoneOffset = 0.0f;
            return;
        }

        currentSemitoneOffset = semitoneAmount;
        float decaySamples = (decayMs * 0.001f) * (float)currentSampleRate;
        coeff = std::exp(-4.0f / juce::jmax(1.0f, decaySamples));
        active = true;
    }

    inline float getNextSemitoneOffset()
    {
        if (!active)
            return 0.0f;

        float out = currentSemitoneOffset;
        currentSemitoneOffset *= coeff;
        if (currentSemitoneOffset < 0.01f)
        {
            currentSemitoneOffset = 0.0f;
            active = false;
        }
        return out;
    }

private:
    double currentSampleRate = 44100.0;
    float currentSemitoneOffset = 0.0f;
    float coeff = 0.99f;
    bool active = false;
};

} // namespace ExtasisDSP
