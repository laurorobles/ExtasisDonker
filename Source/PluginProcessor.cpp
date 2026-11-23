#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cstring>

namespace
{
struct FactoryPreset
{
    const char* name;
    float fmAmount;
    float fmTune;
    float wavePosition;
    float fmEnv;
    float pumpAmount;
    float modAmount;
    float pitchDrop;
    float txCrunch;
    float subGain;
    float subTone;
    float filterReso;
    float filterCutoff;
    float reverbSpace;
    float erosionGrit;
    float punchSlam;
    float softClip;
    float glideTime;
};

// 40 curated, distinct factory presets spanning UK Bounce, Guaracha, LatelyBass, Hardbass, Garage & Acid
static constexpr FactoryPreset kFactoryPresets[] = {
    // 1–10: Classic & Club Donk
    { "1. Blackout",       90.0f, 2.0f,  50.0f, 110.0f,  85.0f, 100.0f,  80.0f,  15.0f,  -6.0f, 15.0f,  0.0f, 18000.0f, 15.0f, 10.0f,  50.0f, 1.0f,  0.0f },
    { "2. Bamboo",         85.0f, 2.0f,   0.0f,  85.0f,   0.0f, 100.0f,  20.0f,  40.0f,  -9.0f, 10.0f,  0.0f, 16000.0f,  5.0f, 30.0f,  40.0f, 1.0f,  0.0f },
    { "3. Wigan",          95.0f, 2.0f,  25.0f,  75.0f,  50.0f, 120.0f,  60.0f,  25.0f,  -5.0f, 25.0f,  0.0f, 20000.0f, 20.0f, 50.0f,  60.0f, 1.0f,  0.0f },
    { "4. Kolbaser",      100.0f, 1.0f, 100.0f, 120.0f,   0.0f, 100.0f,  90.0f, 100.0f,   0.0f, 50.0f, 20.0f, 12000.0f,  0.0f, 10.0f, 100.0f, 1.0f,  0.0f },
    { "5. Medallo",        88.0f, 3.0f,  75.0f, 100.0f, 100.0f,  80.0f,  40.0f,  10.0f,  -6.0f, 10.0f, 30.0f, 15000.0f, 40.0f,  0.0f,  30.0f, 1.0f,  0.0f },
    { "6. Robin Organ",    60.0f, 2.0f,   0.0f, 250.0f,   0.0f, 150.0f,   0.0f,   0.0f, -12.0f,  0.0f,  0.0f, 10000.0f, 30.0f,  0.0f,  10.0f, 0.0f,  0.0f },
    { "7. Lately 1987",    75.0f, 2.0f,  50.0f, 180.0f,   0.0f, 100.0f,   0.0f,   5.0f, -10.0f,  0.0f,  0.0f, 20000.0f,  0.0f,  0.0f,   0.0f, 0.0f,  0.0f },
    { "8. 303 Squelch",    50.0f, 4.0f,   0.0f, 120.0f,  40.0f, 200.0f, 100.0f,  30.0f,  -6.0f, 20.0f, 85.0f,  3500.0f, 10.0f, 40.0f,  50.0f, 1.0f, 25.0f },
    { "9. Speed Rip",      92.0f, 3.0f,  50.0f,  65.0f,  35.0f, 130.0f,  45.0f,  20.0f,  -7.0f, 18.0f, 15.0f, 17000.0f, 22.0f, 25.0f,  55.0f, 1.0f,  0.0f },
    { "10. Sub Punch",     65.0f, 1.0f,  20.0f,  90.0f,  60.0f,  90.0f,  75.0f,  10.0f,  -1.0f, 35.0f,  0.0f,  6500.0f,  0.0f,  5.0f,  70.0f, 1.0f,  0.0f },

    // 11–20: UK Bounce, Scouse & Hardbass
    { "11. Bounce 90",     86.0f, 2.0f,  45.0f,  95.0f,  70.0f, 110.0f,  65.0f,  20.0f,  -6.0f, 20.0f, 10.0f, 16500.0f, 18.0f, 15.0f,  45.0f, 1.0f,  0.0f },
    { "12. Fumaratto",     84.0f, 3.0f,  80.0f,  88.0f,  95.0f,  95.0f,  55.0f,  12.0f,  -5.0f, 15.0f, 25.0f, 14500.0f, 35.0f, 12.0f,  40.0f, 1.0f, 10.0f },
    { "13. Hardbass",      98.0f, 1.5f,  90.0f,  70.0f,   0.0f, 100.0f,  85.0f,  85.0f,  -2.0f, 60.0f, 18.0f, 11000.0f,  5.0f, 45.0f,  90.0f, 1.0f,  0.0f },
    { "14. Scouse House",  88.0f, 2.0f,  35.0f, 105.0f,  65.0f, 125.0f,  50.0f,  30.0f,  -7.0f, 12.0f,  5.0f, 18500.0f, 25.0f, 22.0f,  50.0f, 1.0f,  0.0f },
    { "15. Deep Tube",     55.0f, 1.0f,  15.0f, 160.0f,  25.0f,  80.0f,  15.0f,  45.0f,  -3.0f, 45.0f, 12.0f,  4200.0f,  8.0f,  0.0f,  35.0f, 1.0f, 15.0f },
    { "16. Zapateo",       94.0f, 2.5f,  65.0f,  80.0f, 100.0f, 100.0f,  70.0f,  25.0f,  -4.0f, 22.0f, 20.0f, 15500.0f, 32.0f, 18.0f,  65.0f, 1.0f,  0.0f },
    { "17. 4-Op Classic",  72.0f, 2.0f,  50.0f, 145.0f,   0.0f, 115.0f,  10.0f,  12.0f, -11.0f,  5.0f,  0.0f, 14000.0f, 12.0f,  5.0f,  15.0f, 0.0f,  0.0f },
    { "18. Square Sub",    45.0f, 1.0f, 100.0f, 210.0f,  30.0f,  70.0f,  25.0f,  60.0f,  -2.0f, 75.0f, 10.0f,  2800.0f,  0.0f, 15.0f,  55.0f, 1.0f, 20.0f },
    { "19. Dirty Tape",    80.0f, 2.0f,  40.0f, 130.0f,  20.0f, 100.0f,  35.0f,  75.0f,  -6.0f, 40.0f, 15.0f,  9500.0f, 15.0f, 65.0f,  70.0f, 1.0f,  0.0f },
    { "20. Hyper Donk",   100.0f, 3.0f,  60.0f,  60.0f,  80.0f, 140.0f,  95.0f,  50.0f,  -5.0f, 30.0f, 25.0f, 20000.0f, 28.0f, 55.0f,  85.0f, 1.0f,  0.0f },

    // 21–30: Vintage FM, Garage & Latin Club
    { "21. Galop",         78.0f, 2.0f,  30.0f,  55.0f,  55.0f, 110.0f,  40.0f,  18.0f,  -8.0f, 15.0f,  8.0f, 14000.0f, 14.0f, 12.0f,  45.0f, 1.0f, 35.0f },
    { "22. Tech Slap",     82.0f, 2.0f,  55.0f,  45.0f,  45.0f, 135.0f,  65.0f,  22.0f,  -7.0f, 10.0f,  0.0f, 17500.0f, 10.0f, 28.0f,  60.0f, 1.0f,  0.0f },
    { "23. Sine Mod",      68.0f, 2.0f,   0.0f, 150.0f,   0.0f, 100.0f,  15.0f,   0.0f,  -8.0f,  5.0f,  0.0f, 12500.0f, 18.0f,  0.0f,  20.0f, 0.0f, 12.0f },
    { "24. Bell Top",      88.0f, 5.0f,  25.0f, 190.0f,  30.0f, 150.0f,  10.0f,  10.0f,  -9.0f,  8.0f, 12.0f, 19000.0f, 45.0f, 15.0f,  25.0f, 0.0f,  0.0f },
    { "25. Half Sine",     76.0f, 2.0f,  50.0f, 125.0f,   0.0f, 105.0f,  25.0f,  15.0f,  -8.0f, 12.0f,  5.0f, 16000.0f, 12.0f,  8.0f,  30.0f, 1.0f,  0.0f },
    { "26. Rectified",     85.0f, 3.0f, 100.0f,  95.0f,  40.0f, 115.0f,  50.0f,  35.0f,  -6.0f, 20.0f, 18.0f, 15000.0f, 20.0f, 32.0f,  50.0f, 1.0f,  0.0f },
    { "27. 12-Bit",        82.0f, 2.0f,  60.0f, 100.0f,  15.0f, 100.0f,  35.0f,  65.0f,  -7.0f, 25.0f, 10.0f, 13000.0f, 10.0f, 85.0f,  55.0f, 1.0f,  0.0f },
    { "28. Overdrive",     95.0f, 1.5f,  75.0f, 115.0f,  50.0f,  95.0f,  60.0f,  95.0f,  -4.0f, 55.0f, 22.0f, 10500.0f, 12.0f, 40.0f,  85.0f, 1.0f,  0.0f },
    { "29. Laser Drop",    90.0f, 2.0f,  40.0f,  85.0f,  65.0f, 120.0f, 100.0f,  20.0f,  -5.0f, 18.0f, 35.0f, 17000.0f, 25.0f, 20.0f,  65.0f, 1.0f, 15.0f },
    { "30. Anthem",        88.0f, 2.0f,  50.0f, 135.0f,  75.0f, 110.0f,  50.0f,  25.0f,  -5.0f, 20.0f, 10.0f, 18000.0f, 35.0f, 15.0f,  55.0f, 1.0f,  0.0f },

    // 31–40: Modern Underground, Acid & Bassline
    { "31. Laser Punch",   96.0f, 2.0f,  55.0f,  50.0f,  70.0f, 130.0f, 100.0f,  30.0f,  -4.0f, 25.0f, 20.0f, 19000.0f, 18.0f, 30.0f,  75.0f, 1.0f,  0.0f },
    { "32. Club Destroyer",100.0f, 2.0f, 65.0f,  95.0f,  85.0f, 100.0f,  75.0f,  70.0f,  -1.0f, 45.0f, 15.0f, 16500.0f, 22.0f, 45.0f,  95.0f, 1.0f,  0.0f },
    { "33. Acid Rub",      62.0f, 3.0f,  20.0f, 140.0f,  50.0f, 180.0f,  30.0f,  25.0f,  -6.0f, 15.0f, 92.0f,  2200.0f, 15.0f, 20.0f,  45.0f, 1.0f, 45.0f },
    { "34. UK Garage",     74.0f, 2.0f,  45.0f, 165.0f,   0.0f, 140.0f,  15.0f,  10.0f,  -4.0f, 20.0f, 18.0f,  9000.0f, 28.0f,  5.0f,  30.0f, 1.0f, 30.0f },
    { "35. Nightcore",     92.0f, 4.0f,  60.0f,  70.0f,  80.0f, 120.0f,  80.0f,  20.0f,  -7.0f, 12.0f, 15.0f, 20000.0f, 42.0f, 25.0f,  60.0f, 1.0f,  0.0f },
    { "36. Bassline",      86.0f, 1.5f,  85.0f, 110.0f,  40.0f, 150.0f,  45.0f,  55.0f,  -3.0f, 35.0f, 45.0f,  7500.0f, 20.0f, 35.0f,  70.0f, 1.0f, 55.0f },
    { "37. Dirty Sine",    48.0f, 1.0f,   5.0f, 220.0f,  30.0f,  85.0f,  20.0f,  80.0f,  -2.0f, 65.0f,  8.0f,  5000.0f,  5.0f, 25.0f,  50.0f, 1.0f, 18.0f },
    { "38. Polarity",      90.0f, 6.0f,  50.0f,  65.0f,  60.0f, 125.0f,  55.0f,  18.0f,  -6.0f, 15.0f, 22.0f, 18500.0f, 55.0f, 20.0f,  45.0f, 1.0f,  0.0f },
    { "39. Resonance",     70.0f, 2.5f,  35.0f, 155.0f,  45.0f, 170.0f,  35.0f,  20.0f,  -6.0f, 18.0f, 80.0f,  4800.0f, 30.0f, 15.0f,  40.0f, 1.0f, 22.0f },
    { "40. Masterpiece",   88.0f, 2.0f,  50.0f, 105.0f,  65.0f, 115.0f,  60.0f,  28.0f,  -4.0f, 24.0f, 12.0f, 17500.0f, 26.0f, 22.0f,  62.0f, 1.0f,  0.0f }
};

static constexpr int kNumFactoryPresets = (int)(sizeof(kFactoryPresets) / sizeof(kFactoryPresets[0]));
} // namespace

