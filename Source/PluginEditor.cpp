#include "PluginEditor.h"

ExtasisDonkerAudioProcessorEditor::ExtasisDonkerAudioProcessorEditor(ExtasisDonkerAudioProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    setLookAndFeel(&customLookAndFeel);

    // 1. Display Setup
    addAndMakeVisible(display);
    processorRef.onAudioBlockProcessed = [this](const float* samples, int numSamples) {
        display.pushAudioSamples(samples, numSamples);
    };

    processorRef.onMidiCCReceived = [this](int ccNum, float /*val*/) {
        juce::MessageManager::callAsync([this, ccNum]() {
            display.setParameterReadout("MIDI IN (CC " + juce::String(ccNum) + ")", "AUTOMATION ACTIVE");
        });
    };

    // 2. Preset Selector
    auto presetNames = ExtasisDonkerAudioProcessor::getPresetNames();
    for (int i = 0; i < presetNames.size(); ++i)
    {
        presetBox.addItem(presetNames[i], i + 1);
    }
    presetBox.setSelectedId(processorRef.getCurrentProgram() + 1, juce::dontSendNotification);
    presetBox.addListener(this);
    addAndMakeVisible(presetBox);

    prevPresetBtn.onClick = [this]() {
        int cur = presetBox.getSelectedId() - 1;
        int next = (cur - 1 + presetBox.getNumItems()) % presetBox.getNumItems();
        presetBox.setSelectedId(next + 1, juce::sendNotification);
    };
    addAndMakeVisible(prevPresetBtn);

    nextPresetBtn.onClick = [this]() {
        int cur = presetBox.getSelectedId() - 1;
        int next = (cur + 1) % presetBox.getNumItems();
        presetBox.setSelectedId(next + 1, juce::sendNotification);
    };
    addAndMakeVisible(nextPresetBtn);

    // 3. Audition Trigger Button
    triggerBtn.onNoteOn = [this](int midiNote, float velocity) {
        processorRef.triggerAuditionNote(midiNote, velocity);
    };
    triggerBtn.onNoteOff = [this](int midiNote) {
        processorRef.releaseAuditionNote(midiNote);
    };
    triggerBtn.onStatusChange = [this](const juce::String& title, const juce::String& desc) {
        display.setParameterReadout(title, desc);
    };
    addAndMakeVisible(triggerBtn);

    // 4. Soft Clip Button
    softClipBtn.setClickingTogglesState(true);
    softClipBtn.addListener(this);
    softClipAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        processorRef.getAPVTS(), "soft_clip", softClipBtn);
    addAndMakeVisible(softClipBtn);

    // 5. Create all Knobs with Live Value Labels & MouseListeners
    createKnob("fm_amount", "DONK PUNCH");
    createKnob("fm_tune", "FM RATIO");
    createKnob("fm_env", "DONK TIME");
    createKnob("wave_position", "TX WAVE");
    createKnob("time_scale", "TIME SCALE");
    createKnob("mod_amount", "VEL SENS");

    createKnob("transient_click", "CLICK / SNAP");
    createKnob("tx_crunch", "TX CRUNCH");
    createKnob("glide_time", "GLIDE");
    createKnob("filter_cutoff", "LP FILTER");

    createKnob("erosion_grit", "EROSION");
    createKnob("punch_slam", "PUNCH SLAM");

    createKnob("sub_gain", "SUB GAIN");
    createKnob("sub_tone", "SUB TONE");
    createKnob("reverb_space", "TOP SPREAD");
    createKnob("master_vol", "MASTER OUT");

    // Initialize all value labels with current parameter values
    for (auto& [paramId, ctrl] : controls)
    {
        if (ctrl.slider)
            ctrl.valueLabel->setText(getFormattedValueText(paramId, ctrl.slider->getValue()), juce::dontSendNotification);
    }

    display.setPatchName(presetBox.getText());

    setSize(980, 530);
}

