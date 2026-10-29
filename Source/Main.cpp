#include "Mixer/MixerState.h"

juce::ValueTree ChannelStripSettings::toValueTree() const
{
    juce::ValueTree tree ("channel");
    tree.setProperty ("id", id, nullptr);
    tree.setProperty ("name", name, nullptr);
    tree.setProperty ("gain", gain, nullptr);
    tree.setProperty ("pan", pan, nullptr);
    tree.setProperty ("muted", muted, nullptr);
    tree.setProperty ("solo", solo, nullptr);
    tree.setProperty ("peak", peak, nullptr);
    return tree;
}

ChannelStripSettings ChannelStripSettings::fromValueTree (const juce::ValueTree& tree)
{
    ChannelStripSettings strip;
    strip.id = tree.getProperty ("id").toString();
    strip.name = tree.getProperty ("name").toString();
    strip.gain = static_cast<float> (tree.getProperty ("gain"));
    strip.pan = static_cast<float> (tree.getProperty ("pan"));
    strip.muted = tree.getProperty ("muted");
    strip.solo = tree.getProperty ("solo");
    strip.peak = static_cast<float> (tree.getProperty ("peak"));
    return strip;
}

MixerState::MixerState()
{
    addChannel ({ "kick", "Kick", 1.0f, 0.0f, false, false, 0.0f });
    addChannel ({ "snare", "Snare", 0.95f, 0.1f, false, false, 0.0f });
    addChannel ({ "hat", "Hi-Hat", 0.7f, 0.15f, false, false, 0.0f });
    addChannel ({ "tom", "Tom", 0.85f, -0.1f, false, false, 0.0f });
    addChannel ({ "crash", "Crash", 0.8f, 0.25f, false, false, 0.0f });
}

void MixerState::addChannel (const ChannelStripSettings& strip)
{
    channels_.add (strip);
}

void MixerState::clearChannels()
{
    channels_.clear();
}

juce::Array<ChannelStripSettings> MixerState::getChannels() const
{
    return channels_;
}

void MixerState::setMasterGain (float gain)
{
    masterGain_ = juce::jlimit (0.0f, 2.0f, gain);
}

float MixerState::getMasterGain() const noexcept
{
    return masterGain_;
}

juce::ValueTree MixerState::toValueTree() const
{
    juce::ValueTree root ("mixer");
    root.setProperty ("masterGain", masterGain_, nullptr);
    for (const auto& channel : channels_)
        root.appendChild (channel.toValueTree(), nullptr);
    return root;
}

MixerState MixerState::fromValueTree (const juce::ValueTree& tree)
{
    MixerState state;
    state.masterGain_ = static_cast<float> (tree.getProperty ("masterGain"));
    for (const auto& child : tree)
    {
        if (child.hasType ("channel"))
            state.addChannel (ChannelStripSettings::fromValueTree (child));
    }
    return state;
}

bool MixerState::saveToFile (const juce::File& file) const
{
    auto parent = file.getParentDirectory();
    if (!parent.exists())
        parent.createDirectories();

    const auto tree = toValueTree();
    const auto xml = tree.createXml();
    return xml != nullptr && file.replaceWithText (xml->toString());
}

MixerState MixerState::loadFromFile (const juce::File& file)
{
    if (!file.existsAsFile())
        return MixerState();

    const auto xml = juce::parseXML (file);
    if (xml == nullptr)
        return MixerState();

    const auto tree = juce::ValueTree::fromXml (*xml);
    if (!tree.isValid())
        return MixerState();

    return MixerState::fromValueTree (tree);
}


