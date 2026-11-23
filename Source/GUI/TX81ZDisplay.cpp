#include "TX81ZDisplay.h"
#include "TX81ZLookAndFeel.h"
#include "../PluginProcessor.h"
#include <cmath>

namespace ExtasisGUI
{

TX81ZDisplay::TX81ZDisplay(ExtasisDonkerAudioProcessor& processor)
    : processorRef(processor)
{
    scopeBuffer.resize(scopeBufferSize, 0.0f);
    scratchBuffer.resize(2048, 0.0f);
    startTimerHz(30);
}

TX81ZDisplay::~TX81ZDisplay()
{
    stopTimer();
}

void TX81ZDisplay::setPatchName(const juce::String& name)
{
    currentPatchName = name;
    repaint();
}

void TX81ZDisplay::setParameterReadout(const juce::String& paramName, const juce::String& valueText)
{
    currentParamName = paramName;
    currentValueText = valueText;
    readoutTimeoutCounter = 90; // stay visible for ~3 seconds
    repaint();
}

void TX81ZDisplay::timerCallback()
{
    // 1. Pull lock-free audio samples from processor
    int numPopped = processorRef.popScopeSamples(scratchBuffer.data(), (int)scratchBuffer.size());
    if (numPopped >= scopeBufferSize)
    {
        // Search for positive zero-crossing trigger in the most recent window
        int searchStart = juce::jmax(1, numPopped - scopeBufferSize * 2);
        int searchEnd = numPopped - scopeBufferSize;
        int triggerIdx = searchEnd;

        for (int i = searchStart; i < searchEnd; ++i)
        {
            if (scratchBuffer[(size_t)(i - 1)] <= 0.0f && scratchBuffer[(size_t)i] > 0.0f)
            {
                triggerIdx = i;
                break;
            }
        }

        for (int i = 0; i < scopeBufferSize; ++i)
            scopeBuffer[(size_t)i] = scratchBuffer[(size_t)(triggerIdx + i)];
    }
    else if (numPopped > 0)
    {
        // Decay smoothly toward silence when fewer samples arrive
        for (int i = 0; i < scopeBufferSize; ++i)
            scopeBuffer[(size_t)i] *= 0.85f;
    }

    // 2. Output peak metering with smooth ballistics + peak hold
    float newPeak = processorRef.consumeOutputPeak();
    if (newPeak > smoothedPeak)
        smoothedPeak = newPeak;
    else
        smoothedPeak *= 0.82f;

    if (newPeak >= peakHold)
    {
        peakHold = newPeak;
        peakHoldFrames = 24; // ~0.8s hold
    }
    else if (peakHoldFrames > 0)
    {
        --peakHoldFrames;
    }
    else
    {
        peakHold *= 0.90f;
    }

    // 3. MIDI activity LED & CC readout
    if (processorRef.consumeMidiActivity())
        midiLedFrames = 5;
    else if (midiLedFrames > 0)
        --midiLedFrames;

    int ccNum = processorRef.consumeLastMidiCC();
    if (ccNum >= 0)
    {
        if (ccNum == 1)
            setParameterReadout("MOD WHEEL [CC 1]", "DYNAMIC FILTER OPENING");
        else
            setParameterReadout("MIDI IN [CC " + juce::String(ccNum) + "]", "AUTOMATION ACTIVE");
    }

    if (readoutTimeoutCounter > 0)
    {
        --readoutTimeoutCounter;
        if (readoutTimeoutCounter == 0)
        {
            currentParamName = "FM SYNTHESIS ACTIVE";
            currentValueText = "DONK ENGINE READY";
        }
    }

    repaint();
}

void TX81ZDisplay::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    float scale = juce::jmax(0.4f, bounds.getWidth() / 420.0f);

    // 1. Dark Bezel Frame
    g.setColour(juce::Colour(0xff090b0d));
    g.fillRoundedRectangle(bounds, 6.0f * scale);
    g.setColour(juce::Colour(0xff2d323a));
    g.drawRoundedRectangle(bounds.reduced(1.0f), 5.0f * scale, 1.5f);

    // 2. LCD Screen Window
    auto lcdArea = bounds.reduced(8.0f * scale);
    juce::Colour lcdDark(0xff0a1c08);
    juce::Colour lcdGlow(0xff143310);
    juce::ColourGradient lcdGrad(lcdDark, lcdArea.getX(), lcdArea.getY(),
                                 lcdGlow, lcdArea.getRight(), lcdArea.getBottom(), false);
    g.setGradientFill(lcdGrad);
    g.fillRoundedRectangle(lcdArea, 3.0f * scale);

    // LCD subtle scanlines
    g.setColour(juce::Colour(0x0a39ff14));
    float scanStep = juce::jmax(2.0f, 3.0f * scale);
    for (float y = lcdArea.getY(); y < lcdArea.getBottom(); y += scanStep)
        g.drawHorizontalLine((int)y, lcdArea.getX(), lcdArea.getRight());

    // 3. Neon Display Typography
    juce::Font monoFont(juce::FontOptions(juce::Font::getDefaultMonospacedFontName(), 12.5f * scale, juce::Font::bold));
    g.setFont(monoFont);

    // Top Header: Patch Name + MIDI Activity LED
    auto headerArea = lcdArea.removeFromTop(23.0f * scale).reduced(6.0f * scale, 2.0f * scale);
    auto midiBadgeArea = headerArea.removeFromRight(46.0f * scale);

