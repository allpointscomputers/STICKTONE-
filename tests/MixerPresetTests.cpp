#include "Presets/PresetManager.h"

juce::ValueTree PresetData::toValueTree() const
{
    juce::ValueTree tree ("preset");
    tree.setProperty ("id", id, nullptr);
    tree.setProperty ("name", name, nullptr);
    tree.setProperty ("kitName", kitName, nullptr);
    tree.setProperty ("notes", notes, nullptr);
    tree.appendChild (mixerState.toValueTree(), nullptr);
    return tree;
}

PresetData PresetData::fromValueTree (const juce::ValueTree& tree)
{
    PresetData data;
    data.id = tree.getProperty ("id").toString();
    data.name = tree.getProperty ("name").toString();
    data.kitName = tree.getProperty ("kitName").toString();
    data.notes = tree.getProperty ("notes").toString();

    for (const auto& child : tree)
    {
        if (child.hasType ("mixer"))
            data.mixerState = MixerState::fromValueTree (child);
    }

    return data;
}

PresetManager::PresetManager()
{
    PresetData demo;
    demo.id = "factory-demo";
    demo.name = "Factory Demo";
    demo.kitName = "Wavey Beat Starter Kit";
    demo.notes = "Starter mix and state";
    addPreset (demo);
}

void PresetManager::addPreset (const PresetData& preset)
{
    for (auto it = presets_.begin(); it != presets_.end(); ++it)
    {
        if ((*it).id == preset.id)
        {
            *it = preset;
            return;
        }
    }

    presets_.add (preset);
}

void PresetManager::removePreset (const juce::String& presetId)
{
    presets_.removeIf ([&] (const PresetData& preset) { return preset.id == presetId; });
}

juce::Array<PresetData> PresetManager::getPresets() const
{
    return presets_;
}

PresetData PresetManager::getPresetById (const juce::String& presetId) const
{
    for (const auto& preset : presets_)
    {
        if (preset.id == presetId)
            return preset;
    }
    return PresetData{};
}

juce::ValueTree PresetManager::toValueTree() const
{
    juce::ValueTree root ("preset-manager");
    for (const auto& preset : presets_)
        root.appendChild (preset.toValueTree(), nullptr);
    return root;
}

PresetManager PresetManager::fromValueTree (const juce::ValueTree& tree)
{
    PresetManager manager;
    for (const auto& child : tree)
    {
        if (child.hasType ("preset"))
            manager.addPreset (PresetData::fromValueTree (child));
    }
    return manager;
}

bool PresetManager::saveToFile (const juce::File& file) const
{
    auto parent = file.getParentDirectory();
    if (!parent.exists())
        parent.createDirectories();

    const auto tree = toValueTree();
    const auto xml = tree.createXml();
    return xml != nullptr && file.replaceWithText (xml->toString());
}

PresetManager PresetManager::loadFromFile (const juce::File& file)
{
    if (!file.existsAsFile())
        return PresetManager();

    const auto xml = juce::parseXML (file);
    if (xml == nullptr)
        return PresetManager();

    const auto tree = juce::ValueTree::fromXml (*xml);
    if (!tree.isValid())
        return PresetManager();

    return PresetManager::fromValueTree (tree);
}