ExtasisDonkerAudioProcessorEditor::~ExtasisDonkerAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
    processorRef.onAudioBlockProcessed = nullptr;
    processorRef.onMidiCCReceived = nullptr;
}

juce::String ExtasisDonkerAudioProcessorEditor::getFormattedValueText(const juce::String& paramId, double val)
{
    if (paramId == "fm_amount")
        return juce::String((int)val) + " %";
    if (paramId == "fm_tune")
        return juce::String(val, 2) + " x";
    if (paramId == "fm_env")
        return juce::String((int)val) + " ms";
    if (paramId == "wave_position")
    {
        if (val < 25.0) return "Sine " + juce::String((int)val) + "%";
        if (val < 75.0) return "Half " + juce::String((int)val) + "%";
        return "Rect " + juce::String((int)val) + "%";
    }
    if (paramId == "time_scale")
        return (val > 0.0 ? "+" : "") + juce::String((int)val) + " %";
    if (paramId == "mod_amount")
        return juce::String((int)val) + " %";
    if (paramId == "transient_click")
        return juce::String((int)val) + " %";
    if (paramId == "tx_crunch")
        return juce::String((int)val) + " %";
    if (paramId == "filter_cutoff")
        return val >= 1000.0 ? juce::String(val * 0.001, 1) + " kHz" : juce::String((int)val) + " Hz";
    if (paramId == "glide_time")
        return val <= 0.0 ? "Off" : juce::String((int)val) + " ms";
    if (paramId == "erosion_grit")
        return juce::String((int)val) + " %";
    if (paramId == "punch_slam")
        return juce::String((int)val) + " %";
    if (paramId == "sub_gain")
        return val <= -35.0 ? "-inf dB" : juce::String(val, 1) + " dB";
    if (paramId == "sub_tone")
        return juce::String((int)val) + " %";
    if (paramId == "reverb_space")
        return juce::String((int)val) + " %";
    if (paramId == "master_vol")
        return (val > 0.0 ? "+" : "") + juce::String(val, 1) + " dB";

    return juce::String(val, 1);
}

void ExtasisDonkerAudioProcessorEditor::createKnob(const juce::String& paramId, const juce::String& labelText)
{
    KnobControl ctrl;
    ctrl.slider = std::make_unique<juce::Slider>(juce::Slider::RotaryVerticalDrag, juce::Slider::NoTextBox);
    ctrl.slider->addListener(this);
    ctrl.slider->addMouseListener(this, false);
    ctrl.slider->setName(paramId);
    addAndMakeVisible(*ctrl.slider);

    ctrl.label = std::make_unique<juce::Label>("", labelText);
    ctrl.label->setJustificationType(juce::Justification::centred);
    ctrl.label->setFont(juce::Font(juce::Font::getDefaultMonospacedFontName(), 10.0f, juce::Font::bold));
    ctrl.label->setColour(juce::Label::textColourId, juce::Colour(0xffc8cfdc));
    addAndMakeVisible(*ctrl.label);

    ctrl.valueLabel = std::make_unique<juce::Label>("", getFormattedValueText(paramId, 0.0));
    ctrl.valueLabel->setJustificationType(juce::Justification::centred);
    ctrl.valueLabel->setFont(juce::Font(juce::Font::getDefaultMonospacedFontName(), 9.5f, juce::Font::bold));
    ctrl.valueLabel->setColour(juce::Label::textColourId, ExtasisGUI::TX81ZLookAndFeel::getCyanAccent());
    addAndMakeVisible(*ctrl.valueLabel);

    ctrl.attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.getAPVTS(), paramId, *ctrl.slider);

    controls[paramId] = std::move(ctrl);
}