ExtasisDonkerAudioProcessor::ExtasisDonkerAudioProcessor()
    : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout())
{
    // Cache raw parameter pointers once to avoid string lookups on the audio thread
    pFmAmount     = apvts.getRawParameterValue("fm_amount");
    pFmTune       = apvts.getRawParameterValue("fm_tune");
    pWavePosition = apvts.getRawParameterValue("wave_position");
    pFmEnv        = apvts.getRawParameterValue("fm_env");
    pPumpAmount   = apvts.getRawParameterValue("pump_amount");
    pModAmount    = apvts.getRawParameterValue("mod_amount");
    pPitchDrop    = apvts.getRawParameterValue("pitch_drop");
    pTxCrunch     = apvts.getRawParameterValue("tx_crunch");
    pErosionGrit  = apvts.getRawParameterValue("erosion_grit");
    pPunchSlam    = apvts.getRawParameterValue("punch_slam");
    pSoftClip     = apvts.getRawParameterValue("soft_clip");
    pSubGain      = apvts.getRawParameterValue("sub_gain");
    pSubTone      = apvts.getRawParameterValue("sub_tone");
    pFilterCutoff = apvts.getRawParameterValue("filter_cutoff");
    pFilterReso   = apvts.getRawParameterValue("filter_reso");
    pReverbSpace  = apvts.getRawParameterValue("reverb_space");
    pGlideTime    = apvts.getRawParameterValue("glide_time");
    pMasterVol    = apvts.getRawParameterValue("master_vol");

    // Ensure user presets directory exists
    getPresetsDirectory().createDirectory();
}

