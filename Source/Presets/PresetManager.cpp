#pragma once

#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>

#include "Mixer/MixerState.h"

struct PresetData
{
    juce::String id;
    juce::String name;
    juce::String kitName;
    MixerState mixerState;
    juce::String notes;

    juce::ValueTree toValueTree() const;
    static PresetData fromValueTree (const juce::ValueTree& tree);
};

class PresetManager
{
public:
    PresetManager();

    void addPreset (const PresetData& preset);
    void removePreset (const juce::String& presetId);
    [[nodiscard]] juce::Array<PresetData> getPresets() const;
    [[nodiscard]] PresetData getPresetById (const juce::String& presetId) const;

    [[nodiscard]] juce::ValueTree toValueTree() const;
    static PresetManager fromValueTree (const juce::ValueTree& tree);

    bool saveToFile (const juce::File& file) const;
    static PresetManager loadFromFile (const juce::File& file);

private:
    juce::Array<PresetData> presets_;
};