    g.setColour(TX81ZLookAndFeel::getLcdNeonYellow());
    g.drawFittedText("PATCH: " + currentPatchName.toUpperCase(), headerArea.toNearestInt(), juce::Justification::left, 1);

    // MIDI LED indicator
    float ledRadius = 3.2f * scale;
    float ledX = midiBadgeArea.getX() + 2.0f * scale;
    float ledY = midiBadgeArea.getCentreY() - ledRadius;
    g.setColour(midiLedFrames > 0 ? TX81ZLookAndFeel::getLcdNeonYellow() : juce::Colour(0xff1c3a17));
    g.fillEllipse(ledX, ledY, ledRadius * 2.0f, ledRadius * 2.0f);

    g.setFont(juce::Font(juce::FontOptions(juce::Font::getDefaultMonospacedFontName(), 9.0f * scale, juce::Font::bold)));
    g.setColour(midiLedFrames > 0 ? TX81ZLookAndFeel::getLcdNeonYellow() : juce::Colour(0xff2c5c24));
    g.drawFittedText("MIDI", midiBadgeArea.withTrimmedLeft(ledRadius * 2.0f + 4.0f * scale).toNearestInt(),
                     juce::Justification::left, 1);

    // Subtle divider line
    g.setColour(juce::Colour(0x3344ff22));
    g.drawHorizontalLine((int)lcdArea.getY(), lcdArea.getX() + 6.0f * scale, lcdArea.getRight() - 6.0f * scale);

    // Middle Section: Live Readout
    g.setFont(monoFont);
    auto readoutArea = lcdArea.removeFromTop(22.0f * scale).reduced(6.0f * scale, 2.0f * scale);
    g.setColour(TX81ZLookAndFeel::getLcdNeonGreen());
    g.drawFittedText(currentParamName + "  |  " + currentValueText, readoutArea.toNearestInt(), juce::Justification::left, 1);

    // 4. Oscilloscope + Integrated Peak Meter
    auto bottomArea = lcdArea.reduced(6.0f * scale, 4.0f * scale);
    if (bottomArea.getHeight() > 10.0f)
    {
        // Reserve thin vertical peak meter on right side of scope
        auto meterArea = bottomArea.removeFromRight(10.0f * scale);
        bottomArea.removeFromRight(4.0f * scale);
        auto scopeArea = bottomArea;

        // Scope Background Frame
        g.setColour(juce::Colour(0x1539ff14));
        g.fillRect(scopeArea);
        g.setColour(juce::Colour(0x3339ff14));
        g.drawRect(scopeArea, 1.0f);

        // Center line
        float midY = scopeArea.getCentreY();
        g.setColour(juce::Colour(0x2239ff14));
        g.drawHorizontalLine((int)midY, scopeArea.getX(), scopeArea.getRight());

        // Draw phosphor glowing waveform path
        juce::Path wavePath;
        float xStep = scopeArea.getWidth() / (float)(scopeBufferSize - 1);
        float yHalf = scopeArea.getHeight() * 0.45f;

        for (int i = 0; i < scopeBufferSize; ++i)
        {
            float x = scopeArea.getX() + i * xStep;
            float sample = juce::jlimit(-1.0f, 1.0f, scopeBuffer[(size_t)i]);
            float y = midY - sample * yHalf;

            if (i == 0)
                wavePath.startNewSubPath(x, y);
            else
                wavePath.lineTo(x, y);
        }

        // Phosphor glow (thick background trace)
        g.setColour(juce::Colour(0x4039ff14));
        g.strokePath(wavePath, juce::PathStrokeType(3.0f * scale, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        // Sharp bright trace
        g.setColour(TX81ZLookAndFeel::getLcdNeonYellow());
        g.strokePath(wavePath, juce::PathStrokeType(1.5f * scale, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        // Vertical Peak Level Meter
        g.setColour(juce::Colour(0x1839ff14));
        g.fillRect(meterArea);
        g.setColour(juce::Colour(0x3339ff14));
        g.drawRect(meterArea, 1.0f);

        float db = juce::Decibels::gainToDecibels(smoothedPeak, -48.0f);
        float normLevel = juce::jlimit(0.0f, 1.0f, (db + 48.0f) / 48.0f);
        float barH = meterArea.getHeight() * normLevel;

        if (barH > 1.0f)
        {
            auto barRect = meterArea.withTop(meterArea.getBottom() - barH).reduced(1.5f, 1.0f);
            juce::Colour meterCol = (smoothedPeak > 0.98f) ? TX81ZLookAndFeel::getLedRedOn()
                                  : (smoothedPeak > 0.70f ? TX81ZLookAndFeel::getLcdNeonYellow()
                                                          : TX81ZLookAndFeel::getLcdNeonGreen());
            g.setColour(meterCol);
            g.fillRect(barRect);
        }

        // Peak hold tick
        if (peakHold > 0.01f)
        {
            float holdDb = juce::Decibels::gainToDecibels(peakHold, -48.0f);
            float holdNorm = juce::jlimit(0.0f, 1.0f, (holdDb + 48.0f) / 48.0f);
            float holdY = meterArea.getBottom() - meterArea.getHeight() * holdNorm;
            g.setColour(peakHold > 0.98f ? TX81ZLookAndFeel::getLedRedOn() : TX81ZLookAndFeel::getLcdNeonYellow());
            g.drawHorizontalLine((int)holdY, meterArea.getX() + 1.0f, meterArea.getRight() - 1.0f);
        }
    }
}

void TX81ZDisplay::resized()
{
}

} // namespace ExtasisGUI
