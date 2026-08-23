#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "GUI/TX81ZLookAndFeel.h"
#include "GUI/TX81ZDisplay.h"
#include "GUI/DonkTriggerButton.h"

class ExtasisDonkerAudioProcessorEditor : public juce::AudioProcessorEditor,
                                         public juce::Slider::Listener,
                                         public juce::ComboBox::Listener,
                                         public juce::Button::Listener
{
public:
    explicit ExtasisDonkerAudioProcessorEditor(ExtasisDonkerAudioProcessor&);
    ~ExtasisDonkerAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    void sliderValueChanged(juce::Slider* slider) override;
    void comboBoxChanged(juce::ComboBox* comboBox) override;
    void buttonClicked(juce::Button* button) override;

    void mouseEnter(const juce::MouseEvent& e) override;
    void mouseExit(const juce::MouseEvent& e) override;

    void refreshPresetList();

private:
    ExtasisDonkerAudioProcessor& processorRef;
    ExtasisGUI::TX81ZLookAndFeel customLookAndFeel;

    // Display
    ExtasisGUI::TX81ZDisplay display;

    // Preset selection & Save
    juce::ComboBox presetBox;
    juce::TextButton prevPresetBtn { "<" };
    juce::TextButton nextPresetBtn { ">" };
    juce::TextButton savePresetBtn { "SAVE" };

    // Audition Trigger Button (Large Logo Pad, Drag for +/-12st)
    ExtasisGUI::DonkTriggerButton triggerBtn;

    // Soft Clip Toggle Button
    juce::TextButton softClipBtn { "SOFT CLIP" };
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> softClipAttachment;

    // Top Right Bandcamp Link Button
    juce::HyperlinkButton bandcampLinkBtn { "extasisrecords.bandcamp.com", juce::URL("https://extasisrecords.bandcamp.com") };

    // Sliders & Labels
    struct KnobControl
    {
        std::unique_ptr<juce::Slider> slider;
        std::unique_ptr<juce::Label> label;
        std::unique_ptr<juce::Label> valueLabel;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
    };

    std::map<juce::String, KnobControl> controls;

    void createKnob(const juce::String& paramId, const juce::String& labelText);
    void updateParamDisplayForSlider(juce::Slider* slider);
    juce::String getFormattedValueText(const juce::String& paramId, double val);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ExtasisDonkerAudioProcessorEditor)
};
