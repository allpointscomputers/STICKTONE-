#pragma once

#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>

struct ChannelStripSettings
{
    juce::String id;
    juce::String name;
    float gain = 1.0f;
    float pan = 0.0f;
    bool muted = false;
    bool solo = false;
    float peak = 0.0f;

    juce::ValueTree toValueTree() const;
    static ChannelStripSettings fromValueTree (const juce::ValueTree& tree);
};

class MixerState
{
public:
    MixerState();

    void addChannel (const ChannelStripSettings& strip);
    void clearChannels();
    [[nodiscard]] juce::Array<ChannelStripSettings> getChannels() const;

    void setMasterGain (float gain);
    [[nodiscard]] float getMasterGain() const noexcept;

    [[nodiscard]] juce::ValueTree toValueTree() const;
    static MixerState fromValueTree (const juce::ValueTree& tree);

    bool saveToFile (const juce::File& file) const;
    static MixerState loadFromFile (const juce::File& file);

private:
    juce::Array<ChannelStripSettings> channels_;
    float masterGain_ = 1.0f;
};
