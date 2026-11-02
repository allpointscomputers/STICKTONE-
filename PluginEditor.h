#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class DrumForgeAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit DrumForgeAudioProcessorEditor(DrumForgeAudioProcessor&);
    ~DrumForgeAudioProcessorEditor() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    DrumForgeAudioProcessor& processor;
    juce::Label title;
    juce::Label subtitle;
    std::array<juce::TextButton, DrumForgeAudioProcessor::kDrumCount> drumButtons;
    std::array<juce::Slider, DrumForgeAudioProcessor::kDrumCount> levelSliders;
    std::array<juce::Label, DrumForgeAudioProcessor::kDrumCount> drumLabels;
    juce::Label status;
    juce::TextButton grooveButton { "Play 1-bar groove" };
    juce::TextButton stopButton { "Stop" };
    juce::TextButton exportButton { "Export MIDI (next milestone)" };
    juce::TextButton saveButton { "Save preset" };
    juce::TextButton loadButton { "Load preset" };
    juce::TextButton tapButton { "Tap2Find (next milestone)" };
    juce::TextButton songButton { "Song Creator (next milestone)" };
    juce::TextButton mixerButton { "Mixer (basic levels)" };
    juce::Label tempoLabel;
    juce::Slider tempoSlider;
    juce::Timer timer;
    bool groovePlaying = false;
    int grooveStep = 0;
    int timerTicks = 0;

    void timerCallback() override;
    void startGroove();
    void stopGroove();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DrumForgeAudioProcessorEditor)
};