void ExtasisDonkerAudioProcessorEditor::comboBoxChanged(juce::ComboBox* comboBox)
{
    if (comboBox == &presetBox)
    {
        int idx = presetBox.getSelectedId() - 1;
        processorRef.setCurrentProgram(idx);
        display.setPatchName(presetBox.getText());

        // Update all live value labels
        for (auto& [paramId, ctrl] : controls)
        {
            if (ctrl.slider)
                ctrl.valueLabel->setText(getFormattedValueText(paramId, ctrl.slider->getValue()), juce::dontSendNotification);
        }
    }
}

void ExtasisDonkerAudioProcessorEditor::buttonClicked(juce::Button* button)
{
    if (button == &softClipBtn)
    {
        bool on = softClipBtn.getToggleState();
        display.setParameterReadout("PRE-MASTER SOFTCLIP [CC 23]", on ? "ENABLED (0dB TAPE LIMIT)" : "BYPASS");
    }
}

void ExtasisDonkerAudioProcessorEditor::mouseEnter(const juce::MouseEvent& e)
{
    if (auto* slider = dynamic_cast<juce::Slider*>(e.eventComponent))
    {
        updateParamDisplayForSlider(slider);
    }
}

void ExtasisDonkerAudioProcessorEditor::mouseExit(const juce::MouseEvent&)
{
}

void ExtasisDonkerAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    updateParamDisplayForSlider(slider);
}

void ExtasisDonkerAudioProcessorEditor::updateParamDisplayForSlider(juce::Slider* slider)
{
    if (slider == nullptr)
        return;

    auto paramId = slider->getName();
    auto val = slider->getValue();

    // 1. Update the live value label under the knob
    if (controls.find(paramId) != controls.end() && controls[paramId].valueLabel)
    {
        controls[paramId].valueLabel->setText(getFormattedValueText(paramId, val), juce::dontSendNotification);
    }

    // 2. Update LCD screen readout
    if (paramId == "fm_amount")
        display.setParameterReadout("DONK PUNCH [CC 1]", juce::String((int)val) + "% FM MODULATION");
    else if (paramId == "fm_tune")
        display.setParameterReadout("FM RATIO [CC 14]", juce::String(val, 2) + "x (MOD/CARRIER)");
    else if (paramId == "wave_position")
    {
        if (val < 25.0f)
            display.setParameterReadout("TX WAVE [CC 71]", "PURE SINE (W1) - " + juce::String((int)val) + "%");
        else if (val < 75.0f)
            display.setParameterReadout("TX WAVE [CC 71]", "TX81Z HALF-SINE (W5) - " + juce::String((int)val) + "%");
        else
            display.setParameterReadout("TX WAVE [CC 71]", "FULL RECTIFIED SINE (W3) - " + juce::String((int)val) + "%");
    }
    else if (paramId == "fm_env")
        display.setParameterReadout("DONK TIME [CC 73]", juce::String((int)val) + " ms DECAY");
    else if (paramId == "time_scale")
        display.setParameterReadout("TIME SCALE [CC 12]", juce::String((int)val) + "% ENVELOPE SPEED");
    else if (paramId == "mod_amount")
        display.setParameterReadout("VELOCITY SENS [CC 11]", juce::String((int)val) + "% DYNAMIC RESPONSE");
    else if (paramId == "transient_click")
        display.setParameterReadout("CLICK TRANSIENT [CC 15]", juce::String((int)val) + "% PITCH SNAP");
    else if (paramId == "tx_crunch")
        display.setParameterReadout("TX CRUNCH [CC 16]", juce::String((int)val) + "% FEEDBACK & SAT");
    else if (paramId == "erosion_grit")
        display.setParameterReadout("EROSION DIRT [CC 17]", juce::String((int)val) + "% 12-BIT SIZZLE");
    else if (paramId == "punch_slam")
        display.setParameterReadout("PUNCH SLAM [CC 18]", juce::String((int)val) + "% TRANSIENT OTT COMP");
    else if (paramId == "sub_gain")
        display.setParameterReadout("SUB BODY GAIN [CC 19]", juce::String(val, 1) + " dB (MONO LOCKED)");
    else if (paramId == "sub_tone")
        display.setParameterReadout("SUB TONE [CC 20]", juce::String((int)val) + "% WARMTH");
    else if (paramId == "filter_cutoff")
        display.setParameterReadout("LP FILTER [CC 74]", juce::String((int)val) + " Hz CUTOFF");
    else if (paramId == "reverb_space")
        display.setParameterReadout("TOP SPREAD [CC 21]", juce::String((int)val) + "% STEREO CHORUS (>180Hz)");
    else if (paramId == "glide_time")
        display.setParameterReadout("PORTAMENTO [CC 22]", val <= 0.0f ? "OFF" : juce::String((int)val) + " ms GLIDE");
    else if (paramId == "master_vol")
        display.setParameterReadout("MASTER OUTPUT [CC 7]", juce::String(val, 1) + " dB");
}

