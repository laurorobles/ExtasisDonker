cmake_minimum_required(VERSION 3.15)
project(ExtasisDonker VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# JUCE integration
add_subdirectory(/Users/babyonk1/.gemini/antigravity/scratch/JUCE ${CMAKE_BINARY_DIR}/JUCE)

juce_add_binary_data(ExtasisDonker_Data
    SOURCES
        assets/logo.png
)

juce_add_plugin(ExtasisDonker
    PLUGIN_MANUFACTURER_CODE Exta
    PLUGIN_CODE Ednk
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "ExtasisDonker"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
)

juce_generate_juce_header(ExtasisDonker)

target_sources(ExtasisDonker PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h
    Source/DSP/DonkOscillator.h
    Source/DSP/SnappyEnvelope.h
    Source/DSP/DonkSynth.h
    Source/DSP/StereoProcessor.h
    Source/GUI/TX81ZLookAndFeel.h
    Source/GUI/TX81ZLookAndFeel.cpp
    Source/GUI/TX81ZDisplay.h
    Source/GUI/TX81ZDisplay.cpp
    Source/GUI/DonkTriggerButton.h
    Source/GUI/DonkTriggerButton.cpp
)

target_compile_definitions(ExtasisDonker PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(ExtasisDonker PRIVATE
    ExtasisDonker_Data
    juce::juce_audio_utils
    juce::juce_audio_processors
    juce::juce_dsp
    juce::juce_gui_basics
    juce::juce_gui_extra
)
