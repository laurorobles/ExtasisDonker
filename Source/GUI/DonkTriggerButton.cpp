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
    auto bounds = getLocalBounds().toFloat().reduced(2.0f);
    auto centre = bounds.getCentre();
    float size = juce::jmin(bounds.getWidth(), bounds.getHeight());
    float radius = size * 0.5f;

    // 1. Outer Bezel Ring (Machined Dark Metal)
    g.setColour(juce::Colour(0xff0d0f12));
    g.fillEllipse(centre.x - radius, centre.y - radius, radius * 2.0f, radius * 2.0f);
    g.setColour(isMouseOver() ? juce::Colour(0xff4a5464) : juce::Colour(0xff2b313c));
    g.drawEllipse(centre.x - radius, centre.y - radius, radius * 2.0f, radius * 2.0f, 1.5f);

    // 2. Tactile Concave Button Cap
    float capRadius = radius - 4.0f;
    if (capRadius > 4.0f)
    {
        juce::Colour capDark = isTriggerActive ? juce::Colour(0xff121815) : juce::Colour(0xff1a1e24);
        juce::Colour capLight = isTriggerActive ? juce::Colour(0xff090c0a) : (isMouseOver() ? juce::Colour(0xff2a303a) : juce::Colour(0xff222730));

        juce::ColourGradient capGrad(capLight, centre.x, centre.y - capRadius,
                                     capDark, centre.x, centre.y + capRadius, false);
        g.setGradientFill(capGrad);
        g.fillEllipse(centre.x - capRadius, centre.y - capRadius, capRadius * 2.0f, capRadius * 2.0f);

        g.setColour(juce::Colour(0xff333a46));
        g.drawEllipse(centre.x - capRadius, centre.y - capRadius, capRadius * 2.0f, capRadius * 2.0f, 1.0f);
    }

    // 3. Glowing Center LED (Multi-Stage Optical Lens)
    float ledRadius = 7.0f;
    if (isTriggerActive)
    {
        // Big neon/red pulse halo
        g.setColour(juce::Colour(0x55ff2233));
        g.fillEllipse(centre.x - 14.0f, centre.y - 14.0f, 28.0f, 28.0f);

        g.setColour(juce::Colour(0x88ff2233));
        g.fillEllipse(centre.x - 10.0f, centre.y - 10.0f, 20.0f, 20.0f);

        // Core bright red/neon LED
        g.setColour(TX81ZLookAndFeel::getLedRedOn());
        g.fillEllipse(centre.x - ledRadius, centre.y - ledRadius, ledRadius * 2.0f, ledRadius * 2.0f);

        // Hot center white/amber specular highlight
        g.setColour(juce::Colour(0xffffddaa));
        g.fillEllipse(centre.x - 2.5f, centre.y - 2.5f, 5.0f, 5.0f);
    }
    else if (isMouseOver())
    {
        // Hover glow
        g.setColour(juce::Colour(0x33ff4444));
        g.fillEllipse(centre.x - 10.0f, centre.y - 10.0f, 20.0f, 20.0f);

        g.setColour(juce::Colour(0xbb992222));
        g.fillEllipse(centre.x - ledRadius, centre.y - ledRadius, ledRadius * 2.0f, ledRadius * 2.0f);

        g.setColour(juce::Colour(0xffff8888));
        g.fillEllipse(centre.x - 2.0f, centre.y - 2.0f, 4.0f, 4.0f);
    }
    else
    {
        // Idle dark LED lens
        g.setColour(TX81ZLookAndFeel::getLedRedOff());
        g.fillEllipse(centre.x - ledRadius, centre.y - ledRadius, ledRadius * 2.0f, ledRadius * 2.0f);
        g.setColour(juce::Colour(0xff150303));
        g.drawEllipse(centre.x - ledRadius, centre.y - ledRadius, ledRadius * 2.0f, ledRadius * 2.0f, 1.0f);
    }
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
        onStatusChange("AUDITION TRIGGER ACTIVE", "PLAYING: " + getNoteNameString() + " (" + offsetStr + " st) // DRAG VERTICAL TO PITCH");
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
        onStatusChange("TACTILE TRIGGER [" + getNoteNameString() + " / " + offsetStr + " st]", "CLICK & HOLD TO AUDITION // DRAG UP/DOWN TO SHIFT (+/-12 st)");
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
