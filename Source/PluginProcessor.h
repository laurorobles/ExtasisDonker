#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "DSP/DonkSynth.h"

class ExtasisDonkerAudioProcessor : public juce::AudioProcessor
{
public:
    bool isPluginLicensed = false;
    int64_t demoStartTimeMs = 0;
    bool demoExpired = false;
    std::function<void()> onDemoExpired;
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

    void triggerAuditionNote(int noteNumber = 36, float velocity = 1.0f);
    void releaseAuditionNote(int noteNumber = 36);

    std::function<void(const float* samples, int numSamples)> onAudioBlockProcessed;
    std::function<void(int ccNumber, float value)> onMidiCCReceived;

private:
    uint64_t samplesProcessed = 0;

    juce::AudioProcessorValueTreeState apvts;
    ExtasisDSP::DonkSynth synth;


    int currentProgramIndex = 0;

    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    void handleMidiCC(int ccNumber, int ccValue);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ExtasisDonkerAudioProcessor)
};
