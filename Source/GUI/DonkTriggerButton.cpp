#include "DonkTriggerButton.h"
#include "TX81ZLookAndFeel.h"
#include "BinaryData.h"

namespace ExtasisGUI
{

DonkTriggerButton::DonkTriggerButton()
{
    setRepaintsOnMouseActivity(true);
    if (BinaryData::logo_pngSize > 0)
    {
        logoImage = juce::ImageCache::getFromMemory(BinaryData::logo_png, BinaryData::logo_pngSize);
    }
}

juce::String DonkTriggerButton::getNoteNameString() const
{
    int currentNote = getCurrentMidiNote();
    int octave = (currentNote / 12) - 2;
    static const char* const noteNames[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
    juce::String name = noteNames[currentNote % 12];
    name += juce::String(octave);
    return name;
}

void DonkTriggerButton::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced(2.0f);

    // 1. Recessed Rack Panel Background
    juce::Colour bgDark = isTriggerActive ? juce::Colour(0xff08140b) : (isMouseOver() ? juce::Colour(0xff181c22) : juce::Colour(0xff101317));
    juce::Colour bgLight = isTriggerActive ? juce::Colour(0xff122615) : (isMouseOver() ? juce::Colour(0xff252c38) : juce::Colour(0xff181b21));

    juce::ColourGradient bgGrad(bgDark, bounds.getX(), bounds.getY(),
                                bgLight, bounds.getRight(), bounds.getBottom(), false);
    g.setGradientFill(bgGrad);
    g.fillRoundedRectangle(bounds, 6.0f);

    // Border Bevel
    juce::Colour borderColor = isTriggerActive ? TX81ZLookAndFeel::getLcdNeonGreen() 
                             : (isMouseOver() ? TX81ZLookAndFeel::getCyanAccent() : juce::Colour(0xff2b313c));
    g.setColour(borderColor);
    g.drawRoundedRectangle(bounds, 6.0f, isTriggerActive ? 1.8f : 1.0f);

    // 2. Active / Hover Neon Backlight Glow behind Logo
    auto logoArea = bounds.reduced(8.0f);

    if (isTriggerActive)
    {
        g.setColour(juce::Colour(0x3039ff14));
        g.fillEllipse(logoArea.getCentreX() - 40.0f, logoArea.getCentreY() - 40.0f, 80.0f, 80.0f);
        g.setColour(juce::Colour(0x60d4ff00));
        g.fillEllipse(logoArea.getCentreX() - 25.0f, logoArea.getCentreY() - 25.0f, 50.0f, 50.0f);
    }
    else if (isMouseOver())
    {
        g.setColour(juce::Colour(0x2500d4d4));
        g.fillEllipse(logoArea.getCentreX() - 35.0f, logoArea.getCentreY() - 35.0f, 70.0f, 70.0f);
    }

    // 3. Draw the High-Res Logo Image
    if (logoImage.isValid())
    {
        auto drawArea = logoArea;
        if (isTriggerActive)
            drawArea = drawArea.translated(0.0f, 1.0f); // subtle 3D tactile button press

        float opacity = isTriggerActive ? 1.0f : (isMouseOver() ? 0.95f : 0.8f);
        g.setOpacity(opacity);
        g.drawImage(logoImage, drawArea, juce::RectanglePlacement::centred | juce::RectanglePlacement::onlyReduceInSize);
        g.setOpacity(1.0f);
    }

    // 4. Subtle Status LED / Note Badge Dot at bottom corner
    float dotX = bounds.getRight() - 14.0f;
    float dotY = bounds.getBottom() - 14.0f;
    g.setColour(isTriggerActive ? TX81ZLookAndFeel::getLedRedOn() : (isMouseOver() ? juce::Colour(0xff882222) : TX81ZLookAndFeel::getLedRedOff()));
    g.fillEllipse(dotX, dotY, 6.0f, 6.0f);
}

void DonkTriggerButton::mouseDown(const juce::MouseEvent& e)
{
    isTriggerActive = true;
    dragStartY = (float)e.getScreenY();
    dragStartOffset = semitoneOffset;

    if (onNoteOn)
        onNoteOn(getCurrentMidiNote(), 1.0f);

    if (onStatusChange)
    {
        juce::String offsetStr = (semitoneOffset >= 0 ? "+" : "") + juce::String(semitoneOffset);
        onStatusChange("EXTASIS TRIGGER ACTIVE", "NOTE: " + getNoteNameString() + " (" + offsetStr + " st) // DRAG UP/DOWN TO PITCH");
    }

    repaint();
}

void DonkTriggerButton::mouseDrag(const juce::MouseEvent& e)
{
    if (!isTriggerActive)
        return;

    float deltaY = dragStartY - (float)e.getScreenY();
    int newOffset = juce::jlimit(-12, 12, dragStartOffset + (int)(deltaY / 8.0f));

    if (newOffset != semitoneOffset)
    {
        int oldNote = getCurrentMidiNote();
        semitoneOffset = newOffset;
        int newNote = getCurrentMidiNote();

        if (onNoteOff)
            onNoteOff(oldNote);
        if (onNoteOn)
            onNoteOn(newNote, 1.0f);

        if (onStatusChange)
        {
            juce::String offsetStr = (semitoneOffset >= 0 ? "+" : "") + juce::String(semitoneOffset);
            onStatusChange("TRIGGER PITCH SHIFT", "NOTE: " + getNoteNameString() + " (" + offsetStr + " st) [MIDI " + juce::String(newNote) + "]");
        }

        repaint();
    }
}

void DonkTriggerButton::mouseUp(const juce::MouseEvent&)
{
    if (isTriggerActive)
    {
        isTriggerActive = false;
        if (onNoteOff)
            onNoteOff(getCurrentMidiNote());

        if (onStatusChange)
        {
            juce::String offsetStr = (semitoneOffset >= 0 ? "+" : "") + juce::String(semitoneOffset);
            onStatusChange("TRIGGER RELEASED", "NOTE: " + getNoteNameString() + " (" + offsetStr + " st) READY");
        }

        repaint();
    }
}

void DonkTriggerButton::mouseEnter(const juce::MouseEvent&)
{
    if (onStatusChange)
    {
        juce::String offsetStr = (semitoneOffset >= 0 ? "+" : "") + juce::String(semitoneOffset);
        onStatusChange("EXTASIS TRIGGER [" + getNoteNameString() + " / " + offsetStr + " st]", "CLICK & HOLD TO AUDITION // DRAG UP/DOWN TO SHIFT (+/-12 st)");
    }
}

void DonkTriggerButton::mouseExit(const juce::MouseEvent&)
{
    if (!isTriggerActive && onStatusChange)
    {
        onStatusChange("FM SYNTHESIS ACTIVE", "DONK ENGINE READY");
    }
}

} // namespace ExtasisGUI