ExtasisDonkerAudioProcessor::~ExtasisDonkerAudioProcessor() = default;

juce::AudioProcessorValueTreeState::ParameterLayout ExtasisDonkerAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // Macro 1: DONK PUNCH (CC 13)
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

    // Macro 5: AUTO PUMP (CC 12)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("pump_amount", 1), "Auto Pump",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f), 0.0f, "%"));

    // Macro 6: MOD SENS / VELOCITY (CC 11)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("mod_amount", 1), "Mod Velocity",
        juce::NormalisableRange<float>(0.0f, 200.0f, 1.0f), 100.0f, "%"));

    // Macro 7: PITCH DROP (CC 15)
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
double ExtasisDonkerAudioProcessor::getTailLengthSeconds() const { return 0.25; }

juce::File ExtasisDonkerAudioProcessor::getPresetsDirectory() const
{
    auto userMusic = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory);
    return userMusic.getChildFile("Audio/Presets/Extasis Records/ExtasisDonker");
}

juce::StringArray ExtasisDonkerAudioProcessor::getFactoryPresetNames()
{
    juce::StringArray names;
    names.ensureStorageAllocated(kNumFactoryPresets);
    for (int i = 0; i < kNumFactoryPresets; ++i)
        names.add(kFactoryPresets[i].name);
    return names;
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
            allPresets.add("[User] " + file.getFileNameWithoutExtension());
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
        return xml->writeTo(file);

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
    const juce::SpinLock::ScopedLockType lock(auditionMidiLock);
    pendingAuditionMidi.addEvent(juce::MidiMessage::noteOn(1, noteNumber, velocity), 0);
}

