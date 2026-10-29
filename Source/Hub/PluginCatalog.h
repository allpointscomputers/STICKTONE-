#pragma once

#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>

#include "License/LicenseManager.h"

struct PluginCatalogItem
{
    juce::String id;
    juce::String name;
    juce::String category;
    juce::String version = "1.0.0";
    bool requiresLicense = true;
    bool trialEnabled = true;
    double price = 0.0;
    juce::String fileUrl;
    juce::String description;

    juce::ValueTree toValueTree() const;
    static PluginCatalogItem fromValueTree (const juce::ValueTree& tree);
};

struct SoundPackItem
{
    juce::String id;
    juce::String name;
    juce::String category;
    double price = 0.0;
    juce::String fileUrl;
    juce::String description;

    juce::ValueTree toValueTree() const;
    static SoundPackItem fromValueTree (const juce::ValueTree& tree);
};

class PluginHub
{
public:
    void addPlugin (const PluginCatalogItem& plugin);
    void addSoundPack (const SoundPackItem& pack);

    juce::Array<PluginCatalogItem> getPlugins() const;
    juce::Array<SoundPackItem> getSoundPacks() const;

    bool canAccessPlugin (const PluginCatalogItem& plugin,
                          const LicenseManager& manager) const;

    bool canDownloadPack (const SoundPackItem& pack,
                          const LicenseManager& manager) const;

    juce::ValueTree toValueTree() const;
    static PluginHub fromValueTree (const juce::ValueTree& tree);

private:
    juce::Array<PluginCatalogItem> plugins_;
    juce::Array<SoundPackItem> soundPacks_;
};
