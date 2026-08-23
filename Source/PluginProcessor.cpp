#include "PluginProcessor.h"
#include "PluginEditor.h"

ExtasisDonkerAudioProcessor::ExtasisDonkerAudioProcessor()
    : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout())
{
}

ExtasisDonkerAudioProcessor::~ExtasisDonkerAudioProcessor()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout ExtasisDonkerAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // Macro 1: DONK AMOUNT (CC 1 ModWheel / CC 13)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("fm_amount", 1), "Donk Punch",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.5f), 80.0f, "%"));

    // Macro 2: FM RATIO / TUNE (CC 14)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("fm_tune", 1), "FM Ratio",
        juce::NormalisableRange<float>(0.5f, 8.0f, 0.01f), 2.0f, "x"));

    // Macro 3: TX WAVE (CC 71)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("wave_position", 1), "TX Wave",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.5f), 50.0f, "%"));

    // Macro 4: FM DECAY (CC 73)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("fm_env", 1), "Donk Decay",
        juce::NormalisableRange<float>(20.0f, 350.0f, 1.0f, 0.5f), 95.0f, "ms"));

    // Macro 5: TIME SCALE (CC 12)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("time_scale", 1), "Time Scale",
        juce::NormalisableRange<float>(-100.0f, 100.0f, 1.0f), 0.0f, "%"));

    // Macro 6: MOD SENS / VELOCITY (CC 11)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("mod_amount", 1), "Mod Velocity",
        juce::NormalisableRange<float>(0.0f, 200.0f, 1.0f), 100.0f, "%"));

    // Macro 7: TRANSIENT CLICK (CC 15)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("transient_click", 1), "Transient Click",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f), 55.0f, "%"));

    // Macro 8: TX CRUNCH / FEEDBACK (CC 16)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("tx_crunch", 1), "TX Crunch",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f), 25.0f, "%"));

    // Pre-Master FX: EROSION / DIRT (CC 17)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("erosion_grit", 1), "Erosion Grit",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f), 20.0f, "%"));

    // Pre-Master FX: PUNCH SLAM (CC 18)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("punch_slam", 1), "Punch Slam",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f), 35.0f, "%"));

    // Pre-Master FX: SOFT CLIP (CC 23)
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID("soft_clip", 1), "Soft Clip", true));

    // Macro 9: SUB GAIN (CC 19)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("sub_gain", 1), "Sub Gain",
        juce::NormalisableRange<float>(-36.0f, 0.0f, 0.5f), -9.0f, "dB"));

    // Macro 10: SUB TONE (CC 20)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("sub_tone", 1), "Sub Tone",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f), 15.0f, "%"));

    // Macro 11: FILTER CUTOFF (CC 74)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("filter_cutoff", 1), "Filter Cutoff",
        juce::NormalisableRange<float>(200.0f, 20000.0f, 1.0f, 0.35f), 16000.0f, "Hz"));

    // Macro 12: REVERB / TOP SPREAD (CC 21)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("reverb_space", 1), "Space / Spread",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f), 20.0f, "%"));

    // Macro 13: PORTAMENTO GLIDE (CC 22)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("glide_time", 1), "Glide Time",
        juce::NormalisableRange<float>(0.0f, 200.0f, 1.0f), 0.0f, "ms"));

    // Macro 14: MASTER VOL (CC 7)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("master_vol", 1), "Master Out",
        juce::NormalisableRange<float>(-36.0f, 6.0f, 0.5f), 0.0f, "dB"));

    return { params.begin(), params.end() };
}

const juce::String ExtasisDonkerAudioProcessor::getName() const { return JucePlugin_Name; }
bool ExtasisDonkerAudioProcessor::acceptsMidi() const { return true; }
bool ExtasisDonkerAudioProcessor::producesMidi() const { return false; }
bool ExtasisDonkerAudioProcessor::isMidiEffect() const { return false; }
double ExtasisDonkerAudioProcessor::getTailLengthSeconds() const { return 0.2; }

