#pragma once
#include <juce_dsp/juce_dsp.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include "DonkOscillator.h"
#include "SnappyEnvelope.h"
#include "StereoProcessor.h"

namespace ExtasisDSP
{

class DonkSynth
{
public:
    DonkSynth() = default;

    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        currentSampleRate = spec.sampleRate > 0.0 ? spec.sampleRate : 44100.0;

        oscillator.prepare(currentSampleRate);
        fmEnvelope.prepare(currentSampleRate);
        ampEnvelope.prepare(currentSampleRate);
        pitchTransient.prepare(currentSampleRate);
        stereoProcessor.prepare(spec);

        filter.prepare(spec);
        filter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
        filter.setResonance(0.707f);

        smoothedCutoff.reset(currentSampleRate, 0.015);
        smoothedCutoff.setCurrentAndTargetValue(16000.0f);

        smoothedMasterGain.reset(currentSampleRate, 0.010);
        smoothedMasterGain.setCurrentAndTargetValue(1.0f);

        ampEnvelope.setParameters(2.5f, 900.0f, 0.0f, 30.0f, 1.0f);
        lastFmDecayMs = -1.0f;
        lastGlideMs = -1.0f;

        currentFreq = 0.0f;
        targetFreq = 0.0f;
        glideCoeff = 0.0f;
        currentVelocity = 1.0f;
        activeMidiNote = -1;
        tailSamplesRemaining = 0;
    }

    void reset()
    {
        oscillator.reset();
        fmEnvelope.reset();
        ampEnvelope.reset();
        pitchTransient.prepare(currentSampleRate);
        stereoProcessor.reset();
        filter.reset();
        activeMidiNote = -1;
        pitchBendSemitones = 0.0f;
        modWheelValue = 0.0f;
        tailSamplesRemaining = 0;
        heldNotes.clear();
    }

    void handleMidiEvent(const juce::MidiMessage& msg)
    {
        if (msg.isNoteOn())
        {
            int noteNumber = msg.getNoteNumber();
            float velocity = msg.getFloatVelocity();
            heldNotes.removeAllInstancesOf(noteNumber);
            heldNotes.add(noteNumber);
            triggerNote(noteNumber, velocity);
        }
        else if (msg.isNoteOff())
        {
            int noteNumber = msg.getNoteNumber();
            heldNotes.removeAllInstancesOf(noteNumber);

            if (heldNotes.isEmpty())
            {
                ampEnvelope.noteOff();
                fmEnvelope.noteOff();
                activeMidiNote = -1;
            }
            else
            {
                // Legato return to previous held note
                int prevNote = heldNotes.getLast();
                triggerNote(prevNote, currentVelocity, true);
            }
        }
        else if (msg.isPitchWheel())
        {
            // Pitch bend: -2 to +2 semitones
            int bend = msg.getPitchWheelValue() - 8192;
            pitchBendSemitones = (float)bend / 8192.0f * 2.0f;
        }
        else if (msg.isController() && msg.getControllerNumber() == 1)
        {
            // Mod wheel (CC 1) opens lowpass filter cutoff dynamically
            modWheelValue = (float)msg.getControllerValue() / 127.0f;
        }
        else if (msg.isAllNotesOff() || msg.isAllSoundOff())
        {
            heldNotes.clear();
            ampEnvelope.reset();
            fmEnvelope.reset();
            activeMidiNote = -1;
        }
    }

