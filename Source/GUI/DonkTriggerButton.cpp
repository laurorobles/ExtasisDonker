#include "DonkTriggerButton.h"
#include "TX81ZLookAndFeel.h"

namespace ExtasisGUI
{

DonkTriggerButton::DonkTriggerButton()
{
    setRepaintsOnMouseActivity(true);
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
    auto bounds = getLocalBounds().toFloat().reduced(1.0f);

    // 1. Button Background
    juce::Colour baseColor = isTriggerActive ? juce::Colour(0xff0e2815) : (isMouseOver() ? juce::Colour(0xff2a303a) : juce::Colour(0xff1e2229));
    g.setColour(baseColor);
    g.fillRoundedRectangle(bounds, 4.0f);

    // 2. Border & Bevel
    juce::Colour borderColor = isTriggerActive ? TX81ZLookAndFeel::getCyanAccent() : (isMouseOver() ? juce::Colour(0xff4a5464) : juce::Colour(0xff333a46));
    g.setColour(borderColor);
    g.drawRoundedRectangle(bounds, 4.0f, isTriggerActive ? 1.5f : 1.0f);

    // 3. LED Indicator
    float ledX = bounds.getX() + 8.0f;
    float ledY = bounds.getCentreY() - 3.5f;
    g.setColour(isTriggerActive ? TX81ZLookAndFeel::getLedRedOn() : TX81ZLookAndFeel::getLedRedOff());
    g.fillEllipse(ledX, ledY, 7.0f, 7.0f);
    g.setColour(juce::Colour(0xff111317));
    g.drawEllipse(ledX, ledY, 7.0f, 7.0f, 0.8f);

    if (isTriggerActive)
    {
        // LED Glow
        g.setColour(juce::Colour(0x60ff2233));
        g.drawEllipse(ledX - 2.0f, ledY - 2.0f, 11.0f, 11.0f, 1.5f);
    }

    // 4. Button Text & Note Badge
    g.setFont(juce::Font(juce::Font::getDefaultMonospacedFontName(), 11.0f, juce::Font::bold));
    g.setColour(isTriggerActive ? TX81ZLookAndFeel::getLcdNeonYellow() : juce::Colour(0xffe0e6f0));
    
    juce::String offsetStr = (semitoneOffset >= 0 ? "+" : "") + juce::String(semitoneOffset) + "st";
    juce::String labelText = "TRIGGER: " + getNoteNameString() + " (" + offsetStr + ")";
    
    auto textRect = bounds.withTrimmedLeft(20.0f).toNearestInt();
    g.drawFittedText(labelText, textRect, juce::Justification::centred, 1);
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
        onStatusChange("AUDITION TRIGGER ACTIVE", "NOTE: " + getNoteNameString() + " (" + offsetStr + " st) | DRAG TO PITCH");
    }

    repaint();
}

void DonkTriggerButton::mouseDrag(const juce::MouseEvent& e)
{
    if (!isTriggerActive)
        return;

    float deltaY = dragStartY - (float)e.getScreenY();
    // 8 pixels per semitone
    int newOffset = juce::jlimit(-12, 12, dragStartOffset + (int)(deltaY / 8.0f));

    if (newOffset != semitoneOffset)
    {
        int oldNote = getCurrentMidiNote();
        semitoneOffset = newOffset;
        int newNote = getCurrentMidiNote();

        // Release old note and trigger new note for real-time glide/pitch auditioning
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
        onStatusChange("AUDITION TRIGGER [" + getNoteNameString() + " / " + offsetStr + " st]", "CLICK TO AUDITION // DRAG UP/DOWN TO SHIFT SEMITONES (+/-12)");
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
