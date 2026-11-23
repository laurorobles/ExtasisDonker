#include "TX81ZLookAndFeel.h"

namespace ExtasisGUI
{

TX81ZLookAndFeel::TX81ZLookAndFeel()
{
    setColour(juce::Slider::rotarySliderFillColourId, getCyanAccent());
    setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff333840));
    setColour(juce::Label::textColourId, juce::Colours::white);
    setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff1a1d22));
    setColour(juce::ComboBox::textColourId, juce::Colour(0xffdce3ee));
    setColour(juce::ComboBox::outlineColourId, juce::Colour(0xff3d434e));
    setColour(juce::ComboBox::arrowColourId, getCyanAccent());
    setColour(juce::PopupMenu::backgroundColourId, juce::Colour(0xff14171b));
    setColour(juce::PopupMenu::textColourId, juce::Colour(0xffd0d6e0));
    setColour(juce::PopupMenu::highlightedBackgroundColourId, juce::Colour(0xff1f363c));
    setColour(juce::PopupMenu::highlightedTextColourId, getCyanAccent());
}

void TX81ZLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                        float sliderPosProportional, float rotaryStartAngle,
                                        float rotaryEndAngle, juce::Slider& slider)
{
    auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(3.0f);
    auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.5f;
    auto centre = bounds.getCentre();
    float strokeScale = juce::jmax(0.6f, radius / 22.0f);

    // 1. Outer metallic bezel ring
    g.setColour(juce::Colour(0xff1a1c20));
    g.fillEllipse(centre.x - radius, centre.y - radius, radius * 2.0f, radius * 2.0f);

    g.setColour(slider.isMouseOverOrDragging() ? juce::Colour(0xff525a66) : juce::Colour(0xff3a3f47));
    g.drawEllipse(centre.x - radius, centre.y - radius, radius * 2.0f, radius * 2.0f, 1.3f * strokeScale);

    // 2. Arc Track
    float arcRadius = radius - 3.0f * strokeScale;
    juce::Path backgroundArc;
    backgroundArc.addCentredArc(centre.x, centre.y, arcRadius, arcRadius, 0.0f,
                                rotaryStartAngle, rotaryEndAngle, true);
    g.setColour(juce::Colour(0xff252930));
    g.strokePath(backgroundArc, juce::PathStrokeType(2.8f * strokeScale, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // Active Track with Cyan Glow
    float currentAngle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
    juce::Path valueArc;
    valueArc.addCentredArc(centre.x, centre.y, arcRadius, arcRadius, 0.0f,
                           rotaryStartAngle, currentAngle, true);
    g.setColour(slider.isMouseOverOrDragging() ? getCyanAccent().brighter(0.25f) : getCyanAccent());
    g.strokePath(valueArc, juce::PathStrokeType(3.2f * strokeScale, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // 3. Knob Body (tactile 80s synthesizer knob)
    float knobRadius = radius - 6.5f * strokeScale;
    if (knobRadius > 3.5f)
    {
        juce::ColourGradient knobGrad(juce::Colour(0xff2e333c), centre.x - knobRadius, centre.y - knobRadius,
                                      juce::Colour(0xff111316), centre.x + knobRadius, centre.y + knobRadius, false);
        g.setGradientFill(knobGrad);
        g.fillEllipse(centre.x - knobRadius, centre.y - knobRadius, knobRadius * 2.0f, knobRadius * 2.0f);

        // Subtle inner rim
        g.setColour(juce::Colour(0xff454c57));
        g.drawEllipse(centre.x - knobRadius, centre.y - knobRadius, knobRadius * 2.0f, knobRadius * 2.0f, 1.0f);

        // Indicator Line
        juce::Path pointer;
        float pointerLength = knobRadius * 0.78f;
        pointer.startNewSubPath(centre.x + std::sin(currentAngle) * (knobRadius * 0.18f),
                                centre.y - std::cos(currentAngle) * (knobRadius * 0.18f));
        pointer.lineTo(centre.x + std::sin(currentAngle) * pointerLength,
                       centre.y - std::cos(currentAngle) * pointerLength);

        g.setColour(getCyanAccent());
        g.strokePath(pointer, juce::PathStrokeType(2.3f * strokeScale, juce::PathStrokeType::beveled, juce::PathStrokeType::rounded));
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
    juce::Colour border = button.getToggleState() ? getCyanAccent().withAlpha(0.75f)
                          : (shouldDrawButtonAsDown ? juce::Colour(0xff0d0e10) : juce::Colour(0xff484e59));
    g.setColour(border);
    g.drawRoundedRectangle(bounds, 3.0f, 1.0f);

    // LED indicator for toggle buttons
    if (button.getClickingTogglesState())
    {
        float ledSize = juce::jmax(3.5f, bounds.getHeight() * 0.18f);
        g.setColour(button.getToggleState() ? getLedRedOn() : getLedRedOff());
        g.fillEllipse(bounds.getRight() - ledSize - 3.5f, bounds.getY() + 3.5f, ledSize, ledSize);
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
    g.setFont(juce::Font(juce::FontOptions(juce::Font::getDefaultMonospacedFontName(),
                                           juce::jmax(7.0f, bounds.getHeight() * 0.42f),
                                           juce::Font::bold)));
    g.drawFittedText(button.getButtonText(), bounds.reduced(3.0f, 0.0f).toNearestInt(), juce::Justification::centred, 1);
}

void TX81ZLookAndFeel::drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown,
                                    int /*buttonX*/, int /*buttonY*/, int /*buttonW*/, int /*buttonH*/,
                                    juce::ComboBox& box)
{
    auto bounds = juce::Rectangle<float>(0.0f, 0.0f, (float)width, (float)height).reduced(1.0f);

    g.setColour(isButtonDown ? juce::Colour(0xff14171c) : juce::Colour(0xff1e2228));
    g.fillRoundedRectangle(bounds, 3.0f);

    g.setColour(box.isMouseOver() ? getCyanAccent().withAlpha(0.7f) : juce::Colour(0xff424854));
    g.drawRoundedRectangle(bounds, 3.0f, 1.0f);

    // Subtle dropdown arrow
    float arrowZoneW = juce::jmax(10.0f, (float)height * 0.55f);
    float cx = bounds.getRight() - arrowZoneW * 0.5f;
    float cy = bounds.getCentreY();
    float aw = arrowZoneW * 0.28f;
    float ah = arrowZoneW * 0.18f;

    juce::Path arrow;
    arrow.startNewSubPath(cx - aw, cy - ah);
    arrow.lineTo(cx, cy + ah);
    arrow.lineTo(cx + aw, cy - ah);

    g.setColour(getCyanAccent());
    g.strokePath(arrow, juce::PathStrokeType(1.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
}

juce::Font TX81ZLookAndFeel::getComboBoxFont(juce::ComboBox& box)
{
    return juce::Font(juce::FontOptions(juce::Font::getDefaultMonospacedFontName(),
                                        juce::jmax(8.0f, (float)box.getHeight() * 0.44f),
                                        juce::Font::bold));
}

void TX81ZLookAndFeel::drawPopupMenuBackground(juce::Graphics& g, int width, int height)
{
    g.fillAll(juce::Colour(0xff14171b));
    g.setColour(getCyanAccent().withAlpha(0.5f));
    g.drawRect(0, 0, width, height, 1);
}

void TX81ZLookAndFeel::drawPopupMenuItem(juce::Graphics& g, const juce::Rectangle<int>& area,
                                         bool isSeparator, bool isActive, bool isHighlighted,
                                         bool isTicked, bool /*hasSubMenu*/,
                                         const juce::String& text, const juce::String& /*shortcutKeyText*/,
                                         const juce::Drawable* /*icon*/, const juce::Colour* /*textColourToUse*/)
{
    if (isSeparator)
    {
        g.setColour(juce::Colour(0xff2c313a));
        g.fillRect(area.reduced(5, 0).withHeight(1).withCentre(area.getCentre()));
        return;
    }

    auto r = area.reduced(2, 1);
    if (isHighlighted && isActive)
    {
        g.setColour(juce::Colour(0xff1c343b));
        g.fillRoundedRectangle(r.toFloat(), 2.0f);
    }

    g.setColour(isHighlighted ? getCyanAccent() : (isTicked ? getLcdNeonYellow() : juce::Colour(0xffd0d6e0)));
    g.setFont(juce::Font(juce::FontOptions(juce::Font::getDefaultMonospacedFontName(), 12.0f,
                                           isTicked ? juce::Font::bold : juce::Font::plain)));

    juce::String prefix = isTicked ? "> " : "  ";
    g.drawFittedText(prefix + text, r.reduced(6, 0), juce::Justification::centredLeft, 1);
}

juce::Font TX81ZLookAndFeel::getLabelFont(juce::Label& label)
{
    return label.getFont();
}

} // namespace ExtasisGUI
