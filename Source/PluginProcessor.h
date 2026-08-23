#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
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

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }
    
    // Preset definitions
    void loadPreset(int presetIndex);
    static juce::StringArray getPresetNames();

    // Direct Audition Trigger from UI
    void triggerAuditionNote(int noteNumber, float velocity);
    void releaseAuditionNote(int noteNumber);

    // FIFO Scope listener for Editor
    std::function<void(const float*, int)> onAudioBlockProcessed;
    // Callback when MIDI CC changes a param to notify UI
    std::function<void(int ccNumber, float value)> onMidiCCReceived;

private:
    juce::AudioProcessorValueTreeState apvts;
    ExtasisDSP::DonkSynth synth;

    int currentProgramIndex = 0;

    void handleMidiCC(int ccNumber, int ccValue);
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ExtasisDonkerAudioProcessor)
};
