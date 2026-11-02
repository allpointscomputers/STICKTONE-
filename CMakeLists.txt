cmake_minimum_required(VERSION 3.22)
project(StickTone VERSION 0.1.0 LANGUAGES CXX)

# Pin the JUCE checkout to a tested release for repeatable CI builds.
# The GitHub Actions workflow checks out JUCE 8.0.6 into ./JUCE.
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/JUCE/CMakeLists.txt")
    message(FATAL_ERROR "JUCE not found. Clone the configured JUCE release into ./JUCE (see README.md).")
endif()
add_subdirectory(JUCE)

set(STICKTONE_FORMATS VST3 Standalone)
if(APPLE)
    list(APPEND STICKTONE_FORMATS AU)
endif()

juce_add_plugin(StickTone
    COMPANY_NAME "Wavey Beat"
    BUNDLE_ID "com.waveybeat.sticktone"
    PLUGIN_MANUFACTURER_CODE WvBt
    PLUGIN_CODE StkT
    PRODUCT_NAME "StickTone"
    DESCRIPTION "StickTone by Wavey Beat - Cross-platform drum instrument"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD FALSE
    FORMATS ${STICKTONE_FORMATS}
    VST3_CATEGORIES Instrument Synth Drum
    AU_MAIN_TYPE kAudioUnitType_MusicDevice
)

target_sources(StickTone PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
)

target_compile_features(StickTone PRIVATE cxx_std_17)

target_compile_definitions(StickTone PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(StickTone PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)
