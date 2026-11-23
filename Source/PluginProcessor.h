#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <array>
#include <atomic>
#include "DSP/DonkSynth.h"

class ExtasisDonkerAudioProcessor : public juce::AudioProcessor
{
public:
    ExtasisDonkerAudioProcessor();
    ~ExtasisDonkerAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    static juce::StringArray getFactoryPresetNames();
    juce::StringArray getAllPresetNames();
    void loadPreset(int presetIndex);
    bool saveUserPreset(const juce::String& presetName);
    bool deleteUserPreset(const juce::String& presetName);
    juce::File getPresetsDirectory() const;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }
    ExtasisDSP::DonkSynth& getSynth() { return synth; }

    // Thread-safe audition trigger from GUI
    void triggerAuditionNote(int noteNumber = 36, float velocity = 1.0f);
    void releaseAuditionNote(int noteNumber = 36);

    // Lock-free audio FIFO for oscilloscope & metering
    static constexpr int scopeFifoCapacity = 4096;
    int popScopeSamples(float* dest, int maxSamples);
    float consumeOutputPeak();
    bool consumeMidiActivity();
    int consumeLastMidiCC();

private:
    void pushScopeSamples(const float* samples, int numSamples);

    juce::AudioProcessorValueTreeState apvts;
    ExtasisDSP::DonkSynth synth;

    // Cached atomic parameter pointers (zero string lookups on audio thread)
    std::atomic<float>* pFmAmount = nullptr;
    std::atomic<float>* pFmTune = nullptr;
    std::atomic<float>* pWavePosition = nullptr;
    std::atomic<float>* pFmEnv = nullptr;
    std::atomic<float>* pPumpAmount = nullptr;
    std::atomic<float>* pModAmount = nullptr;
    std::atomic<float>* pPitchDrop = nullptr;
    std::atomic<float>* pTxCrunch = nullptr;
    std::atomic<float>* pErosionGrit = nullptr;
    std::atomic<float>* pPunchSlam = nullptr;
    std::atomic<float>* pSoftClip = nullptr;
    std::atomic<float>* pSubGain = nullptr;
    std::atomic<float>* pSubTone = nullptr;
    std::atomic<float>* pFilterCutoff = nullptr;
    std::atomic<float>* pFilterReso = nullptr;
    std::atomic<float>* pReverbSpace = nullptr;
    std::atomic<float>* pGlideTime = nullptr;
    std::atomic<float>* pMasterVol = nullptr;

    // Thread-safe audition MIDI queue
    juce::SpinLock auditionMidiLock;
    juce::MidiBuffer pendingAuditionMidi;

    // Lock-free FIFO for GUI oscilloscope
    juce::AbstractFifo scopeFifo { scopeFifoCapacity };
    std::array<float, scopeFifoCapacity> scopeFifoBuffer {};

    // Atomic telemetry for GUI
    std::atomic<float> outputPeak { 0.0f };
    std::atomic<bool> midiActivity { false };
    std::atomic<int> lastMidiCC { -1 };

    int currentProgramIndex = 0;

    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    void handleMidiCC(int ccNumber, int ccValue);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ExtasisDonkerAudioProcessor)
};
