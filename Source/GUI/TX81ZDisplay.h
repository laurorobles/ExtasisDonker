#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <vector>

namespace ExtasisGUI
{

class TX81ZDisplay : public juce::Component, public juce::Timer
{
public:
    TX81ZDisplay();
    ~TX81ZDisplay() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void timerCallback() override;

    void setPatchName(const juce::String& name);
    void setParameterReadout(const juce::String& paramName, const juce::String& valueText);
    void pushAudioSamples(const float* samples, int numSamples);

private:
    juce::String currentPatchName = "01. LATELY 1987 (TX81Z)";
    juce::String currentParamName = "ALGORITHM: 2 (2-OP FM)";
    juce::String currentValueText = "RATIO: 2.00 | DONK: 80%";
    int readoutTimeoutCounter = 0;

    // Real-time oscilloscope buffer
    static constexpr int scopeBufferSize = 256;
    std::vector<float> scopeBuffer;
    std::vector<float> incomingFifo;
    juce::CriticalSection fifoLock;
};

} // namespace ExtasisGUI
