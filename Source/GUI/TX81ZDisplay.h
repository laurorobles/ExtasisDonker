#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <vector>

class ExtasisDonkerAudioProcessor;

namespace ExtasisGUI
{

class TX81ZDisplay : public juce::Component, public juce::Timer
{
public:
    explicit TX81ZDisplay(ExtasisDonkerAudioProcessor& processor);
    ~TX81ZDisplay() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void timerCallback() override;

    void setPatchName(const juce::String& name);
    void setParameterReadout(const juce::String& paramName, const juce::String& valueText);

private:
    ExtasisDonkerAudioProcessor& processorRef;

    juce::String currentPatchName = "1. BLACKOUT";
    juce::String currentParamName = "FM SYNTHESIS ACTIVE";
    juce::String currentValueText = "DONK ENGINE READY";
    int readoutTimeoutCounter = 0;

    // Telemetry indicators
    float smoothedPeak = 0.0f;
    float peakHold = 0.0f;
    int peakHoldFrames = 0;
    int midiLedFrames = 0;

    // Real-time oscilloscope buffer (GUI-thread owned)
    static constexpr int scopeBufferSize = 256;
    std::vector<float> scopeBuffer;
    std::vector<float> scratchBuffer;
};

} // namespace ExtasisGUI
