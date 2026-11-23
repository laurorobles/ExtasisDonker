#include "PluginEditor.h"

ExtasisDonkerAudioProcessorEditor::ExtasisDonkerAudioProcessorEditor(ExtasisDonkerAudioProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p), display(p)
{
    setLookAndFeel(&customLookAndFeel);

    // 1. Display Setup
    addAndMakeVisible(display);

    // 2. Preset Selector & Navigation
    refreshPresetList();
    presetBox.setSelectedId(processorRef.getCurrentProgram() + 1, juce::dontSendNotification);
    presetBox.addListener(this);
    addAndMakeVisible(presetBox);

    prevPresetBtn.onClick = [this]() {
        if (presetBox.getNumItems() <= 0) return;
        int cur = presetBox.getSelectedId() - 1;
        int next = (cur - 1 + presetBox.getNumItems()) % presetBox.getNumItems();
        presetBox.setSelectedId(next + 1, juce::sendNotification);
    };
    addAndMakeVisible(prevPresetBtn);

    nextPresetBtn.onClick = [this]() {
        if (presetBox.getNumItems() <= 0) return;
        int cur = presetBox.getSelectedId() - 1;
        int next = (cur + 1) % presetBox.getNumItems();
        presetBox.setSelectedId(next + 1, juce::sendNotification);
    };
    addAndMakeVisible(nextPresetBtn);

    // Native Save Preset Overlay (replaces OS AlertWindow)
    addChildComponent(savePresetOverlay);
    savePresetOverlay.onSave = [this](const juce::String& name) {
        savePresetOverlay.setVisible(false);
        if (name.isNotEmpty() && processorRef.saveUserPreset(name))
        {
            refreshPresetList();
            auto names = processorRef.getAllPresetNames();
            for (int i = 0; i < names.size(); ++i)
            {
                if (names[i] == "[User] " + name)
                {
                    presetBox.setSelectedId(i + 1, juce::sendNotification);
                    break;
                }
            }
            display.setParameterReadout("PRESET SAVED", name.toUpperCase());
        }
        repaint();
    };
    savePresetOverlay.onCancel = [this]() {
        savePresetOverlay.setVisible(false);
        repaint();
    };

    savePresetBtn.onClick = [this]() {
        savePresetOverlay.prepareToShow("Custom Donk");
    };
    addAndMakeVisible(savePresetBtn);

    // 3. Audition Trigger Button (Large Centered Logo Pad)
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

    // 5. Create all Knobs with Live Value Labels, Double-Click Default Reset & MouseListeners
    createKnob("fm_amount", "DONK PUNCH");
    createKnob("fm_tune", "FM RATIO");
    createKnob("wave_position", "TX WAVE");
    createKnob("mod_amount", "VEL SENS");
    createKnob("fm_env", "DONK TIME");
    createKnob("pump_amount", "AUTO PUMP");
    createKnob("tx_crunch", "TX CRUNCH");
    createKnob("pitch_drop", "PITCH DROP");

    createKnob("filter_cutoff", "LP FILTER");
    createKnob("filter_reso", "RESONANCE");
    createKnob("glide_time", "GLIDE");

    createKnob("erosion_grit", "EROSION");
    createKnob("punch_slam", "PUNCH SLAM");

    createKnob("sub_gain", "SUB GAIN");
    createKnob("sub_tone", "SUB TONE");
    createKnob("reverb_space", "TOP SPREAD");
    createKnob("master_vol", "MASTER OUT");

    // Initialize all value labels
    for (auto& [paramId, ctrl] : controls)
    {
        if (ctrl.slider)
            ctrl.valueLabel->setText(getFormattedValueText(paramId, ctrl.slider->getValue()), juce::dontSendNotification);
    }

    display.setPatchName(presetBox.getText());

    setSize(700, 400);
    setResizable(true, true);
    setResizeLimits(350, 200, 1400, 800);
    getConstrainer()->setFixedAspectRatio(700.0 / 400.0);
}

ExtasisDonkerAudioProcessorEditor::~ExtasisDonkerAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

