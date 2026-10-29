#include "Core/ProjectState.h"

ProjectState::ProjectState() = default;

void ProjectState::setVersion (int version)
{
    version_ = version;
}

int ProjectState::getVersion() const noexcept
{
    return version_;
}

void ProjectState::setKitName (const juce::String& kitName)
{
    kitName_ = kitName;
}

juce::String ProjectState::getKitName() const
{
    return kitName_;
}

void ProjectState::setCurrentPreset (const juce::String& presetName)
{
    presetName_ = presetName;
}

juce::String ProjectState::getCurrentPreset() const
{
    return presetName_;
}

void ProjectState::setGrooveCount (int count)
{
    grooveCount_ = count;
}

int ProjectState::getGrooveCount() const noexcept
{
    return grooveCount_;
}

juce::ValueTree ProjectState::toValueTree() const
{
    juce::ValueTree tree ("project-state");
    tree.setProperty ("version", version_, nullptr);
    tree.setProperty ("kitName", kitName_, nullptr);
    tree.setProperty ("presetName", presetName_, nullptr);
    tree.setProperty ("grooveCount", grooveCount_, nullptr);
    return tree;
}

ProjectState ProjectState::fromValueTree (const juce::ValueTree& tree)
{
    ProjectState state;
    state.version_ = tree.getProperty ("version");
    state.kitName_ = tree.getProperty ("kitName").toString();
    state.presetName_ = tree.getProperty ("presetName").toString();
    state.grooveCount_ = tree.getProperty ("grooveCount");
    return state;
}

bool ProjectState::saveToFile (const juce::File& file) const
{
    auto parent = file.getParentDirectory();
    if (!parent.exists())
        parent.createDirectories();

    const auto tree = toValueTree();
    const auto xml = tree.createXml();
    return xml != nullptr && file.replaceWithText (xml->toString());
}

ProjectState ProjectState::loadFromFile (const juce::File& file)
{
    if (!file.existsAsFile())
        return ProjectState();

    const auto xml = juce::parseXML (file);
    if (xml == nullptr)
        return ProjectState();

    const auto tree = juce::ValueTree::fromXml (*xml);
    if (!tree.isValid())
        return ProjectState();

    return ProjectState::fromValueTree (tree);
}