    void triggerNote(int noteNumber, float velocity, bool isLegato = false)
    {
        currentVelocity = velocity;
        targetFreq = (float)juce::MidiMessage::getMidiNoteInHertz(noteNumber);

        if (!isLegato || activeMidiNote < 0)
        {
            currentFreq = targetFreq;

            // ANTI-CLICK: If the voice is already sounding loudly, DO NOT reset the phase
            // to avoid a discontinuous jump (pop). If it's silent, reset phase for max punch.
            if (ampEnvelope.getCurrentLevel() < 0.05f)
            {
                oscillator.reset();
                ampEnvelope.forceZero();
            }

            ampEnvelope.noteOn();
            fmEnvelope.noteOn();
            pitchTransient.trigger(pitchDropAmount * 24.0f, 12.0f); // up to 24 semitones snap
        }
        else
        {
            // Legato glide: retrigger pitch transient slightly for bite
            pitchTransient.trigger(pitchDropAmount * 12.0f, 8.0f);
            if (!fmEnvelope.isActive())
                fmEnvelope.noteOn();
        }

        activeMidiNote = noteNumber;
        tailSamplesRemaining = (int)(currentSampleRate * 0.25); // 250ms stereo tail allowance
    }

    void updateGlide(float glideMs)
    {
        if (std::abs(glideMs - lastGlideMs) < 0.01f)
            return;

        lastGlideMs = glideMs;
        if (glideMs <= 1.0f)
        {
            glideCoeff = 0.0f;
        }
        else
        {
            float glideSamples = (glideMs * 0.001f) * (float)currentSampleRate;
            glideCoeff = std::exp(-4.0f / juce::jmax(1.0f, glideSamples));
        }
    }

