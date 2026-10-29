#pragma once

#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>

class ProjectState
{
public:
    ProjectState();

    void setVersion (int version);
    [[nodiscard]] int getVersion() const noexcept;

    void setKitName (const juce::String& kitName);
    [[nodiscard]] juce::String getKitName() const;

    void setCurrentPreset (const juce::String& presetName);
    [[nodiscard]] juce::String getCurrentPreset() const;

    void setGrooveCount (int count);
    [[nodiscard]] int getGrooveCount() const noexcept;

    [[nodiscard]] juce::ValueTree toValueTree() const;
    static ProjectState fromValueTree (const juce::ValueTree& tree);

    bool saveToFile (const juce::File& file) const;
    static ProjectState loadFromFile (const juce::File& file);

private:
    int version_ = 1;
    juce::String kitName_ = "Wavey Beat Starter Kit";
    juce::String presetName_ = "Factory Demo";
    int grooveCount_ = 0;
};
