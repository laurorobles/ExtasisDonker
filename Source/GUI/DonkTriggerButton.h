#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <functional>

namespace ExtasisGUI
{

class DonkTriggerButton : public juce::Component
{
public:
    DonkTriggerButton();
    ~DonkTriggerButton() override = default;

    void paint(juce::Graphics& g) override;
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;
    void mouseEnter(const juce::MouseEvent& e) override;
    void mouseExit(const juce::MouseEvent& e) override;

    int getCurrentMidiNote() const { return baseMidiNote + semitoneOffset; }
    juce::String getNoteNameString() const;

    std::function<void(int midiNote, float velocity)> onNoteOn;
    std::function<void(int midiNote)> onNoteOff;
    std::function<void(const juce::String& title, const juce::String& desc)> onStatusChange;

private:
    int baseMidiNote = 36; // C1
    int semitoneOffset = 0; // -12 to +12
    bool isTriggerActive = false;
    float dragStartY = 0.0f;
    int dragStartOffset = 0;
};

} // namespace ExtasisGUI