juce::StringArray ExtasisDonkerAudioProcessor::getPresetNames()
{
    return {
        "01. Lately 1987 (TX81Z)",
        "02. Basic FM House (Robin S)",
        "03. Russian Hardbass Punch",
        "04. UK Bounce Donk",
        "05. Tech House Subby",
        "06. Speed Garage Organ",
        "07. Metallic Monster",
        "08. Deep Afterhours"
    };
}

int ExtasisDonkerAudioProcessor::getNumPrograms() { return getPresetNames().size(); }
int ExtasisDonkerAudioProcessor::getCurrentProgram() { return currentProgramIndex; }
const juce::String ExtasisDonkerAudioProcessor::getProgramName(int index)
{
    auto names = getPresetNames();
    if (juce::isPositiveAndBelow(index, names.size()))
        return names[index];
    return {};
}
void ExtasisDonkerAudioProcessor::changeProgramName(int, const juce::String&) {}

void ExtasisDonkerAudioProcessor::setCurrentProgram(int index)
{
    if (juce::isPositiveAndBelow(index, getNumPrograms()))
    {
        currentProgramIndex = index;
        loadPreset(index);
    }
}

void ExtasisDonkerAudioProcessor::triggerAuditionNote(int noteNumber, float velocity)
{
    synth.triggerNote(noteNumber, velocity, false);
}

void ExtasisDonkerAudioProcessor::releaseAuditionNote(int noteNumber)
{
    juce::MidiMessage off = juce::MidiMessage::noteOff(1, noteNumber);
    synth.handleMidiEvent(off);
}

