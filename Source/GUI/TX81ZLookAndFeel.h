#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

namespace ExtasisGUI
{

class TX81ZLookAndFeel : public juce::LookAndFeel_V4
{
public:
    TX81ZLookAndFeel();
    ~TX81ZLookAndFeel() override = default;

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPosProportional, float rotaryStartAngle,
                          float rotaryEndAngle, juce::Slider& slider) override;

    void drawButtonBackground(juce::Graphics& g, juce::Button& button,
                              const juce::Colour& backgroundColour,
                              bool shouldDrawButtonAsHighlighted,
                              bool shouldDrawButtonAsDown) override;

    void drawButtonText(juce::Graphics& g, juce::TextButton& button,
                        bool shouldDrawButtonAsHighlighted,
                        bool shouldDrawButtonAsDown) override;

    juce::Font getLabelFont(juce::Label&) override;

    // Palette
    static juce::Colour getChassisColor()      { return juce::Colour(0xff16181b); }
    static juce::Colour getChassisBevel()      { return juce::Colour(0xff2a2d32); }
    static juce::Colour getCyanAccent()        { return juce::Colour(0xff00d4d4); }
    static juce::Colour getLcdBacklight()      { return juce::Colour(0xff0e210b); }
    static juce::Colour getLcdNeonGreen()      { return juce::Colour(0xff44ff22); }
    static juce::Colour getLcdNeonYellow()     { return juce::Colour(0xffd8ff00); }
    static juce::Colour getLedRedOn()          { return juce::Colour(0xffff2233); }
    static juce::Colour getLedRedOff()         { return juce::Colour(0xff440808); }
    static juce::Colour getKnobCapColor()      { return juce::Colour(0xff222428); }
};

} // namespace ExtasisGUI
