cmake_minimum_required(VERSION 3.22)

project(STICKTONE-BY-WAVEY-BEAT VERSION 0.1.0 LANGUAGES CXX)

include(FetchContent)

set(JUCE_BUILD_EXTRAS OFF)
set(JUCE_BUILD_EXAMPLES OFF)
set(JUCE_USE_CURL OFF)

FetchContent_Declare(
    juce
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.8
)

FetchContent_MakeAvailable(juce)

juce_add_gui_app(StickToneApp
    COMPANY_NAME "Wavey Beat"
    PRODUCT_NAME "STICKTONE BY WAVEY BEAT"
    VERSION ${PROJECT_VERSION}
)

target_compile_features(StickToneApp PRIVATE cxx_std_17)
target_include_directories(StickToneApp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Source)

target_sources(StickToneApp PRIVATE
    Source/Main.cpp
    Source/Audio/DrumSynth.cpp
    Source/Kit/DrumKit.cpp
    Source/SampleLibrary/SampleLibrary.cpp
    Source/Groove/GrooveLibrary.cpp
    Source/Core/ProjectState.cpp
    Source/Mixer/MixerState.cpp
    Source/Presets/PresetManager.cpp
)

target_link_libraries(StickToneApp PRIVATE
    juce::juce_audio_basics
    juce::juce_audio_devices
    juce::juce_audio_utils
    juce::juce_core
    juce::juce_data_structures
    juce::juce_events
    juce::juce_graphics
    juce::juce_gui_basics
    juce::juce_gui_extra
)

juce_generate_juce_header(StickToneApp)

enable_testing()

add_executable(StickToneTests
    tests/DrumSynthTests.cpp
    tests/GrooveLibraryTests.cpp
    tests/MixerPresetTests.cpp
    Source/Audio/DrumSynth.cpp
    Source/Kit/DrumKit.cpp
    Source/SampleLibrary/SampleLibrary.cpp
    Source/Groove/GrooveLibrary.cpp
    Source/Core/ProjectState.cpp
    Source/Mixer/MixerState.cpp
    Source/Presets/PresetManager.cpp
)

target_compile_features(StickToneTests PRIVATE cxx_std_17)
target_include_directories(StickToneTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Source)
target_link_libraries(StickToneTests PRIVATE
    juce::juce_audio_basics
    juce::juce_core
    juce::juce_data_structures
    juce::juce_events
)

add_test(NAME StickToneTests COMMAND StickToneTests)
