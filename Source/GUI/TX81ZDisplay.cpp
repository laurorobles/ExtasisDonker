#include "TX81ZDisplay.h"
#include "TX81ZLookAndFeel.h"

namespace ExtasisGUI
{

TX81ZDisplay::TX81ZDisplay()
{
    scopeBuffer.resize(scopeBufferSize, 0.0f);
    incomingFifo.resize(1024, 0.0f);
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
    readoutTimeoutCounter = 90; // stay visible for 3 seconds
    repaint();
}

void TX81ZDisplay::pushAudioSamples(const float* samples, int numSamples)
{
    if (samples == nullptr || numSamples <= 0)
        return;

    const juce::ScopedLock sl(fifoLock);
    for (int i = 0; i < numSamples; ++i)
    {
        incomingFifo.push_back(samples[i]);
    }
    while (incomingFifo.size() > 2048)
    {
        incomingFifo.erase(incomingFifo.begin(), incomingFifo.begin() + 512);
    }
}

void TX81ZDisplay::timerCallback()
{
    {
        const juce::ScopedLock sl(fifoLock);
        if (incomingFifo.size() >= (size_t)scopeBufferSize)
        {
            // Simple trigger: search for positive zero crossing
            size_t triggerIdx = 0;
            for (size_t i = 1; i < incomingFifo.size() - scopeBufferSize; ++i)
            {
                if (incomingFifo[i - 1] <= 0.0f && incomingFifo[i] > 0.0f)
                {
                    triggerIdx = i;
                    break;
                }
            }

            for (int i = 0; i < scopeBufferSize; ++i)
            {
                if (triggerIdx + i < incomingFifo.size())
                    scopeBuffer[i] = incomingFifo[triggerIdx + i];
            }

            incomingFifo.erase(incomingFifo.begin(), incomingFifo.begin() + juce::jmin((size_t)scopeBufferSize, incomingFifo.size()));
        }
    }

    if (readoutTimeoutCounter > 0)
    {
        readoutTimeoutCounter--;
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

    // 1. Dark Bezel Frame
    g.setColour(juce::Colour(0xff090b0d));
    g.fillRoundedRectangle(bounds, 6.0f);
    g.setColour(juce::Colour(0xff2d323a));
    g.drawRoundedRectangle(bounds.reduced(1.0f), 5.0f, 1.5f);

    // 2. LCD Screen Window
    auto lcdArea = bounds.reduced(8.0f);
    juce::Colour lcdDark(0xff0a1c08);
    juce::Colour lcdGlow(0xff143310);
    juce::ColourGradient lcdGrad(lcdDark, lcdArea.getX(), lcdArea.getY(),
                                lcdGlow, lcdArea.getRight(), lcdArea.getBottom(), false);
    g.setGradientFill(lcdGrad);
    g.fillRoundedRectangle(lcdArea, 3.0f);

    // LCD subtle grid / scanlines
    g.setColour(juce::Colour(0x0a39ff14));
    for (float y = lcdArea.getY(); y < lcdArea.getBottom(); y += 3.0f)
    {
        g.drawHorizontalLine((int)y, lcdArea.getX(), lcdArea.getRight());
    }

    // 3. Neon Display Typography
    juce::Font monoFont(juce::Font::getDefaultMonospacedFontName(), 13.0f, juce::Font::bold);
    g.setFont(monoFont);

    // Top Header: Patch Name
    auto headerArea = lcdArea.removeFromTop(24.0f).reduced(6.0f, 2.0f);
    g.setColour(TX81ZLookAndFeel::getLcdNeonYellow());
    g.drawFittedText("PATCH: " + currentPatchName.toUpperCase(), headerArea.toNearestInt(), juce::Justification::left, 1);

    // Subtle divider line
    g.setColour(juce::Colour(0x3344ff22));
    g.drawHorizontalLine((int)lcdArea.getY(), lcdArea.getX() + 6.0f, lcdArea.getRight() - 6.0f);

    // Middle Section: Live Readout
    auto readoutArea = lcdArea.removeFromTop(24.0f).reduced(6.0f, 2.0f);
    g.setColour(TX81ZLookAndFeel::getLcdNeonGreen());
    g.drawFittedText(currentParamName + "  |  " + currentValueText, readoutArea.toNearestInt(), juce::Justification::left, 1);

    // 4. Oscilloscope Vector Waveform Display
    auto scopeArea = lcdArea.reduced(6.0f, 4.0f);
    if (scopeArea.getHeight() > 10.0f)
    {
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
        float xStep = scopeArea.getWidth() / (float)scopeBufferSize;
        float yHalf = scopeArea.getHeight() * 0.45f;

        for (int i = 0; i < scopeBufferSize; ++i)
        {
            float x = scopeArea.getX() + i * xStep;
            float sample = juce::jlimit(-1.0f, 1.0f, scopeBuffer[i]);
            float y = midY - sample * yHalf;

            if (i == 0)
                wavePath.startNewSubPath(x, y);
            else
                wavePath.lineTo(x, y);
        }

        // Phosphor glow (thick background trace)
        g.setColour(juce::Colour(0x4039ff14));
        g.strokePath(wavePath, juce::PathStrokeType(3.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        // Sharp bright trace
        g.setColour(TX81ZLookAndFeel::getLcdNeonYellow());
        g.strokePath(wavePath, juce::PathStrokeType(1.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }
}

void TX81ZDisplay::resized()
{
}

} // namespace ExtasisGUI