    void process(juce::AudioBuffer<float>& buffer,
                 float fmAmount,
                 float fmTuneRatio,
                 float fmEnvDecayMs,
                 float pumpAmount,
                 double bpm,
                 double ppqPosition,
                 bool isHostPlaying,
                 float modSens,
                 float wavePosition,
                 float subGainDb,
                 float subTone,
                 float txCrunch,
                 float pitchDrop,
                 float filterCutoffHz,
                 float filterResoAmount,
                 float reverbSpace,
                 float erosionAmount,
                 float punchSlamAmount,
                 bool enableSoftClip,
                 float glideMs,
                 float masterVolGain)
    {
        pitchDropAmount = pitchDrop;
        updateGlide(glideMs);

        // Update FM envelope decay only when changed
        if (std::abs(fmEnvDecayMs - lastFmDecayMs) > 0.01f)
        {
            lastFmDecayMs = fmEnvDecayMs;
            fmEnvelope.setParameters(1.5f, fmEnvDecayMs, 0.0f, 20.0f, 1.0f);
        }

        // Filter cutoff & resonance targets (smoothed per-sample below)
        float velCutoffScale = 1.0f + (currentVelocity - 0.5f) * modSens * 0.6f;
        float modWheelCutoffScale = 1.0f + (modWheelValue * 2.0f); // up to +200% cutoff via mod wheel
        float targetCutoff = juce::jlimit(20.0f, 20000.0f, filterCutoffHz * velCutoffScale * modWheelCutoffScale);
        smoothedCutoff.setTargetValue(targetCutoff);
        filter.setResonance(juce::jmap(filterResoAmount, 0.707f, 5.0f));

        smoothedMasterGain.setTargetValue(masterVolGain);

        // Sub gain linear
        float subGainLinear = subGainDb <= -35.0f ? 0.0f : juce::Decibels::decibelsToGain(subGainDb);

        // Dynamic FM Amount scaled by velocity and modSens
        float velFmScale = 1.0f + (currentVelocity - 0.5f) * modSens;
        float effectiveFmAmount = juce::jlimit(0.0f, 1.0f, fmAmount * velFmScale);

        auto* leftChannel = buffer.getWritePointer(0);
        auto* rightChannel = buffer.getNumChannels() > 1 ? buffer.getWritePointer(1) : nullptr;
        int numSamples = buffer.getNumSamples();

        double samplesPerBeat = (currentSampleRate * 60.0) / (bpm > 0.0 ? bpm : 120.0);

        for (int i = 0; i < numSamples; ++i)
        {
            float currentCutoff = smoothedCutoff.getNextValue();
            filter.setCutoffFrequency(currentCutoff);
            float currentMasterGain = smoothedMasterGain.getNextValue();

            // Pump Ducking (1/4 note sidechain, active only while host transport is playing
            // so presets remain fully audible when auditioning with transport stopped)
            float pumpMultiplier = 1.0f;
            if (isHostPlaying && pumpAmount > 0.001f)
            {
                double currentPpq = ppqPosition + ((double)i / samplesPerBeat);
                double fraction = currentPpq - std::floor(currentPpq);
                float duckCurve = 1.0f - (float)std::pow(1.0 - fraction, 3.0);
                pumpMultiplier = (1.0f - pumpAmount) + (pumpAmount * duckCurve);
            }

            // Pitch glide calculation
            if (glideCoeff > 0.0f && currentFreq > 0.0f)
                currentFreq = targetFreq + (currentFreq - targetFreq) * glideCoeff;
            else
                currentFreq = targetFreq;

            // Pitch transient modulation (click) and global pitch bend
            float pitchSnapSemitones = pitchTransient.getNextSemitoneOffset();
            float totalPitchOffset = pitchSnapSemitones + pitchBendSemitones;
            float snappedFreq = currentFreq * std::pow(2.0f, totalPitchOffset / 12.0f);

            // Envelope values
            float fmEnvVal = fmEnvelope.getNextSample();
            float ampEnvVal = ampEnvelope.getNextSample();

            float synthOut = 0.0f;
            bool voiceRunning = ampEnvelope.isActive() || activeMidiNote >= 0;

            if (voiceRunning)
            {
                auto [carrierOut, subOut] = oscillator.processSample(
                    snappedFreq,
                    fmTuneRatio,
                    effectiveFmAmount,
                    fmEnvVal,
                    txCrunch,
                    wavePosition,
                    subTone
                );
                synthOut = (carrierOut + subOut * subGainLinear) * ampEnvVal;
                tailSamplesRemaining = (int)(currentSampleRate * 0.25);
            }
            else if (tailSamplesRemaining <= 0 || reverbSpace <= 0.001f)
            {
                leftChannel[i] = 0.0f;
                if (rightChannel != nullptr)
                    rightChannel[i] = 0.0f;
                continue;
            }
            else
            {
                --tailSamplesRemaining;
            }

            // Lowpass Filter
            float filteredOut = filter.processSample(0, synthOut);

            // Stereo Pre-Master Processing (Erosion + Punch Slam + Mono Crossover + Soft-Clip)
            float leftSample = filteredOut;
            float rightSample = filteredOut;
            stereoProcessor.processSample(leftSample, rightSample, reverbSpace, txCrunch * 0.5f,
                                          erosionAmount, punchSlamAmount, enableSoftClip);

            // Master Volume with Pump Ducking
            float finalGain = currentMasterGain * pumpMultiplier;
            leftChannel[i] = leftSample * finalGain;
            if (rightChannel != nullptr)
                rightChannel[i] = rightSample * finalGain;
        }
    }

    bool isVoiceActive() const { return ampEnvelope.isActive(); }

private:
    double currentSampleRate = 44100.0;
    DonkOscillator oscillator;
    SnappyEnvelope fmEnvelope;
    SnappyEnvelope ampEnvelope;
    PitchTransientEnvelope pitchTransient;
    StereoProcessor stereoProcessor;
    juce::dsp::StateVariableTPTFilter<float> filter;

    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Multiplicative> smoothedCutoff;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> smoothedMasterGain;

    float currentFreq = 0.0f;
    float targetFreq = 0.0f;
    float glideCoeff = 0.0f;
    float lastGlideMs = -1.0f;
    float lastFmDecayMs = -1.0f;
    float currentVelocity = 1.0f;
    float pitchBendSemitones = 0.0f;
    float modWheelValue = 0.0f;
    float pitchDropAmount = 0.5f;
    int activeMidiNote = -1;
    int tailSamplesRemaining = 0;
    juce::Array<int> heldNotes;
};

} // namespace ExtasisDSP