void ExtasisDonkerAudioProcessor::releaseAuditionNote(int noteNumber)
{
    const juce::SpinLock::ScopedLockType lock(auditionMidiLock);
    pendingAuditionMidi.addEvent(juce::MidiMessage::noteOff(1, noteNumber), 0);
}

void ExtasisDonkerAudioProcessor::pushScopeSamples(const float* samples, int numSamples)
{
    if (samples == nullptr || numSamples <= 0)
        return;

    int toWrite = juce::jmin(numSamples, scopeFifoCapacity);
    int start1, size1, start2, size2;
    scopeFifo.prepareToWrite(toWrite, start1, size1, start2, size2);

    if (size1 > 0)
        std::memcpy(scopeFifoBuffer.data() + start1, samples, (size_t)size1 * sizeof(float));
    if (size2 > 0)
        std::memcpy(scopeFifoBuffer.data() + start2, samples + size1, (size_t)size2 * sizeof(float));

    scopeFifo.finishedWrite(size1 + size2);
}

int ExtasisDonkerAudioProcessor::popScopeSamples(float* dest, int maxSamples)
{
    if (dest == nullptr || maxSamples <= 0)
        return 0;

    int available = scopeFifo.getNumReady();
    int toRead = juce::jmin(available, maxSamples);
    if (toRead <= 0)
        return 0;

    int start1, size1, start2, size2;
    scopeFifo.prepareToRead(toRead, start1, size1, start2, size2);

    if (size1 > 0)
        std::memcpy(dest, scopeFifoBuffer.data() + start1, (size_t)size1 * sizeof(float));
    if (size2 > 0)
        std::memcpy(dest + size1, scopeFifoBuffer.data() + start2, (size_t)size2 * sizeof(float));

    scopeFifo.finishedRead(size1 + size2);
    return size1 + size2;
}