void ExtasisDonkerAudioProcessorEditor::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // 1. Chassis: Dark Brushed Metal Rack
    g.setColour(ExtasisGUI::TX81ZLookAndFeel::getChassisColor());
    g.fillRect(bounds);

    // Bevel borders
    g.setColour(juce::Colour(0xff2a2d33));
    g.drawRect(bounds.reduced(0.5f), 1.0f);

    // 2. Rack Mount Ears (Left & Right 38px)
    auto leftEar = bounds.removeFromLeft(38.0f);
    auto rightEar = bounds.removeFromRight(38.0f);

    g.setColour(juce::Colour(0xff121417));
    g.fillRect(leftEar);
    g.fillRect(rightEar);

    g.setColour(juce::Colour(0xff2b2f36));
    g.drawVerticalLine((int)leftEar.getRight(), 0.0f, bounds.getBottom());
    g.drawVerticalLine((int)rightEar.getX(), 0.0f, bounds.getBottom());

    // Screw Holes & Rack Screws
    auto drawScrew = [&g](float x, float y) {
        g.setColour(juce::Colour(0xff090a0c));
        g.fillEllipse(x - 9.0f, y - 9.0f, 18.0f, 18.0f);
        g.setColour(juce::Colour(0xff3a3f48));
        g.drawEllipse(x - 9.0f, y - 9.0f, 18.0f, 18.0f, 1.5f);
        g.setColour(juce::Colour(0xff555d6b));
        g.fillEllipse(x - 6.0f, y - 6.0f, 12.0f, 12.0f);
        g.setColour(juce::Colour(0xff1c1e22));
        g.drawLine(x - 4.0f, y, x + 4.0f, y, 2.0f);
        g.drawLine(x, y - 4.0f, x, y + 4.0f, 2.0f);
    };

    drawScrew(leftEar.getCentreX(), 35.0f);
    drawScrew(leftEar.getCentreX(), getHeight() - 35.0f);
    drawScrew(rightEar.getCentreX(), 35.0f);
    drawScrew(rightEar.getCentreX(), getHeight() - 35.0f);

    // 3. Top Header: Vintage Silkscreen Logo & Badges
    auto headerArea = bounds.removeFromTop(48.0f);
    
    // Logo
    g.setColour(ExtasisGUI::TX81ZLookAndFeel::getCyanAccent());
    g.setFont(juce::Font(juce::Font::getDefaultMonospacedFontName(), 18.0f, juce::Font::bold));
    g.drawText("EXTASIS DONKER", 50, 12, 200, 24, juce::Justification::left);

    g.setColour(juce::Colour(0xff8e96a4));
    g.setFont(juce::Font(juce::Font::getDefaultMonospacedFontName(), 10.5f, juce::Font::bold));
    g.drawText("- FM BASS SYNTHESIZER // TX81Z ARCHITECTURE // LIVE VALUES DISPLAY", 230, 15, 480, 20, juce::Justification::left);

    // Right Brand Decal
    g.setColour(ExtasisGUI::TX81ZLookAndFeel::getCyanAccent());
    g.drawText("|||||||| TX-CORE DSP", getWidth() - 250, 14, 200, 20, juce::Justification::right);

    // Cyan horizontal accent strip
    g.setColour(ExtasisGUI::TX81ZLookAndFeel::getCyanAccent());
    g.fillRect(48, 42, getWidth() - 96, 2);

    // 4. Section Framing & Panels
    auto drawSectionBox = [&g](juce::Rectangle<float> r, const juce::String& title) {
        g.setColour(juce::Colour(0xff1b1e22));
        g.fillRoundedRectangle(r, 4.0f);
        g.setColour(juce::Colour(0xff333842));
        g.drawRoundedRectangle(r, 4.0f, 1.0f);

        // Section Title Header
        g.setColour(ExtasisGUI::TX81ZLookAndFeel::getCyanAccent());
        g.setFont(juce::Font(juce::Font::getDefaultMonospacedFontName(), 10.5f, juce::Font::bold));
        g.drawText(title, (int)r.getX() + 8, (int)r.getY() + 5, (int)r.getWidth() - 16, 16, juce::Justification::left);

        g.setColour(juce::Colour(0x3300d4d4));
        g.drawHorizontalLine((int)r.getY() + 22, r.getX() + 8.0f, r.getRight() - 8.0f);
    };

    // 4 Distinct Sections
    drawSectionBox(juce::Rectangle<float>(48, 230, 360, 285), "1. FM SYNTHESIS (THE DONK)");
    drawSectionBox(juce::Rectangle<float>(416, 230, 200, 285), "2. TRANSIENT & TONE");
    drawSectionBox(juce::Rectangle<float>(624, 230, 160, 285), "3. PRE-MASTER FX");
    drawSectionBox(juce::Rectangle<float>(792, 230, 140, 285), "4. SUB & SPACE");
}