void ExtasisDonkerAudioProcessor::loadPreset(int presetIndex)
{
    auto setP = [this](const juce::String& id, float val) {
        if (auto* param = apvts.getParameter(id))
            param->setValueNotifyingHost(param->convertTo0to1(val));
    };

    switch (presetIndex)
    {
        case 0: // 01. Lately 1987 (TX81Z)
            setP("fm_amount", 80.0f); setP("fm_tune", 2.0f); setP("wave_position", 50.0f);
            setP("fm_env", 95.0f); setP("time_scale", 0.0f); setP("mod_amount", 110.0f);
            setP("transient_click", 55.0f); setP("tx_crunch", 30.0f); setP("sub_gain", -9.0f);
            setP("sub_tone", 15.0f); setP("filter_cutoff", 16000.0f); setP("reverb_space", 15.0f);
            setP("erosion_grit", 15.0f); setP("punch_slam", 30.0f); setP("soft_clip", 1.0f);
            setP("glide_time", 0.0f);
            break;

        case 1: // 02. Basic FM House (Robin S)
            setP("fm_amount", 88.0f); setP("fm_tune", 2.0f); setP("wave_position", 10.0f);
            setP("fm_env", 120.0f); setP("time_scale", 0.0f); setP("mod_amount", 100.0f);
            setP("transient_click", 65.0f); setP("tx_crunch", 15.0f); setP("sub_gain", -6.0f);
            setP("sub_tone", 20.0f); setP("filter_cutoff", 18000.0f); setP("reverb_space", 25.0f);
            setP("erosion_grit", 20.0f); setP("punch_slam", 40.0f); setP("soft_clip", 1.0f);
            setP("glide_time", 15.0f);
            break;

        case 2: // 03. Russian Hardbass Punch
            setP("fm_amount", 100.0f); setP("fm_tune", 2.0f); setP("wave_position", 80.0f);
            setP("fm_env", 75.0f); setP("time_scale", -15.0f); setP("mod_amount", 140.0f);
            setP("transient_click", 95.0f); setP("tx_crunch", 50.0f); setP("sub_gain", -12.0f);
            setP("sub_tone", 35.0f); setP("filter_cutoff", 20000.0f); setP("reverb_space", 10.0f);
            setP("erosion_grit", 45.0f); setP("punch_slam", 70.0f); setP("soft_clip", 1.0f);
            setP("glide_time", 0.0f);
            break;

        case 3: // 04. UK Bounce Donk
            setP("fm_amount", 95.0f); setP("fm_tune", 3.0f); setP("wave_position", 60.0f);
            setP("fm_env", 85.0f); setP("time_scale", 0.0f); setP("mod_amount", 120.0f);
            setP("transient_click", 80.0f); setP("tx_crunch", 35.0f); setP("sub_gain", -8.0f);
            setP("sub_tone", 25.0f); setP("filter_cutoff", 19000.0f); setP("reverb_space", 30.0f);
            setP("erosion_grit", 30.0f); setP("punch_slam", 50.0f); setP("soft_clip", 1.0f);
            setP("glide_time", 20.0f);
            break;

        case 4: // 05. Tech House Subby
            setP("fm_amount", 65.0f); setP("fm_tune", 1.0f); setP("wave_position", 25.0f);
            setP("fm_env", 130.0f); setP("time_scale", 5.0f); setP("mod_amount", 80.0f);
            setP("transient_click", 40.0f); setP("tx_crunch", 10.0f); setP("sub_gain", -3.0f);
            setP("sub_tone", 10.0f); setP("filter_cutoff", 12000.0f); setP("reverb_space", 15.0f);
            setP("erosion_grit", 10.0f); setP("punch_slam", 25.0f); setP("soft_clip", 1.0f);
            setP("glide_time", 10.0f);
            break;

        case 5: // 06. Speed Garage Organ
            setP("fm_amount", 90.0f); setP("fm_tune", 4.0f); setP("wave_position", 40.0f);
            setP("fm_env", 110.0f); setP("time_scale", 0.0f); setP("mod_amount", 110.0f);
            setP("transient_click", 50.0f); setP("tx_crunch", 25.0f); setP("sub_gain", -7.0f);
            setP("sub_tone", 15.0f); setP("filter_cutoff", 17000.0f); setP("reverb_space", 35.0f);
            setP("erosion_grit", 25.0f); setP("punch_slam", 35.0f); setP("soft_clip", 1.0f);
            setP("glide_time", 25.0f);
            break;

        case 6: // 07. Metallic Monster
            setP("fm_amount", 100.0f); setP("fm_tune", 5.0f); setP("wave_position", 90.0f);
            setP("fm_env", 65.0f); setP("time_scale", -20.0f); setP("mod_amount", 150.0f);
            setP("transient_click", 90.0f); setP("tx_crunch", 70.0f); setP("sub_gain", -10.0f);
            setP("sub_tone", 40.0f); setP("filter_cutoff", 20000.0f); setP("reverb_space", 40.0f);
            setP("erosion_grit", 60.0f); setP("punch_slam", 65.0f); setP("soft_clip", 1.0f);
            setP("glide_time", 0.0f);
            break;

        case 7: // 08. Deep Afterhours
            setP("fm_amount", 50.0f); setP("fm_tune", 1.0f); setP("wave_position", 0.0f);
            setP("fm_env", 160.0f); setP("time_scale", 10.0f); setP("mod_amount", 70.0f);
            setP("transient_click", 30.0f); setP("tx_crunch", 5.0f); setP("sub_gain", -4.0f);
            setP("sub_tone", 5.0f); setP("filter_cutoff", 8000.0f); setP("reverb_space", 20.0f);
            setP("erosion_grit", 0.0f); setP("punch_slam", 15.0f); setP("soft_clip", 1.0f);
            setP("glide_time", 35.0f);
            break;
    }
}

void ExtasisDonkerAudioProcessor::handleMidiCC(int ccNumber, int ccValue)
{
    float normVal = (float)ccValue / 127.0f;
    auto mapParam = [this, normVal](const juce::String& paramId) {
        if (auto* param = apvts.getParameter(paramId))
            param->setValueNotifyingHost(normVal);
    };

    switch (ccNumber)
    {
        case 1:
        case 13: mapParam("fm_amount"); break;
        case 14: mapParam("fm_tune"); break;
        case 71: mapParam("wave_position"); break;
        case 73: mapParam("fm_env"); break;
        case 12: mapParam("time_scale"); break;
        case 11: mapParam("mod_amount"); break;
        case 15: mapParam("transient_click"); break;
        case 16: mapParam("tx_crunch"); break;
        case 17: mapParam("erosion_grit"); break;
        case 18: mapParam("punch_slam"); break;
        case 19: mapParam("sub_gain"); break;
        case 20: mapParam("sub_tone"); break;
        case 74: mapParam("filter_cutoff"); break;
        case 21: mapParam("reverb_space"); break;
        case 22: mapParam("glide_time"); break;
        case 23:
            if (auto* param = apvts.getParameter("soft_clip"))
                param->setValueNotifyingHost(normVal >= 0.5f ? 1.0f : 0.0f);
            break;
        case 7:  mapParam("master_vol"); break;
    }

    if (onMidiCCReceived)
        onMidiCCReceived(ccNumber, normVal);
}

void ExtasisDonkerAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = (juce::uint32)samplesPerBlock;
    spec.numChannels = (juce::uint32)getTotalNumOutputChannels();

    synth.prepare(spec);
}

void ExtasisDonkerAudioProcessor::releaseResources()
{
    synth.reset();
}

bool ExtasisDonkerAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    return true;
}

void ExtasisDonkerAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    // Handle incoming MIDI (Notes & CCs)
    for (const auto metadata : midiMessages)
    {
        auto msg = metadata.getMessage();
        if (msg.isController())
        {
            handleMidiCC(msg.getControllerNumber(), msg.getControllerValue());
        }
        else
        {
            synth.handleMidiEvent(msg);
        }
    }

    float fmAmount = apvts.getRawParameterValue("fm_amount")->load() * 0.01f;
    float fmTune = apvts.getRawParameterValue("fm_tune")->load();
    float wavePosition = apvts.getRawParameterValue("wave_position")->load() * 0.01f;
    float fmEnvDecay = apvts.getRawParameterValue("fm_env")->load();
    float timeScale = apvts.getRawParameterValue("time_scale")->load() * 0.01f;
    float modAmount = apvts.getRawParameterValue("mod_amount")->load() * 0.01f;
    float transientClick = apvts.getRawParameterValue("transient_click")->load() * 0.01f;
    float txCrunch = apvts.getRawParameterValue("tx_crunch")->load() * 0.01f;
    float erosionGrit = apvts.getRawParameterValue("erosion_grit")->load() * 0.01f;
    float punchSlam = apvts.getRawParameterValue("punch_slam")->load() * 0.01f;
    bool enableSoftClip = apvts.getRawParameterValue("soft_clip")->load() > 0.5f;
    float subGain = apvts.getRawParameterValue("sub_gain")->load();
    float subTone = apvts.getRawParameterValue("sub_tone")->load() * 0.01f;
    float filterCutoff = apvts.getRawParameterValue("filter_cutoff")->load();
    float reverbSpace = apvts.getRawParameterValue("reverb_space")->load() * 0.01f;
    float glideMs = apvts.getRawParameterValue("glide_time")->load();
    float masterVolDb = apvts.getRawParameterValue("master_vol")->load();
    float masterVolGain = juce::Decibels::decibelsToGain(masterVolDb);

    synth.process(buffer,
                  fmAmount,
                  fmTune,
                  fmEnvDecay,
                  timeScale,
                  modAmount,
                  wavePosition,
                  subGain,
                  subTone,
                  txCrunch,
                  transientClick,
                  filterCutoff,
                  reverbSpace,
                  erosionGrit,
                  punchSlam,
                  enableSoftClip,
                  glideMs,
                  masterVolGain);

    // Push output samples to the GUI Oscilloscope FIFO
    if (onAudioBlockProcessed && buffer.getNumSamples() > 0)
    {
        onAudioBlockProcessed(buffer.getReadPointer(0), buffer.getNumSamples());
    }
}

bool ExtasisDonkerAudioProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor* ExtasisDonkerAudioProcessor::createEditor()
{
    return new ExtasisDonkerAudioProcessorEditor(*this);
}

void ExtasisDonkerAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    state.setProperty("current_program", currentProgramIndex, nullptr);
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void ExtasisDonkerAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
    {
        if (xmlState->hasTagName(apvts.state.getType()))
        {
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
            currentProgramIndex = apvts.state.getProperty("current_program", 0);
        }
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ExtasisDonkerAudioProcessor();
}