float ExtasisDonkerAudioProcessor::consumeOutputPeak()
{
    return outputPeak.exchange(0.0f, std::memory_order_relaxed);
}

bool ExtasisDonkerAudioProcessor::consumeMidiActivity()
{
    return midiActivity.exchange(false, std::memory_order_relaxed);
}

int ExtasisDonkerAudioProcessor::consumeLastMidiCC()
{
    return lastMidiCC.exchange(-1, std::memory_order_relaxed);
}

void ExtasisDonkerAudioProcessor::loadPreset(int presetIndex)
{
    auto setP = [this](const juce::String& id, float val) {
        if (auto* param = apvts.getParameter(id))
            param->setValueNotifyingHost(param->convertTo0to1(val));
    };

    // User Preset Loading
    if (presetIndex >= kNumFactoryPresets)
    {
        int userIndex = presetIndex - kNumFactoryPresets;
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
        return;
    }

    if (juce::isPositiveAndBelow(presetIndex, kNumFactoryPresets))
    {
        const auto& p = kFactoryPresets[presetIndex];
        setP("fm_amount",     p.fmAmount);
        setP("fm_tune",       p.fmTune);
        setP("wave_position", p.wavePosition);
        setP("fm_env",        p.fmEnv);
        setP("pump_amount",   p.pumpAmount);
        setP("mod_amount",    p.modAmount);
        setP("pitch_drop",    p.pitchDrop);
        setP("tx_crunch",     p.txCrunch);
        setP("sub_gain",      p.subGain);
        setP("sub_tone",      p.subTone);
        setP("filter_reso",   p.filterReso);
        setP("filter_cutoff", p.filterCutoff);
        setP("reverb_space",  p.reverbSpace);
        setP("erosion_grit",  p.erosionGrit);
        setP("punch_slam",    p.punchSlam);
        setP("soft_clip",     p.softClip);
        setP("glide_time",    p.glideTime);
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
        default: break;
    }

    lastMidiCC.store(ccNumber, std::memory_order_relaxed);
}

void ExtasisDonkerAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = (juce::uint32)samplesPerBlock;
    spec.numChannels = (juce::uint32)getTotalNumOutputChannels();

    synth.prepare(spec);
    scopeFifo.reset();
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

    // 1. Drain thread-safe audition MIDI notes from GUI
    {
        juce::MidiBuffer auditionCopy;
        {
            const juce::SpinLock::ScopedTryLockType lock(auditionMidiLock);
            if (lock.isLocked() && !pendingAuditionMidi.isEmpty())
                auditionCopy.swapWith(pendingAuditionMidi);
        }
        for (const auto metadata : auditionCopy)
        {
            midiActivity.store(true, std::memory_order_relaxed);
            synth.handleMidiEvent(metadata.getMessage());
        }
    }

    // 2. Handle incoming DAW MIDI (Notes, Pitch Wheel, Mod Wheel CC 1, and Mapped CCs)
    for (const auto metadata : midiMessages)
    {
        auto msg = metadata.getMessage();
        midiActivity.store(true, std::memory_order_relaxed);

        if (msg.isController())
        {
            if (msg.getControllerNumber() == 1)
            {
                // Mod Wheel (CC 1) drives dynamic filter opening in DonkSynth
                synth.handleMidiEvent(msg);
                lastMidiCC.store(1, std::memory_order_relaxed);
            }
            else
            {
                handleMidiCC(msg.getControllerNumber(), msg.getControllerValue());
            }
        }
        else
        {
            synth.handleMidiEvent(msg);
        }
    }

    // 3. Transport Info
    double bpm = 120.0;
    double ppqPosition = 0.0;
    bool isHostPlaying = false;
    if (auto* playHead = getPlayHead())
    {
        if (auto posInfo = playHead->getPosition())
        {
            if (posInfo->getBpm().hasValue()) bpm = *posInfo->getBpm();
            if (posInfo->getPpqPosition().hasValue()) ppqPosition = *posInfo->getPpqPosition();
            isHostPlaying = posInfo->getIsPlaying();
        }
    }

    // 4. Read cached atomic parameters (zero allocations / lookups)
    float fmAmount      = pFmAmount->load(std::memory_order_relaxed) * 0.01f;
    float fmTune        = pFmTune->load(std::memory_order_relaxed);
    float wavePosition  = pWavePosition->load(std::memory_order_relaxed) * 0.01f;
    float fmEnvDecay    = pFmEnv->load(std::memory_order_relaxed);
    float pumpAmount    = pPumpAmount->load(std::memory_order_relaxed) * 0.01f;
    float modAmount     = pModAmount->load(std::memory_order_relaxed) * 0.01f;
    float pitchDrop     = pPitchDrop->load(std::memory_order_relaxed) * 0.01f;
    float txCrunch      = pTxCrunch->load(std::memory_order_relaxed) * 0.01f;
    float erosionGrit   = pErosionGrit->load(std::memory_order_relaxed) * 0.01f;
    float punchSlam     = pPunchSlam->load(std::memory_order_relaxed) * 0.01f;
    bool enableSoftClip = pSoftClip->load(std::memory_order_relaxed) > 0.5f;
    float subGain       = pSubGain->load(std::memory_order_relaxed);
    float subTone       = pSubTone->load(std::memory_order_relaxed) * 0.01f;
    float filterCutoff  = pFilterCutoff->load(std::memory_order_relaxed);
    float filterReso    = pFilterReso->load(std::memory_order_relaxed) * 0.01f;
    float reverbSpace   = pReverbSpace->load(std::memory_order_relaxed) * 0.01f;
    float glideMs       = pGlideTime->load(std::memory_order_relaxed);
    float masterVolDb   = pMasterVol->load(std::memory_order_relaxed);
    float masterVolGain = juce::Decibels::decibelsToGain(masterVolDb);

    // 5. Synthesize audio block
    synth.process(buffer,
                  fmAmount,
                  fmTune,
                  fmEnvDecay,
                  pumpAmount,
                  bpm,
                  ppqPosition,
                  isHostPlaying,
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

    // 6. Lock-free oscilloscope push & output peak meter update
    int numSamples = buffer.getNumSamples();
    if (numSamples > 0)
    {
        const float* leftPtr = buffer.getReadPointer(0);
        pushScopeSamples(leftPtr, numSamples);

        float blockPeak = buffer.getMagnitude(0, numSamples);
        float prevPeak = outputPeak.load(std::memory_order_relaxed);
        while (blockPeak > prevPeak && !outputPeak.compare_exchange_weak(prevPeak, blockPeak, std::memory_order_relaxed)) {}
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
    if (xmlState != nullptr && xmlState->hasTagName(apvts.state.getType()))
    {
        apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
        currentProgramIndex = apvts.state.getProperty("current_program", 0);
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ExtasisDonkerAudioProcessor();
}