void ExtasisDonkerAudioProcessorEditor::refreshPresetList()
{
    int currentId = presetBox.getSelectedId();
    presetBox.clear(juce::dontSendNotification);
    auto presetNames = processorRef.getAllPresetNames();
    for (int i = 0; i < presetNames.size(); ++i)
        presetBox.addItem(presetNames[i], i + 1);

    if (currentId > 0 && currentId <= presetNames.size())
        presetBox.setSelectedId(currentId, juce::dontSendNotification);
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
    if (paramId == "pump_amount")
        return juce::String((int)val) + " %";
    if (paramId == "mod_amount")
        return juce::String((int)val) + " %";
    if (paramId == "pitch_drop")
        return juce::String((int)val) + " %";
    if (paramId == "tx_crunch")
        return juce::String((int)val) + " %";
    if (paramId == "filter_cutoff")
        return val >= 1000.0 ? juce::String(val * 0.001, 1) + " kHz" : juce::String((int)val) + " Hz";
    if (paramId == "filter_reso")
        return juce::String((int)val) + " %";
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

    if (auto* param = processorRef.getAPVTS().getParameter(paramId))
        ctrl.slider->setDoubleClickReturnValue(true, param->convertFrom0to1(param->getDefaultValue()));

    addAndMakeVisible(*ctrl.slider);

    ctrl.label = std::make_unique<juce::Label>("", labelText);
    ctrl.label->setJustificationType(juce::Justification::centred);
    ctrl.label->setFont(juce::Font(juce::FontOptions(juce::Font::getDefaultMonospacedFontName(), 8.5f, juce::Font::bold)));
    ctrl.label->setColour(juce::Label::textColourId, juce::Colour(0xffc8cfdc));
    addAndMakeVisible(*ctrl.label);

    ctrl.valueLabel = std::make_unique<juce::Label>("", getFormattedValueText(paramId, 0.0));
    ctrl.valueLabel->setJustificationType(juce::Justification::centred);
    ctrl.valueLabel->setFont(juce::Font(juce::FontOptions(juce::Font::getDefaultMonospacedFontName(), 9.0f, juce::Font::plain)));
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
        updateParamDisplayForSlider(slider);
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

    if (controls.find(paramId) != controls.end() && controls[paramId].valueLabel)
        controls[paramId].valueLabel->setText(getFormattedValueText(paramId, val), juce::dontSendNotification);

    if (paramId == "fm_amount")
        display.setParameterReadout("DONK PUNCH [CC 13]", juce::String((int)val) + "% FM MODULATION");
    else if (paramId == "fm_tune")
        display.setParameterReadout("FM RATIO [CC 14]", juce::String(val, 2) + "x (MOD/CARRIER)");
    else if (paramId == "wave_position")
    {
        if (val < 25.0)
            display.setParameterReadout("TX WAVE [CC 71]", "PURE SINE (W1) - " + juce::String((int)val) + "%");
        else if (val < 75.0)
            display.setParameterReadout("TX WAVE [CC 71]", "TX81Z HALF-SINE (W5) - " + juce::String((int)val) + "%");
        else
            display.setParameterReadout("TX WAVE [CC 71]", "FULL RECTIFIED SINE (W3) - " + juce::String((int)val) + "%");
    }
    else if (paramId == "fm_env")
        display.setParameterReadout("DONK TIME [CC 73]", juce::String((int)val) + " ms FM DECAY");
    else if (paramId == "pump_amount")
        display.setParameterReadout("AUTO PUMP [CC 12]", juce::String((int)val) + "% 1/4-NOTE SIDECHAIN");
    else if (paramId == "mod_amount")
        display.setParameterReadout("VELOCITY SENS [CC 11]", juce::String((int)val) + "% DYNAMIC RESPONSE");
    else if (paramId == "pitch_drop")
        display.setParameterReadout("PITCH DROP [CC 15]", juce::String((int)val) + "% LASER TRANSIENT SNAP");
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
        display.setParameterReadout("LP FILTER [CC 74]", juce::String((int)val) + " Hz CUTOFF (MOD=CC1)");
    else if (paramId == "filter_reso")
        display.setParameterReadout("RESONANCE [CC 75]", juce::String((int)val) + "% SVF PEAK");
    else if (paramId == "reverb_space")
        display.setParameterReadout("TOP SPREAD [CC 21]", juce::String((int)val) + "% STEREO CHORUS (>180Hz)");
    else if (paramId == "glide_time")
        display.setParameterReadout("PORTAMENTO [CC 22]", val <= 0.0 ? "OFF" : juce::String((int)val) + " ms GLIDE");
    else if (paramId == "master_vol")
        display.setParameterReadout("MASTER OUTPUT [CC 7]", juce::String(val, 1) + " dB");
}

