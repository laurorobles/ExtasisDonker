#include "TX81ZLookAndFeel.h"

namespace ExtasisGUI
{

TX81ZLookAndFeel::TX81ZLookAndFeel()
{
    setColour(juce::Slider::rotarySliderFillColourId, getCyanAccent());
    setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff333840));
    setColour(juce::Label::textColourId, juce::Colours::white);
}

void TX81ZLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                        float sliderPosProportional, float rotaryStartAngle,
                                        float rotaryEndAngle, juce::Slider& /*slider*/)
{
    auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(4.0f);
    auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.5f;
    auto centre = bounds.getCentre();

    // 1. Outer metallic bezel ring
    g.setColour(juce::Colour(0xff1a1c20));
    g.fillEllipse(centre.x - radius, centre.y - radius, radius * 2.0f, radius * 2.0f);

    g.setColour(juce::Colour(0xff3a3f47));
    g.drawEllipse(centre.x - radius, centre.y - radius, radius * 2.0f, radius * 2.0f, 1.5f);

    // 2. Arc Track
    float arcRadius = radius - 3.0f;
    juce::Path backgroundArc;
    backgroundArc.addCentredArc(centre.x, centre.y, arcRadius, arcRadius, 0.0f,
                                rotaryStartAngle, rotaryEndAngle, true);
    g.setColour(juce::Colour(0xff252930));
    g.strokePath(backgroundArc, juce::PathStrokeType(3.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // Active Track with Cyan Glow
    float currentAngle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
    juce::Path valueArc;
    valueArc.addCentredArc(centre.x, centre.y, arcRadius, arcRadius, 0.0f,
                           rotaryStartAngle, currentAngle, true);
    g.setColour(getCyanAccent());
    g.strokePath(valueArc, juce::PathStrokeType(3.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // 3. Knob Body (tactile ribbed 80s synthesizer knob)
    float knobRadius = radius - 7.0f;
    if (knobRadius > 4.0f)
    {
        juce::ColourGradient knobGrad(juce::Colour(0xff2c3038), centre.x - knobRadius, centre.y - knobRadius,
                                     juce::Colour(0xff121417), centre.x + knobRadius, centre.y + knobRadius, false);
        g.setGradientFill(knobGrad);
        g.fillEllipse(centre.x - knobRadius, centre.y - knobRadius, knobRadius * 2.0f, knobRadius * 2.0f);

        // Subtle inner rim
        g.setColour(juce::Colour(0xff454c57));
        g.drawEllipse(centre.x - knobRadius, centre.y - knobRadius, knobRadius * 2.0f, knobRadius * 2.0f, 1.0f);

        // Indicator Line
        juce::Path pointer;
        float pointerLength = knobRadius * 0.75f;
        pointer.startNewSubPath(centre.x + std::sin(currentAngle) * (knobRadius * 0.2f),
                                centre.y - std::cos(currentAngle) * (knobRadius * 0.2f));
        pointer.lineTo(centre.x + std::sin(currentAngle) * pointerLength,
                       centre.y - std::cos(currentAngle) * pointerLength);

        g.setColour(getCyanAccent());
        g.strokePath(pointer, juce::PathStrokeType(2.5f, juce::PathStrokeType::beveled, juce::PathStrokeType::rounded));
    }
}

void TX81ZLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button,
                                            const juce::Colour& /*backgroundColour*/,
                                            bool shouldDrawButtonAsHighlighted,
                                            bool shouldDrawButtonAsDown)
{
    auto bounds = button.getLocalBounds().toFloat().reduced(1.0f);

    // Rubberized 80s button
    juce::Colour base = shouldDrawButtonAsDown ? juce::Colour(0xff181a1d)
                        : (shouldDrawButtonAsHighlighted ? juce::Colour(0xff32363e) : juce::Colour(0xff24272c));

    g.setColour(base);
    g.fillRoundedRectangle(bounds, 3.0f);

    // Bevel edges
    g.setColour(shouldDrawButtonAsDown ? juce::Colour(0xff0d0e10) : juce::Colour(0xff484e59));
    g.drawRoundedRectangle(bounds, 3.0f, 1.0f);

    // If button has LED or toggle state
    if (button.getToggleState())
    {
        g.setColour(getLedRedOn());
        g.fillEllipse(bounds.getRight() - 9.0f, bounds.getY() + 4.0f, 5.0f, 5.0f);
    }
}

void TX81ZLookAndFeel::drawButtonText(juce::Graphics& g, juce::TextButton& button,
                                      bool /*shouldDrawButtonAsHighlighted*/,
                                      bool shouldDrawButtonAsDown)
{
    auto bounds = button.getLocalBounds().toFloat();
    if (shouldDrawButtonAsDown)
        bounds.translate(0.0f, 1.0f);

    g.setColour(button.getToggleState() ? getCyanAccent() : juce::Colour(0xffd0d6e0));
    g.setFont(juce::Font(juce::Font::getDefaultMonospacedFontName(), 11.0f, juce::Font::bold));
    g.drawFittedText(button.getButtonText(), bounds.toNearestInt(), juce::Justification::centred, 1);
}

juce::Font TX81ZLookAndFeel::getLabelFont(juce::Label&)
{
    return juce::Font(juce::Font::getDefaultMonospacedFontName(), 11.0f, juce::Font::bold);
}

} // namespace ExtasisGUI
