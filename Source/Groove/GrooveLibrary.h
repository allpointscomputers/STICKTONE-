#pragma once

#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>

class GrooveMetadata
{
public:
    juce::String id;
    juce::String name;
    juce::String category;
    juce::String genre;
    float bpm = 120.0f;
    int beatsPerBar = 4;
    int subdivision = 4;
    bool swing = false;
    juce::Array<int> notes;

    juce::ValueTree toValueTree() const;
    static GrooveMetadata fromValueTree (const juce::ValueTree& tree);
};

class GrooveLibrary
{
public:
    GrooveLibrary();

    void addGroove (const GrooveMetadata& groove);
    void removeGroove (const juce::String& grooveId);
    [[nodiscard]] juce::Array<GrooveMetadata> getGrooves() const;
    [[nodiscard]] juce::Array<GrooveMetadata> searchByName (const juce::String& query) const;
    [[nodiscard]] juce::Array<GrooveMetadata> getByCategory (const juce::String& genreOrCategory) const;

    [[nodiscard]] juce::ValueTree toValueTree() const;
    static GrooveLibrary fromValueTree (const juce::ValueTree& tree);

    bool saveToFile (const juce::File& file) const;
    static GrooveLibrary loadFromFile (const juce::File& file);

private:
    juce::Array<GrooveMetadata> grooves_;
};
