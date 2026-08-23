#include "PluginProcessor.h"
#include "PluginEditor.h"

ExtasisDonkerAudioProcessor::ExtasisDonkerAudioProcessor()
    : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout())
{
    // Ensure user presets directory exists
    getPresetsDirectory().createDirectory();
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
        juce::NormalisableRange<float>(0.5f, 8.0f, 0.5f), 2.0f, "x"));

    // Macro 3: TX WAVE (CC 71)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("wave_position", 1), "TX Wave",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.5f), 50.0f, "%"));

    // Macro 4: FM DECAY (CC 73)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("fm_env", 1), "Donk Decay",
        juce::NormalisableRange<float>(5.0f, 500.0f, 1.0f, 0.5f), 95.0f, "ms"));

    // Macro 5: TIME SCALE (CC 12)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("pump_amount", 1), "Auto Pump",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f), 0.0f, "%"));

    // Macro 6: MOD SENS / VELOCITY (CC 11)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("mod_amount", 1), "Mod Velocity",
        juce::NormalisableRange<float>(0.0f, 200.0f, 1.0f), 100.0f, "%"));

    // Macro 7: TRANSIENT CLICK (CC 15)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("pitch_drop", 1), "Pitch Drop",
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
        juce::NormalisableRange<float>(50.0f, 20000.0f, 1.0f, 0.35f), 16000.0f, "Hz"));

    // Macro: FILTER RESONANCE (CC 75)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("filter_reso", 1), "Resonance",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f), 0.0f, "%"));

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

juce::File ExtasisDonkerAudioProcessor::getPresetsDirectory() const
{
    auto userMusic = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory);
    return userMusic.getChildFile("Audio/Presets/Extasis Records/ExtasisDonker");
}

juce::StringArray ExtasisDonkerAudioProcessor::getFactoryPresetNames()
{
    return {
        // Classic & 90s House
        "01. Lately 1987 (TX81Z)",
        "02. Basic FM House (Robin S)",
        "03. Solid Bass Legacy",
        "04. StoneBridge Organ Pluck",
        "05. Speed Garage 1998",
        "06. Todd Edwards Cut",
        // Guaracha / Aleteo / Zapateo Tribal
        "07. Guaracha Medallo Punch",
        "08. Aleteo Zapateo Knock",
        "09. Tribal House Donker",
        "10. Zapateo Laser Attack",
        "11. Cartagena Sunset Bass",
        "12. Morro Guaracha Bounce",
        // UK Bounce, Donk & Russian Hardbass
        "13. Russian Hardbass Punch",
        "14. UK Bounce Donk",
        "15. Klubbheads Hard House",
        "16. Scouse House Spring",
        "17. Donk-O-Mania Banger",
        "18. Gopnik Street Bass",
        // Modern Tech House, Bass House & Brazilian
        "19. Tech House Subby",
        "20. Bass House Growler",
        "21. Brazilian Slap FM",
        "22. Minimal Deep Donk",
        "23. Acid Donk Pluck",
        "24. Dirtybird Knock",
        // Underground & Experimental
        "25. Metallic Monster",
        "26. Deep Afterhours",
        "27. 2-Step London Garage",
        "28. 12-Bit Vintage Sampler",
        "29. Sub Atomic Laser",
        "30. Extasis Anthem Donk"
    };
}

juce::StringArray ExtasisDonkerAudioProcessor::getAllPresetNames()
{
    juce::StringArray allPresets = getFactoryPresetNames();

    auto dir = getPresetsDirectory();
    if (dir.isDirectory())
    {
        juce::Array<juce::File> userFiles;
        dir.findChildFiles(userFiles, juce::File::findFiles, false, "*.edpreset;*.xml");
        userFiles.sort();

        for (const auto& file : userFiles)
        {
            allPresets.add("[User] " + file.getFileNameWithoutExtension());
        }
    }

    return allPresets;
}

