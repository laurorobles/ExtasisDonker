#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <functional>

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
        nameInput.setFont(juce::Font(juce::FontOptions(juce::Font::getDefaultMonospacedFontName(), 14.0f, juce::Font::bold)));
        nameInput.setJustification(juce::Justification::centred);
        nameInput.setTextToShowWhenEmpty("Enter preset name...", juce::Colour(0xff718093));
        nameInput.setColour(juce::TextEditor::backgroundColourId, juce::Colour(0xff14171a));
        nameInput.setColour(juce::TextEditor::textColourId, juce::Colours::white);
        nameInput.setColour(juce::TextEditor::outlineColourId, juce::Colour(0xff00d4d4));
        nameInput.setColour(juce::TextEditor::focusedOutlineColourId, juce::Colour(0xff44ff22));
        nameInput.onReturnKey = [this]() {
            if (onSave) onSave(nameInput.getText().trim());
        };
        nameInput.onEscapeKey = [this]() {
            if (onCancel) onCancel();
        };

        addAndMakeVisible(saveButton);
        saveButton.setButtonText("SAVE");
        saveButton.onClick = [this]() {
            if (onSave) onSave(nameInput.getText().trim());
        };

        addAndMakeVisible(cancelButton);
        cancelButton.setButtonText("CANCEL");
        cancelButton.onClick = [this]() {
            if (onCancel) onCancel();
        };
    }

    void prepareToShow(const juce::String& suggestedName = "Custom Donk")
    {
        nameInput.setText(suggestedName, juce::dontSendNotification);
        nameInput.selectAll();
        setVisible(true);
        toFront(true);
        nameInput.grabKeyboardFocus();
        repaint();
    }

    bool keyPressed(const juce::KeyPress& key) override
    {
        if (key == juce::KeyPress::escapeKey)
        {
            if (onCancel) onCancel();
            return true;
        }
        return false;
    }

    void paint(juce::Graphics& g) override
    {
        float scale = juce::jmax(0.5f, (float)getWidth() / 700.0f);
        g.fillAll(juce::Colour(0xdd0b0e12));

        float modalW = 320.0f * scale;
        float modalH = 148.0f * scale;
        float modalX = ((float)getWidth() - modalW) * 0.5f;
        float modalY = ((float)getHeight() - modalH) * 0.5f;

        g.setColour(juce::Colours::black.withAlpha(0.7f));
        g.fillRoundedRectangle(modalX + 6.0f * scale, modalY + 6.0f * scale, modalW, modalH, 8.0f * scale);

        juce::ColourGradient cardGrad(juce::Colour(0xff23272f), modalX, modalY,
                                      juce::Colour(0xff16191e), modalX, modalY + modalH, false);
        g.setGradientFill(cardGrad);
        g.fillRoundedRectangle(modalX, modalY, modalW, modalH, 8.0f * scale);

        g.setColour(juce::Colour(0xff00d4d4).withAlpha(0.8f));
        g.drawRoundedRectangle(modalX, modalY, modalW, modalH, 8.0f * scale, 1.5f);

        g.setFont(juce::Font(juce::FontOptions(juce::Font::getDefaultMonospacedFontName(), 13.5f * scale, juce::Font::bold)));
        g.setColour(juce::Colour(0xff00d4d4));
        g.drawText("SAVE USER PRESET", (int)modalX, (int)(modalY + 14.0f * scale), (int)modalW, (int)(22.0f * scale),
                   juce::Justification::centred);
    }

    void resized() override
    {
        float scale = juce::jmax(0.5f, (float)getWidth() / 700.0f);
        float modalW = 320.0f * scale;
        float modalH = 148.0f * scale;
        float modalX = ((float)getWidth() - modalW) * 0.5f;
        float modalY = ((float)getHeight() - modalH) * 0.5f;

        nameInput.setFont(juce::Font(juce::FontOptions(juce::Font::getDefaultMonospacedFontName(), 13.0f * scale, juce::Font::bold)));
        nameInput.setBounds((int)(modalX + 20.0f * scale), (int)(modalY + 48.0f * scale),
                            (int)(modalW - 40.0f * scale), (int)(30.0f * scale));
        saveButton.setBounds((int)(modalX + 20.0f * scale), (int)(modalY + 96.0f * scale),
                             (int)(134.0f * scale), (int)(30.0f * scale));
        cancelButton.setBounds((int)(modalX + 166.0f * scale), (int)(modalY + 96.0f * scale),
                               (int)(134.0f * scale), (int)(30.0f * scale));
    }
};