void ExtasisDonkerAudioProcessorEditor::resized()
{
    // Display in the center top
    display.setBounds(48, 52, 640, 168);

    // Top Right Controls (Preset Selector + Audition Trigger)
    presetBox.setBounds(700, 56, 180, 28);
    prevPresetBtn.setBounds(885, 56, 28, 28);
    nextPresetBtn.setBounds(917, 56, 28, 28);

    // Audition Trigger Button below Preset Box
    triggerBtn.setBounds(700, 92, 245, 34);

    auto placeKnob = [this](const juce::String& id, int x, int y, int w, int h) {
        if (controls.find(id) != controls.end())
        {
            controls[id].slider->setBounds(x, y, w, h - 28);
            controls[id].label->setBounds(x - 6, y + h - 28, w + 12, 14);
            controls[id].valueLabel->setBounds(x - 6, y + h - 14, w + 12, 14);
        }
    };

    // Section 1: FM Core
    placeKnob("fm_amount", 58, 258, 62, 88);
    placeKnob("fm_tune", 128, 258, 62, 88);
    placeKnob("fm_env", 198, 258, 62, 88);
    placeKnob("wave_position", 268, 258, 62, 88);
    placeKnob("time_scale", 338, 258, 62, 88);

    placeKnob("mod_amount", 98, 382, 62, 88);
    placeKnob("tx_crunch", 178, 382, 62, 88);
    placeKnob("transient_click", 258, 382, 62, 88);

    // Section 2: Transient & Tone
    placeKnob("filter_cutoff", 432, 258, 68, 88);
    placeKnob("glide_time", 522, 258, 68, 88);

    // Section 3: Pre-Master FX (Erosion & Slam + Soft Clip button)
    placeKnob("erosion_grit", 640, 258, 62, 88);
    placeKnob("punch_slam", 710, 258, 62, 88);
    softClipBtn.setBounds(642, 400, 125, 34);

    // Section 4: Sub & Space
    placeKnob("sub_gain", 802, 258, 60, 88);
    placeKnob("sub_tone", 864, 258, 60, 88);
    placeKnob("reverb_space", 802, 382, 60, 88);
    placeKnob("master_vol", 864, 382, 60, 88);
}