int ExtasisDonkerAudioProcessor::getNumPrograms() { return getAllPresetNames().size(); }
int ExtasisDonkerAudioProcessor::getCurrentProgram() { return currentProgramIndex; }
const juce::String ExtasisDonkerAudioProcessor::getProgramName(int index)
{
    auto names = getAllPresetNames();
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

bool ExtasisDonkerAudioProcessor::saveUserPreset(const juce::String& presetName)
{
    if (presetName.trim().isEmpty())
        return false;

    auto dir = getPresetsDirectory();
    dir.createDirectory();

    auto cleanName = presetName.trim().replaceCharacters("/\\:?*\"<>|", "_________");
    auto file = dir.getChildFile(cleanName + ".edpreset");

    auto state = apvts.copyState();
    state.setProperty("preset_name", cleanName, nullptr);

    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    if (xml != nullptr)
    {
        return xml->writeTo(file);
    }

    return false;
}

bool ExtasisDonkerAudioProcessor::deleteUserPreset(const juce::String& presetName)
{
    auto dir = getPresetsDirectory();
    auto file = dir.getChildFile(presetName + ".edpreset");
    if (file.existsAsFile())
        return file.deleteFile();
    return false;
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

    // User Preset Loading
    if (presetIndex >= 30)
    {
        int userIndex = presetIndex - 30;
        auto dir = getPresetsDirectory();
        if (dir.isDirectory())
        {
            juce::Array<juce::File> userFiles;
            dir.findChildFiles(userFiles, juce::File::findFiles, false, "*.edpreset;*.xml");
            userFiles.sort();

            if (juce::isPositiveAndBelow(userIndex, userFiles.size()))
            {
                auto file = userFiles[userIndex];
                std::unique_ptr<juce::XmlElement> xml(juce::XmlDocument::parse(file));
                if (xml != nullptr && xml->hasTagName(apvts.state.getType()))
                {
                    apvts.replaceState(juce::ValueTree::fromXml(*xml));
                    return;
                }
            }
        }
    }

    // Factory Presets (0..29)
    switch (presetIndex)
    {
        // 1. Lately 1987 (TX81Z)
        case 0:
            setP("fm_amount", 80.0f); setP("fm_tune", 2.0f); setP("wave_position", 50.0f);
            setP("fm_env", 95.0f); setP("pump_amount", 0.0f); setP("mod_amount", 110.0f);
            setP("pitch_drop", 55.0f); setP("tx_crunch", 30.0f); setP("sub_gain", -9.0f);
            setP("sub_tone", 15.0f); setP("filter_reso", 0.0f); setP("filter_cutoff", 16000.0f); setP("reverb_space", 15.0f);
            setP("erosion_grit", 15.0f); setP("punch_slam", 30.0f); setP("soft_clip", 1.0f);
            setP("glide_time", 0.0f);
            break;

        // 2. Basic FM House (Robin S)
        case 1:
            setP("fm_amount", 88.0f); setP("fm_tune", 2.0f); setP("wave_position", 10.0f);
            setP("fm_env", 120.0f); setP("pump_amount", 0.0f); setP("mod_amount", 100.0f);
            setP("pitch_drop", 65.0f); setP("tx_crunch", 15.0f); setP("sub_gain", -6.0f);
            setP("sub_tone", 20.0f); setP("filter_reso", 0.0f); setP("filter_cutoff", 18000.0f); setP("reverb_space", 25.0f);
            setP("erosion_grit", 20.0f); setP("punch_slam", 40.0f); setP("soft_clip", 1.0f);
            setP("glide_time", 15.0f);
            break;

        // 3. Solid Bass Legacy
        case 2:
            setP("fm_amount", 82.0f); setP("fm_tune", 2.0f); setP("wave_position", 45.0f);
            setP("fm_env", 105.0f); setP("pump_amount", 0.0f); setP("mod_amount", 100.0f);
            setP("pitch_drop", 50.0f); setP("tx_crunch", 20.0f); setP("sub_gain", -7.0f);
            setP("sub_tone", 15.0f); setP("filter_reso", 0.0f); setP("filter_cutoff", 15000.0f); setP("reverb_space", 10.0f);
            setP("erosion_grit", 10.0f); setP("punch_slam", 35.0f); setP("soft_clip", 1.0f);
            setP("glide_time", 0.0f);
            break;

        // 4. StoneBridge Organ Pluck
        case 3:
            setP("fm_amount", 92.0f); setP("fm_tune", 2.0f); setP("wave_position", 20.0f);
            setP("fm_env", 85.0f); setP("pump_amount", 0.0f); setP("mod_amount", 120.0f);
            setP("pitch_drop", 60.0f); setP("tx_crunch", 25.0f); setP("sub_gain", -8.0f);
            setP("sub_tone", 20.0f); setP("filter_reso", 0.0f); setP("filter_cutoff", 19000.0f); setP("reverb_space", 35.0f);
            setP("erosion_grit", 25.0f); setP("punch_slam", 45.0f); setP("soft_clip", 1.0f);
            setP("glide_time", 10.0f);
            break;

        // 5. Speed Garage 1998
        case 4:
            setP("fm_amount", 90.0f); setP("fm_tune", 4.0f); setP("wave_position", 40.0f);
            setP("fm_env", 110.0f); setP("pump_amount", 0.0f); setP("mod_amount", 110.0f);
            setP("pitch_drop", 50.0f); setP("tx_crunch", 25.0f); setP("sub_gain", -7.0f);
            setP("sub_tone", 15.0f); setP("filter_reso", 0.0f); setP("filter_cutoff", 17000.0f); setP("reverb_space", 35.0f);
            setP("erosion_grit", 25.0f); setP("punch_slam", 35.0f); setP("soft_clip", 1.0f);
            setP("glide_time", 25.0f);
            break;

        // 6. Todd Edwards Cut
        case 5:
            setP("fm_amount", 78.0f); setP("fm_tune", 1.0f); setP("wave_position", 30.0f);
            setP("fm_env", 70.0f); setP("pump_amount", -10.0f); setP("mod_amount", 90.0f);
            setP("pitch_drop", 45.0f); setP("tx_crunch", 15.0f); setP("sub_gain", -5.0f);
            setP("sub_tone", 10.0f); setP("filter_reso", 0.0f); setP("filter_cutoff", 14000.0f); setP("reverb_space", 20.0f);
            setP("erosion_grit", 15.0f); setP("punch_slam", 30.0f); setP("soft_clip", 1.0f);
            setP("glide_time", 5.0f);
            break;

        // 7. Guaracha Medallo Punch
        case 6:
            setP("fm_amount", 96.0f); setP("fm_tune", 2.0f); setP("wave_position", 65.0f);
            setP("fm_env", 65.0f); setP("pump_amount", -15.0f); setP("mod_amount", 130.0f);
            setP("pitch_drop", 85.0f); setP("tx_crunch", 35.0f); setP("sub_gain", -8.0f);
            setP("sub_tone", 25.0f); setP("filter_reso", 0.0f); setP("filter_cutoff", 18500.0f); setP("reverb_space", 15.0f);
            setP("erosion_grit", 30.0f); setP("punch_slam", 60.0f); setP("soft_clip", 1.0f);
            setP("glide_time", 0.0f);
            break;

        // 8. Aleteo Zapateo Knock
        case 7:
            setP("fm_amount", 100.0f); setP("fm_tune", 3.0f); setP("wave_position", 55.0f);
            setP("fm_env", 50.0f); setP("pump_amount", -25.0f); setP("mod_amount", 140.0f);
            setP("pitch_drop", 95.0f); setP("tx_crunch", 40.0f); setP("sub_gain", -10.0f);
            setP("sub_tone", 30.0f); setP("filter_reso", 0.0f); setP("filter_cutoff", 20000.0f); setP("reverb_space", 20.0f);
            setP("erosion_grit", 40.0f); setP("punch_slam", 75.0f); setP("soft_clip", 1.0f);
            setP("glide_time", 0.0f);
            break;

        // 9. Tribal House Donker
        case 8:
            setP("fm_amount", 90.0f); setP("fm_tune", 2.0f); setP("wave_position", 50.0f);
            setP("fm_env", 80.0f); setP("pump_amount", -10.0f); setP("mod_amount", 115.0f);
            setP("pitch_drop", 75.0f); setP("tx_crunch", 30.0f); setP("sub_gain", -6.0f);
            setP("sub_tone", 20.0f); setP("filter_reso", 0.0f); setP("filter_cutoff", 17000.0f); setP("reverb_space", 25.0f);
            setP("erosion_grit", 25.0f); setP("punch_slam", 50.0f); setP("soft_clip", 1.0f);
            setP("glide_time", 10.0f);
            break;

        // 10. Zapateo Laser Attack
        case 9:
            setP("fm_amount", 98.0f); setP("fm_tune", 4.0f); setP("wave_position", 70.0f);
            setP("fm_env", 45.0f); setP("pump_amount", -30.0f); setP("mod_amount", 150.0f);
            setP("pitch_drop", 100.0f); setP("tx_crunch", 45.0f); setP("sub_gain", -11.0f);
            setP("sub_tone", 35.0f); setP("filter_reso", 0.0f); setP("filter_cutoff", 20000.0f); setP("reverb_space", 15.0f);
            setP("erosion_grit", 50.0f); setP("punch_slam", 80.0f); setP("soft_clip", 1.0f);
            setP("glide_time", 0.0f);
            break;

        // 11. Cartagena Sunset Bass
        case 10:
            setP("fm_amount", 75.0f); setP("fm_tune", 2.0f); setP("wave_position", 35.0f);
            setP("fm_env", 110.0f); setP("pump_amount", 0.0f); setP("mod_amount", 95.0f);
            setP("pitch_drop", 50.0f); setP("tx_crunch", 15.0f); setP("sub_gain", -4.0f);
            setP("sub_tone", 15.0f); setP("filter_reso", 0.0f); setP("filter_cutoff", 15000.0f); setP("reverb_space", 30.0f);
            setP("erosion_grit", 10.0f); setP("punch_slam", 35.0f); setP("soft_clip", 1.0f);
            setP("glide_time", 15.0f);
            break;

        // 12. Morro Guaracha Bounce
        case 11:
            setP("fm_amount", 92.0f); setP("fm_tune", 2.0f); setP("wave_position", 60.0f);
            setP("fm_env", 75.0f); setP("pump_amount", -5.0f); setP("mod_amount", 125.0f);
            setP("pitch_drop", 70.0f); setP("tx_crunch", 25.0f); setP("sub_gain", -7.0f);
            setP("sub_tone", 20.0f); setP("filter_reso", 0.0f); setP("filter_cutoff", 18000.0f); setP("reverb_space", 20.0f);
            setP("erosion_grit", 20.0f); setP("punch_slam", 55.0f); setP("soft_clip", 1.0f);
            setP("glide_time", 20.0f);
            break;

        // 13. Russian Hardbass Punch
        case 12:
            setP("fm_amount", 100.0f); setP("fm_tune", 2.0f); setP("wave_position", 80.0f);
            setP("fm_env", 75.0f); setP("pump_amount", -15.0f); setP("mod_amount", 140.0f);
            setP("pitch_drop", 95.0f); setP("tx_crunch", 50.0f); setP("sub_gain", -12.0f);
            setP("sub_tone", 35.0f); setP("filter_reso", 0.0f); setP("filter_cutoff", 20000.0f); setP("reverb_space", 10.0f);
            setP("erosion_grit", 45.0f); setP("punch_slam", 70.0f); setP("soft_clip", 1.0f);
            setP("glide_time", 0.0f);
            break;

        // 14. UK Bounce Donk
        case 13:
            setP("fm_amount", 95.0f); setP("fm_tune", 3.0f); setP("wave_position", 60.0f);
            setP("fm_env", 85.0f); setP("pump_amount", 0.0f); setP("mod_amount", 120.0f);
            setP("pitch_drop", 80.0f); setP("tx_crunch", 35.0f); setP("sub_gain", -8.0f);
            setP("sub_tone", 25.0f); setP("filter_reso", 0.0f); setP("filter_cutoff", 19000.0f); setP("reverb_space", 30.0f);
            setP("erosion_grit", 30.0f); setP("punch_slam", 50.0f); setP("soft_clip", 1.0f);
            setP("glide_time", 20.0f);
            break;

        // 15. Klubbheads Hard House
        case 14:
            setP("fm_amount", 94.0f); setP("fm_tune", 2.0f); setP("wave_position", 50.0f);
            setP("fm_env", 65.0f); setP("pump_amount", -10.0f); setP("mod_amount", 130.0f);
            setP("pitch_drop", 75.0f); setP("tx_crunch", 35.0f); setP("sub_gain", -9.0f);
            setP("sub_tone", 20.0f); setP("filter_reso", 0.0f); setP("filter_cutoff", 19500.0f); setP("reverb_space", 25.0f);
            setP("erosion_grit", 35.0f); setP("punch_slam", 60.0f); setP("soft_clip", 1.0f);
            setP("glide_time", 0.0f);
            break;

        // 16. Scouse House Spring
        case 15:
            setP("fm_amount", 88.0f); setP("fm_tune", 2.5f); setP("wave_position", 40.0f);
            setP("fm_env", 90.0f); setP("pump_amount", 0.0f); setP("mod_amount", 110.0f);
            setP("pitch_drop", 70.0f); setP("tx_crunch", 25.0f); setP("sub_gain", -8.0f);
            setP("sub_tone", 15.0f); setP("filter_reso", 0.0f); setP("filter_cutoff", 17500.0f); setP("reverb_space", 35.0f);
            setP("erosion_grit", 20.0f); setP("punch_slam", 45.0f); setP("soft_clip", 1.0f);
            setP("glide_time", 25.0f);
            break;

        // 17. Donk-O-Mania Banger
        case 16:
            setP("fm_amount", 100.0f); setP("fm_tune", 3.0f); setP("wave_position", 75.0f);
            setP("fm_env", 70.0f); setP("pump_amount", -15.0f); setP("mod_amount", 150.0f);
            setP("pitch_drop", 90.0f); setP("tx_crunch", 55.0f); setP("sub_gain", -10.0f);
            setP("sub_tone", 30.0f); setP("filter_reso", 0.0f); setP("filter_cutoff", 20000.0f); setP("reverb_space", 25.0f);
            setP("erosion_grit", 50.0f); setP("punch_slam", 75.0f); setP("soft_clip", 1.0f);
            setP("glide_time", 15.0f);
            break;

        // 18. Gopnik Street Bass
        case 17:
            setP("fm_amount", 98.0f); setP("fm_tune", 4.0f); setP("wave_position", 85.0f);
            setP("fm_env", 60.0f); setP("pump_amount", -20.0f); setP("mod_amount", 145.0f);
            setP("pitch_drop", 85.0f); setP("tx_crunch", 60.0f); setP("sub_gain", -11.0f);
            setP("sub_tone", 35.0f); setP("filter_reso", 0.0f); setP("filter_cutoff", 20000.0f); setP("reverb_space", 15.0f);
            setP("erosion_grit", 55.0f); setP("punch_slam", 70.0f); setP("soft_clip", 1.0f);
            setP("glide_time", 0.0f);
            break;

        // 19. Tech House Subby
        case 18:
            setP("fm_amount", 65.0f); setP("fm_tune", 1.0f); setP("wave_position", 25.0f);
            setP("fm_env", 130.0f); setP("pump_amount", 5.0f); setP("mod_amount", 80.0f);
            setP("pitch_drop", 40.0f); setP("tx_crunch", 10.0f); setP("sub_gain", -3.0f);
            setP("sub_tone", 10.0f); setP("filter_reso", 0.0f); setP("filter_cutoff", 12000.0f); setP("reverb_space", 15.0f);
            setP("erosion_grit", 10.0f); setP("punch_slam", 25.0f); setP("soft_clip", 1.0f);
            setP("glide_time", 10.0f);
            break;

        // 20. Bass House Growler
        case 19:
            setP("fm_amount", 95.0f); setP("fm_tune", 1.5f); setP("wave_position", 80.0f);
            setP("fm_env", 140.0f); setP("pump_amount", 0.0f); setP("mod_amount", 125.0f);
            setP("pitch_drop", 65.0f); setP("tx_crunch", 45.0f); setP("sub_gain", -6.0f);
            setP("sub_tone", 30.0f); setP("filter_reso", 0.0f); setP("filter_cutoff", 16000.0f); setP("reverb_space", 20.0f);
            setP("erosion_grit", 40.0f); setP("punch_slam", 65.0f); setP("soft_clip", 1.0f);
            setP("glide_time", 30.0f);
            break;

        // 21. Brazilian Slap FM
        case 20:
            setP("fm_amount", 85.0f); setP("fm_tune", 2.0f); setP("wave_position", 40.0f);
            setP("fm_env", 80.0f); setP("pump_amount", -10.0f); setP("mod_amount", 110.0f);
            setP("pitch_drop", 75.0f); setP("tx_crunch", 20.0f); setP("sub_gain", -5.0f);
            setP("sub_tone", 15.0f); setP("filter_reso", 0.0f); setP("filter_cutoff", 16500.0f); setP("reverb_space", 15.0f);
            setP("erosion_grit", 20.0f); setP("punch_slam", 50.0f); setP("soft_clip", 1.0f);
            setP("glide_time", 15.0f);
            break;

        // 22. Minimal Deep Donk
        case 21:
            setP("fm_amount", 45.0f); setP("fm_tune", 1.0f); setP("wave_position", 15.0f);
            setP("fm_env", 150.0f); setP("pump_amount", 10.0f); setP("mod_amount", 60.0f);
            setP("pitch_drop", 25.0f); setP("tx_crunch", 5.0f); setP("sub_gain", -2.0f);
            setP("sub_tone", 5.0f); setP("filter_reso", 0.0f); setP("filter_cutoff", 9000.0f); setP("reverb_space", 10.0f);
            setP("erosion_grit", 0.0f); setP("punch_slam", 20.0f); setP("soft_clip", 1.0f);
            setP("glide_time", 20.0f);
            break;

        // 23. Acid Donk Pluck
        case 22:
            setP("fm_amount", 88.0f); setP("fm_tune", 3.0f); setP("wave_position", 70.0f);
            setP("fm_env", 90.0f); setP("pump_amount", 0.0f); setP("mod_amount", 130.0f);
            setP("pitch_drop", 70.0f); setP("tx_crunch", 40.0f); setP("sub_gain", -8.0f);
            setP("sub_tone", 25.0f); setP("filter_reso", 0.0f); setP("filter_cutoff", 13000.0f); setP("reverb_space", 25.0f);
            setP("erosion_grit", 30.0f); setP("punch_slam", 55.0f); setP("soft_clip", 1.0f);
            setP("glide_time", 25.0f);
            break;

        // 24. Dirtybird Knock
        case 23:
            setP("fm_amount", 80.0f); setP("fm_tune", 2.0f); setP("wave_position", 30.0f);
            setP("fm_env", 95.0f); setP("pump_amount", 0.0f); setP("mod_amount", 105.0f);
            setP("pitch_drop", 65.0f); setP("tx_crunch", 20.0f); setP("sub_gain", -4.0f);
            setP("sub_tone", 15.0f); setP("filter_reso", 0.0f); setP("filter_cutoff", 15500.0f); setP("reverb_space", 20.0f);
            setP("erosion_grit", 15.0f); setP("punch_slam", 45.0f); setP("soft_clip", 1.0f);
            setP("glide_time", 15.0f);
            break;

        // 25. Metallic Monster
        case 24:
            setP("fm_amount", 100.0f); setP("fm_tune", 5.0f); setP("wave_position", 90.0f);
            setP("fm_env", 65.0f); setP("pump_amount", -20.0f); setP("mod_amount", 150.0f);
            setP("pitch_drop", 90.0f); setP("tx_crunch", 70.0f); setP("sub_gain", -10.0f);
            setP("sub_tone", 40.0f); setP("filter_reso", 0.0f); setP("filter_cutoff", 20000.0f); setP("reverb_space", 40.0f);
            setP("erosion_grit", 60.0f); setP("punch_slam", 65.0f); setP("soft_clip", 1.0f);
            setP("glide_time", 0.0f);
            break;

        // 26. Deep Afterhours
        case 25:
            setP("fm_amount", 50.0f); setP("fm_tune", 1.0f); setP("wave_position", 0.0f);
            setP("fm_env", 160.0f); setP("pump_amount", 10.0f); setP("mod_amount", 70.0f);
            setP("pitch_drop", 30.0f); setP("tx_crunch", 5.0f); setP("sub_gain", -4.0f);
            setP("sub_tone", 5.0f); setP("filter_reso", 0.0f); setP("filter_cutoff", 8000.0f); setP("reverb_space", 20.0f);
            setP("erosion_grit", 0.0f); setP("punch_slam", 15.0f); setP("soft_clip", 1.0f);
            setP("glide_time", 35.0f);
            break;

        // 27. 2-Step London Garage
        case 26:
            setP("fm_amount", 85.0f); setP("fm_tune", 2.0f); setP("wave_position", 35.0f);
            setP("fm_env", 100.0f); setP("pump_amount", 0.0f); setP("mod_amount", 105.0f);
            setP("pitch_drop", 55.0f); setP("tx_crunch", 20.0f); setP("sub_gain", -6.0f);
            setP("sub_tone", 15.0f); setP("filter_reso", 0.0f); setP("filter_cutoff", 16000.0f); setP("reverb_space", 30.0f);
            setP("erosion_grit", 20.0f); setP("punch_slam", 40.0f); setP("soft_clip", 1.0f);
            setP("glide_time", 20.0f);
            break;

        // 28. 12-Bit Vintage Sampler
        case 27:
            setP("fm_amount", 80.0f); setP("fm_tune", 2.0f); setP("wave_position", 50.0f);
            setP("fm_env", 90.0f); setP("pump_amount", 0.0f); setP("mod_amount", 100.0f);
            setP("pitch_drop", 60.0f); setP("tx_crunch", 30.0f); setP("sub_gain", -8.0f);
            setP("sub_tone", 20.0f); setP("filter_reso", 0.0f); setP("filter_cutoff", 14000.0f); setP("reverb_space", 15.0f);
            setP("erosion_grit", 70.0f); setP("punch_slam", 50.0f); setP("soft_clip", 1.0f);
            setP("glide_time", 0.0f);
            break;

        // 29. Sub Atomic Laser
        case 28:
            setP("fm_amount", 95.0f); setP("fm_tune", 3.0f); setP("wave_position", 60.0f);
            setP("fm_env", 40.0f); setP("pump_amount", -35.0f); setP("mod_amount", 160.0f);
            setP("pitch_drop", 100.0f); setP("tx_crunch", 30.0f); setP("sub_gain", -5.0f);
            setP("sub_tone", 25.0f); setP("filter_reso", 0.0f); setP("filter_cutoff", 20000.0f); setP("reverb_space", 20.0f);
            setP("erosion_grit", 30.0f); setP("punch_slam", 85.0f); setP("soft_clip", 1.0f);
            setP("glide_time", 0.0f);
            break;

        // 30. Extasis Anthem Donk
        case 29:
            setP("fm_amount", 96.0f); setP("fm_tune", 2.0f); setP("wave_position", 55.0f);
            setP("fm_env", 80.0f); setP("pump_amount", -5.0f); setP("mod_amount", 135.0f);
            setP("pitch_drop", 85.0f); setP("tx_crunch", 40.0f); setP("sub_gain", -7.0f);
            setP("sub_tone", 25.0f); setP("filter_reso", 0.0f); setP("filter_cutoff", 19500.0f); setP("reverb_space", 28.0f);
            setP("erosion_grit", 35.0f); setP("punch_slam", 65.0f); setP("soft_clip", 1.0f);
            setP("glide_time", 15.0f);
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
        case 12: mapParam("pump_amount"); break;
        case 11: mapParam("mod_amount"); break;
        case 15: mapParam("pitch_drop"); break;
        case 16: mapParam("tx_crunch"); break;
        case 17: mapParam("erosion_grit"); break;
        case 18: mapParam("punch_slam"); break;
        case 19: mapParam("sub_gain"); break;
        case 20: mapParam("sub_tone"); break;
        case 74: mapParam("filter_cutoff"); break;
        case 75: mapParam("filter_reso"); break;
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

    double bpm = 120.0;
    double ppqPosition = 0.0;
    if (auto* playHead = getPlayHead()) {
        if (auto posInfo = playHead->getPosition()) {
            if (posInfo->getBpm().hasValue()) bpm = *posInfo->getBpm();
            if (posInfo->getPpqPosition().hasValue()) ppqPosition = *posInfo->getPpqPosition();
        }
    }

    float fmAmount = apvts.getRawParameterValue("fm_amount")->load() * 0.01f;
    float fmTune = apvts.getRawParameterValue("fm_tune")->load();
    float wavePosition = apvts.getRawParameterValue("wave_position")->load() * 0.01f;
    float fmEnvDecay = apvts.getRawParameterValue("fm_env")->load();
    float pumpAmount = apvts.getRawParameterValue("pump_amount")->load() * 0.01f;
    float modAmount = apvts.getRawParameterValue("mod_amount")->load() * 0.01f;
    float pitchDrop = apvts.getRawParameterValue("pitch_drop")->load() * 0.01f;
    float txCrunch = apvts.getRawParameterValue("tx_crunch")->load() * 0.01f;
    float erosionGrit = apvts.getRawParameterValue("erosion_grit")->load() * 0.01f;
    float punchSlam = apvts.getRawParameterValue("punch_slam")->load() * 0.01f;
    bool enableSoftClip = apvts.getRawParameterValue("soft_clip")->load() > 0.5f;
    float subGain = apvts.getRawParameterValue("sub_gain")->load();
    float subTone = apvts.getRawParameterValue("sub_tone")->load() * 0.01f;
    float filterCutoff = apvts.getRawParameterValue("filter_cutoff")->load();
    float filterReso = apvts.getRawParameterValue("filter_reso")->load() * 0.01f;
    float reverbSpace = apvts.getRawParameterValue("reverb_space")->load() * 0.01f;
    float glideMs = apvts.getRawParameterValue("glide_time")->load();
    float masterVolDb = apvts.getRawParameterValue("master_vol")->load();
    float masterVolGain = juce::Decibels::decibelsToGain(masterVolDb);

    synth.process(buffer,
                  fmAmount,
                  fmTune,
                  fmEnvDecay,
                  pumpAmount,
                  bpm,
                  ppqPosition,
                  modAmount,
                  wavePosition,
                  subGain,
                  subTone,
                  txCrunch,
                  pitchDrop,
                  filterCutoff,
                  filterReso,
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
