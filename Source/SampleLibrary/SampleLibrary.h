#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>

class SampleMetadata
{
public:
    enum class SampleStatus
    {
        valid,
        missing,
        corrupt,
        unsupported
    };

    juce::String id;
    juce::String name;
    juce::String drumPieceId;
    juce::String filePath;
    int velocityLayer = 1;
    int roundRobinIndex = 1;
    int sampleRate = 44100;
    int bitDepth = 16;
    SampleStatus status = SampleStatus::valid;

    juce::ValueTree toValueTree() const;
    static SampleMetadata fromValueTree (const juce::ValueTree& tree);
};

class SampleLibrary
{
public:
    SampleLibrary();

    void addSample (const SampleMetadata& metadata);
    void removeSample (const juce::String& sampleId);
    void clear();

    [[nodiscard]] juce::Array<SampleMetadata> getSamples() const;
    [[nodiscard]] juce::Array<SampleMetadata> getSamplesForPiece (const juce::String& drumPieceId) const;

    [[nodiscard]] bool validateFile (const juce::File& file) const;
    [[nodiscard]] juce::String getStatusSummary() const;

    [[nodiscard]] juce::ValueTree toValueTree() const;
    static SampleLibrary fromValueTree (const juce::ValueTree& tree);

    bool saveToFile (const juce::File& file) const;
    static SampleLibrary loadFromFile (const juce::File& file);

private:
    juce::Array<SampleMetadata> samples_;
};
