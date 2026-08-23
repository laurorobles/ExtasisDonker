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
        filter.setResonance(1.0f);

        currentFreq = 0.0f;
        targetFreq = 0.0f;
        glideCoeff = 0.0f;
        currentVelocity = 1.0f;
        activeMidiNote = -1;
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
        heldNotes.clear();
    }

    void handleMidiEvent(const juce::MidiMessage& msg)
    {
        if (msg.isNoteOn())
        {
            int noteNumber = msg.getNoteNumber();
            float velocity = msg.getFloatVelocity();
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
            ampEnvelope.noteOn();
            fmEnvelope.noteOn();
            pitchTransient.trigger(transientClickAmount * 24.0f, 12.0f); // up to 24 semitones snap
            oscillator.reset();
        }
        else
        {
            // Legato glide: retrigger pitch transient slightly for bite
            pitchTransient.trigger(transientClickAmount * 12.0f, 8.0f);
            if (!fmEnvelope.isActive())
                fmEnvelope.noteOn();
        }

        activeMidiNote = noteNumber;
    }

    void updateGlide(float glideMs)
    {
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
                 float timeScale,
                 float modSens,
                 float wavePosition,
                 float subGainDb,
                 float subTone,
                 float txCrunch,
                 float transientClick,
                 float filterCutoffHz,
                 float reverbSpace,
                 float erosionAmount,
                 float punchSlamAmount,
                 bool enableSoftClip,
                 float glideMs,
                 float masterVolGain)
    {
        transientClickAmount = transientClick;
        updateGlide(glideMs);

        // Update envelope parameters
        fmEnvelope.setParameters(0.5f, fmEnvDecayMs, 0.0f, 40.0f, timeScale);
        ampEnvelope.setParameters(1.0f, 800.0f, 0.7f, 60.0f, timeScale);

        // Filter cutoff modulated slightly by velocity
        float velCutoffScale = 1.0f + (currentVelocity - 0.5f) * modSens * 0.6f;
        float actualCutoff = juce::jlimit(20.0f, 20000.0f, filterCutoffHz * velCutoffScale);
        filter.setCutoffFrequency(actualCutoff);

        // Sub gain linear
        float subGainLinear = subGainDb <= -35.0f ? 0.0f : juce::Decibels::decibelsToGain(subGainDb);

        // Dynamic FM Amount scaled by velocity and modSens
        float velFmScale = 1.0f + (currentVelocity - 0.5f) * modSens;
        float effectiveFmAmount = juce::jlimit(0.0f, 1.0f, fmAmount * velFmScale);

        auto* leftChannel = buffer.getWritePointer(0);
        auto* rightChannel = buffer.getNumChannels() > 1 ? buffer.getWritePointer(1) : nullptr;
        int numSamples = buffer.getNumSamples();

        for (int i = 0; i < numSamples; ++i)
        {
            // Pitch glide calculation
            if (glideCoeff > 0.0f && currentFreq > 0.0f)
            {
                currentFreq = targetFreq + (currentFreq - targetFreq) * glideCoeff;
            }
            else
            {
                currentFreq = targetFreq;
            }

            // Pitch transient modulation (click)
            float pitchSnapSemitones = pitchTransient.getNextSemitoneOffset();
            float snappedFreq = currentFreq * std::pow(2.0f, pitchSnapSemitones / 12.0f);

            // Envelope values
            float fmEnvVal = fmEnvelope.getNextSample();
            float ampEnvVal = ampEnvelope.getNextSample();

            if (!ampEnvelope.isActive() && activeMidiNote < 0)
            {
                leftChannel[i] = 0.0f;
                if (rightChannel != nullptr)
                    rightChannel[i] = 0.0f;
                continue;
            }

            // Synthesis core: FM Carrier + Sub
            auto [carrierOut, subOut] = oscillator.processSample(
                snappedFreq,
                fmTuneRatio,
                effectiveFmAmount,
                fmEnvVal,
                txCrunch,
                wavePosition,
                subTone
            );

            // Sum FM Carrier + Sub with amp envelope
            float synthOut = (carrierOut + subOut * subGainLinear) * ampEnvVal;

            // Lowpass Filter
            float filteredOut = filter.processSample(0, synthOut);

            // Stereo Pre-Master Processing (Erosion + Punch Slam + Mono Crossover + Soft-Clip)
            float leftSample = filteredOut;
            float rightSample = filteredOut;
            stereoProcessor.processSample(leftSample, rightSample, reverbSpace, txCrunch * 0.5f,
                                          erosionAmount, punchSlamAmount, enableSoftClip);

            // Master Volume
            leftChannel[i] = leftSample * masterVolGain;
            if (rightChannel != nullptr)
                rightChannel[i] = rightSample * masterVolGain;
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

    float currentFreq = 0.0f;
    float targetFreq = 0.0f;
    float glideCoeff = 0.0f;
    float currentVelocity = 1.0f;
    float transientClickAmount = 0.5f;
    int activeMidiNote = -1;
    juce::Array<int> heldNotes;
};

} // namespace ExtasisDSP