void ExtasisDonkerAudioProcessorEditor::paint(juce::Graphics& g)
{
    constexpr float baseW = 700.0f;
    constexpr float baseH = 400.0f;
    auto bounds = juce::Rectangle<float>(0.0f, 0.0f, baseW, baseH);
    g.addTransform(juce::AffineTransform::scale((float)getWidth() / baseW, (float)getHeight() / baseH));

    // 1. Chassis: Dark Brushed Metal Rack
    g.setColour(ExtasisGUI::TX81ZLookAndFeel::getChassisColor());
    g.fillRect(bounds);

    g.setColour(juce::Colour(0xff2a2d33));
    g.drawRect(bounds.reduced(0.5f), 1.0f);

    // 2. Rack Mount Ears (Left & Right 38px)
    auto leftEar = bounds.removeFromLeft(38.0f);
    auto rightEar = bounds.removeFromRight(38.0f);

    g.setColour(juce::Colour(0xff121417));
    g.fillRect(leftEar);
    g.fillRect(rightEar);

    g.setColour(juce::Colour(0xff2b2f36));
    g.drawVerticalLine((int)leftEar.getRight(), 0.0f, baseH);
    g.drawVerticalLine((int)rightEar.getX(), 0.0f, baseH);

    // Screw Holes (using virtual 700x400 coordinates so scaling stays exact)
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
    drawScrew(leftEar.getCentreX(), baseH - 35.0f);
    drawScrew(rightEar.getCentreX(), 35.0f);
    drawScrew(rightEar.getCentreX(), baseH - 35.0f);

    // 3. Top Header: Title, Subtitle, Open Source Badge
    g.setColour(ExtasisGUI::TX81ZLookAndFeel::getCyanAccent());
    g.setFont(juce::Font(juce::FontOptions(juce::Font::getDefaultMonospacedFontName(), 17.0f, juce::Font::bold)));
    g.drawText("EXTASIS DONKER", 48, 7, 185, 20, juce::Justification::left);

    g.setColour(juce::Colour(0xff8e96a4));
    g.setFont(juce::Font(juce::FontOptions(juce::Font::getDefaultMonospacedFontName(), 10.0f, juce::Font::bold)));
    g.drawText("// TX81Z FM BASS SYNTHESIZER", 210, 9, 260, 18, juce::Justification::left);

    // Subtle Open Source Edition Hardware Decal (strictly aligned to right margin 652px)
    g.setColour(juce::Colour(0xff4a5360));
    g.setFont(juce::Font(juce::FontOptions(juce::Font::getDefaultMonospacedFontName(), 8.5f, juce::Font::bold)));
    g.drawText("OPEN SOURCE v4.0", 512, 10, 140, 16, juce::Justification::right);

    // Discreet Credit at bottom-right
    g.setColour(juce::Colour(0xff323842));
    g.setFont(juce::Font(juce::FontOptions(juce::Font::getDefaultMonospacedFontName(), 7.5f, juce::Font::plain)));
    g.drawText("coded by @laurorobles", 542, (int)(baseH - 13.0f), 110, 10, juce::Justification::right);

    // Cyan horizontal accent strip (strictly 48..652 -> width 604)
    g.setColour(ExtasisGUI::TX81ZLookAndFeel::getCyanAccent());
    g.fillRect(48.0f, 34.0f, baseW - 96.0f, 2.0f);

    // 4. Section Framing & Panels
    auto drawSectionBox = [&g](juce::Rectangle<float> r, const juce::String& title, bool isCore = false) {
        g.setColour(isCore ? juce::Colour(0xff14171a) : juce::Colour(0xff181a1e));
        g.fillRoundedRectangle(r, 4.0f);
        g.setColour(isCore ? juce::Colour(0x4000d2ff) : juce::Colour(0xff2d3138));
        g.drawRoundedRectangle(r, 4.0f, 1.5f);

        // Section Title Header
        g.setColour(ExtasisGUI::TX81ZLookAndFeel::getCyanAccent());
        g.setFont(juce::Font(juce::FontOptions(juce::Font::getDefaultMonospacedFontName(), 10.0f, juce::Font::bold)));
        g.drawText(title, (int)r.getX() + 8, (int)r.getY() + 4, (int)r.getWidth() - 16, 16, juce::Justification::left);

        g.setColour(juce::Colour(0x3300d4d4));
        g.drawHorizontalLine((int)r.getY() + 20, r.getX() + 8.0f, r.getRight() - 8.0f);
    };

    // 4 Sections - symmetric margins (left ear at 38px, content at 48px, right content ends at 652px)
    drawSectionBox(juce::Rectangle<float>(48, 184, 248, 202), "1. FM SYNTHESIS CORE", true);
    drawSectionBox(juce::Rectangle<float>(302, 184, 64, 202), "2. FILTER");
    drawSectionBox(juce::Rectangle<float>(372, 184, 126, 202), "3. TONE & FX");
    drawSectionBox(juce::Rectangle<float>(504, 184, 148, 202), "4. SUB & SPACE");
}

