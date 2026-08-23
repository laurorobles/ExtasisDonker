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
    COMPANY_NAME "Extasis Records"
    BUNDLE_ID "com.laurorobles.ExtasisDonker"
    PLUGIN_MANUFACTURER_CODE Exta
    PLUGIN_CODE Ednk
    FORMATS VST3 AU Standalone CLAP
    PRODUCT_NAME "ExtasisDonker"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    ICON_BIG assets/logo.png
    ICON_SMALL assets/logo.png
    COPY_PLUGIN_AFTER_BUILD TRUE
)

juce_generate_juce_header(ExtasisDonker)

target_sources(ExtasisDonker PRIVATE
    Source/LicenseManager.h
    Source/GUI/ActivationOverlay.h
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

# Custom post-build step to guarantee installation and app icon
add_custom_command(TARGET ExtasisDonker_Standalone POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/assets/logo.icns"
        "$<TARGET_BUNDLE_DIR:ExtasisDonker_Standalone>/Contents/Resources/Icon.icns"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/assets/logo.icns"
        "$<TARGET_BUNDLE_DIR:ExtasisDonker_Standalone>/Contents/Resources/logo.icns"
    COMMENT "Copying logo.icns to Standalone app bundle"
)

add_custom_command(TARGET ExtasisDonker_VST3 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$ENV{HOME}/Library/Audio/Plug-Ins/VST3"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "$<TARGET_BUNDLE_DIR:ExtasisDonker_VST3>"
        "$ENV{HOME}/Library/Audio/Plug-Ins/VST3/ExtasisDonker.vst3"
    COMMENT "Installing VST3 to ~/Library/Audio/Plug-Ins/VST3/"
)

add_custom_command(TARGET ExtasisDonker_AU POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$ENV{HOME}/Library/Audio/Plug-Ins/Components"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "$<TARGET_BUNDLE_DIR:ExtasisDonker_AU>"
        "$ENV{HOME}/Library/Audio/Plug-Ins/Components/ExtasisDonker.component"
    COMMENT "Installing AU to ~/Library/Audio/Plug-Ins/Components/"
)
