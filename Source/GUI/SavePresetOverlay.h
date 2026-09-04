#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

class SavePresetOverlay : public juce::Component
{
public:
    std::function<void(const juce::String&)> onSave;
    std::function<void()> onCancel;

    juce::TextEditor nameInput;
    juce::TextButton saveButton;
    juce::TextButton cancelButton;

    SavePresetOverlay()
    {
        addAndMakeVisible(nameInput);
        nameInput.setMultiLine(false);
        nameInput.setFont(juce::FontOptions(14.0f, juce::Font::bold));
        nameInput.setJustification(juce::Justification::centred);
        nameInput.setTextToShowWhenEmpty("Enter preset name...", juce::Colour(0xff718093));
        nameInput.setColour(juce::TextEditor::backgroundColourId, juce::Colour(0xff14171a));
        nameInput.setColour(juce::TextEditor::textColourId, juce::Colours::white);
        nameInput.setColour(juce::TextEditor::outlineColourId, juce::Colour(0xff00d2ff));

        addAndMakeVisible(saveButton);
        saveButton.setButtonText("SAVE");
        saveButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff27ae60));
        saveButton.onClick = [this]() {
            if (onSave) onSave(nameInput.getText().trim());
        };

        addAndMakeVisible(cancelButton);
        cancelButton.setButtonText("CANCEL");
        cancelButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xffe74c3c));
        cancelButton.onClick = [this]() {
            if (onCancel) onCancel();
        };
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colour(0xee0f141a));

        int modalW = 300;
        int modalH = 140;
        int modalX = (getWidth() - modalW) / 2;
        int modalY = (getHeight() - modalH) / 2;

        g.setColour(juce::Colours::black.withAlpha(0.7f));
        g.fillRoundedRectangle((float)(modalX + 6), (float)(modalY + 6), (float)modalW, (float)modalH, 8.0f);

        juce::ColourGradient cardGrad(juce::Colour(0xff282c34), (float)modalX, (float)modalY,
                                       juce::Colour(0xff1c2025), (float)modalX, (float)(modalY + modalH), false);
        g.setGradientFill(cardGrad);
        g.fillRoundedRectangle((float)modalX, (float)modalY, (float)modalW, (float)modalH, 8.0f);

        g.setColour(juce::Colour(0xff00d2ff).withAlpha(0.7f));
        g.drawRoundedRectangle((float)modalX, (float)modalY, (float)modalW, (float)modalH, 8.0f, 1.5f);

        g.setFont(juce::FontOptions(14.0f, juce::Font::bold));
        g.setColour(juce::Colours::white);
        g.drawText("SAVE CUSTOM PRESET", modalX, modalY + 15, modalW, 20, juce::Justification::centred);
    }

    void resized() override
    {
        int modalW = 300;
        int modalH = 140;
        int modalX = (getWidth() - modalW) / 2;
        int modalY = (getHeight() - modalH) / 2;

        nameInput.setBounds(modalX + 20, modalY + 50, modalW - 40, 30);
        saveButton.setBounds(modalX + 20, modalY + 95, 125, 30);
        cancelButton.setBounds(modalX + 155, modalY + 95, 125, 30);
    }
};