void ExtasisDonkerAudioProcessorEditor::resized()
{
    float scale = (float)getWidth() / 700.0f;

    // Global App Bounds
    auto bounds = getLocalBounds();
    savePresetOverlay.setBounds(bounds);

    // Display in the center top
    display.setBounds((int)(48 * scale), (int)(42 * scale), (int)(420 * scale), (int)(130 * scale));

    // Preset Selector, Navigation & Save Buttons
    // Total available width: 652 - 480 = 172.
    presetBox.setBounds((int)(480 * scale), (int)(42 * scale), (int)(98 * scale), (int)(26 * scale));
    prevPresetBtn.setBounds((int)(580 * scale), (int)(42 * scale), (int)(22 * scale), (int)(26 * scale));
    nextPresetBtn.setBounds((int)(604 * scale), (int)(42 * scale), (int)(22 * scale), (int)(26 * scale));
    savePresetBtn.setBounds((int)(628 * scale), (int)(42 * scale), (int)(24 * scale), (int)(26 * scale));

    // Large Centered Logo Audition Trigger Pad right below Presets (ends at 652)
    triggerBtn.setBounds((int)(480 * scale), (int)(74 * scale), (int)(172 * scale), (int)(98 * scale));

    auto placeKnob = [this, scale](const juce::String& id, int x, int y, int w, int h) {
        auto it = controls.find(id);
        if (it != controls.end())
        {
            auto& ctrl = it->second;
            ctrl.slider->setBounds((int)(x * scale), (int)(y * scale), (int)(w * scale), (int)((h - 24) * scale));
            ctrl.label->setBounds((int)((x - 10) * scale), (int)((y + h - 24) * scale), (int)((w + 20) * scale), (int)(12 * scale));
            ctrl.valueLabel->setBounds((int)((x - 10) * scale), (int)((y + h - 12) * scale), (int)((w + 20) * scale), (int)(12 * scale));
            ctrl.label->setFont(juce::Font(juce::FontOptions(juce::Font::getDefaultMonospacedFontName(), juce::jmax(6.5f, 8.5f * scale), juce::Font::bold)));
            ctrl.valueLabel->setFont(juce::Font(juce::FontOptions(juce::Font::getDefaultMonospacedFontName(), juce::jmax(6.5f, 9.0f * scale), juce::Font::plain)));
        }
    };

    const int knobW = 50;
    const int knobH = 74;

    // Section 1: FM Core (4x2 grid, x=48, w=248, ends=296)
    placeKnob("fm_amount",     52, 204, knobW, knobH);
    placeKnob("fm_tune",      114, 204, knobW, knobH);
    placeKnob("wave_position",176, 204, knobW, knobH);
    placeKnob("mod_amount",   238, 204, knobW, knobH);

    placeKnob("fm_env",        52, 296, knobW, knobH);
    placeKnob("pump_amount",  114, 296, knobW, knobH);
    placeKnob("tx_crunch",    176, 296, knobW, knobH);
    placeKnob("pitch_drop",   238, 296, knobW, knobH);

    // Section 2: Filter (1x2 grid, x=302, w=64)
    placeKnob("filter_cutoff", 309, 204, knobW, knobH);
    placeKnob("filter_reso",   309, 296, knobW, knobH);

    // Section 3: Tone & FX (2x2 grid, x=372, w=126, ends=498)
    placeKnob("glide_time",   378, 204, knobW, knobH);
    placeKnob("erosion_grit", 440, 204, knobW, knobH);
    placeKnob("punch_slam",   378, 296, knobW, knobH);
    softClipBtn.setBounds((int)(440 * scale), (int)(316 * scale), (int)(50 * scale), (int)(26 * scale));

    // Section 4: Sub & Space (2x2 grid, x=504, w=148, ends=652)
    placeKnob("sub_gain",     520, 204, knobW, knobH);
    placeKnob("sub_tone",     586, 204, knobW, knobH);
    placeKnob("reverb_space", 520, 296, knobW, knobH);
    placeKnob("master_vol",   586, 296, knobW, knobH);
}
