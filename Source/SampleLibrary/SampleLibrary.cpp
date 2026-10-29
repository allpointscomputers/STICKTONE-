#include "SampleLibrary/SampleLibrary.h"

namespace
{
    bool looksLikeAudioFile (const juce::File& file)
    {
        const auto extension = file.getFileExtension().toLowerCase();
        return extension == ".wav" || extension == ".aiff" || extension == ".aif";
    }
}

juce::ValueTree SampleMetadata::toValueTree() const
{
    juce::ValueTree tree ("sample");
    tree.setProperty ("id", id, nullptr);
    tree.setProperty ("name", name, nullptr);
    tree.setProperty ("drumPieceId", drumPieceId, nullptr);
    tree.setProperty ("filePath", filePath, nullptr);
    tree.setProperty ("velocityLayer", velocityLayer, nullptr);
    tree.setProperty ("roundRobinIndex", roundRobinIndex, nullptr);
    tree.setProperty ("sampleRate", sampleRate, nullptr);
    tree.setProperty ("bitDepth", bitDepth, nullptr);
    tree.setProperty ("status", static_cast<int> (status), nullptr);
    return tree;
}

SampleMetadata SampleMetadata::fromValueTree (const juce::ValueTree& tree)
{
    SampleMetadata metadata;
    metadata.id = tree.getProperty ("id").toString();
    metadata.name = tree.getProperty ("name").toString();
    metadata.drumPieceId = tree.getProperty ("drumPieceId").toString();
    metadata.filePath = tree.getProperty ("filePath").toString();
    metadata.velocityLayer = tree.getProperty ("velocityLayer");
    metadata.roundRobinIndex = tree.getProperty ("roundRobinIndex");
    metadata.sampleRate = tree.getProperty ("sampleRate");
    metadata.bitDepth = tree.getProperty ("bitDepth");
    metadata.status = static_cast<SampleMetadata::SampleStatus> (tree.getProperty ("status"));
    return metadata;
}

SampleLibrary::SampleLibrary() = default;

void SampleLibrary::addSample (const SampleMetadata& metadata)
{
    if (metadata.id.isEmpty())
        return;

    for (auto it = samples_.begin(); it != samples_.end(); ++it)
    {
        if ((*it).id == metadata.id)
        {
            *it = metadata;
            return;
        }
    }

    samples_.add (metadata);
}

void SampleLibrary::removeSample (const juce::String& sampleId)
{
    samples_.removeIf ([&] (const SampleMetadata& sample) { return sample.id == sampleId; });
}

void SampleLibrary::clear()
{
    samples_.clear();
}

juce::Array<SampleMetadata> SampleLibrary::getSamples() const
{
    return samples_;
}

juce::Array<SampleMetadata> SampleLibrary::getSamplesForPiece (const juce::String& drumPieceId) const
{
    juce::Array<SampleMetadata> matches;
    for (const auto& sample : samples_)
    {
        if (sample.drumPieceId == drumPieceId)
            matches.add (sample);
    }
    return matches;
}

bool SampleLibrary::validateFile (const juce::File& file) const
{
    if (!file.existsAsFile())
        return false;

    if (!looksLikeAudioFile (file))
        return false;

    const auto* stream = file.createInputStream();
    if (stream == nullptr)
        return false;

    std::unique_ptr<juce::FileInputStream> input (stream);
    const auto header = input->read (12);
    if (header.getSize() < 12)
        return false;

    const auto riff = header.getData();
    const bool isWave = riff[0] == 'R' && riff[1] == 'I' && riff[2] == 'F' && riff[3] == 'F';
    const bool isAiff = riff[0] == 'F' && riff[1] == 'O' && riff[2] == 'R' && riff[3] == 'M';
    return isWave || isAiff;
}

juce::String SampleLibrary::getStatusSummary() const
{
    if (samples_.isEmpty())
        return "No samples loaded";

    int valid = 0;
    int missing = 0;
    int corrupt = 0;
    int unsupported = 0;

    for (const auto& sample : samples_)
    {
        switch (sample.status)
        {
            case SampleMetadata::SampleStatus::valid: ++valid; break;
            case SampleMetadata::SampleStatus::missing: ++missing; break;
            case SampleMetadata::SampleStatus::corrupt: ++corrupt; break;
            case SampleMetadata::SampleStatus::unsupported: ++unsupported; break;
        }
    }

    return "valid=" + juce::String (valid) + ", missing=" + juce::String (missing) + ", corrupt=" + juce::String (corrupt) + ", unsupported=" + juce::String (unsupported);
}

juce::ValueTree SampleLibrary::toValueTree() const
{
    juce::ValueTree root ("sample-library");
    for (const auto& sample : samples_)
        root.appendChild (sample.toValueTree(), nullptr);
    return root;
}

SampleLibrary SampleLibrary::fromValueTree (const juce::ValueTree& tree)
{
    SampleLibrary library;
    for (const auto& child : tree)
    {
        if (child.hasType ("sample"))
            library.addSample (SampleMetadata::fromValueTree (child));
    }
    return library;
}

bool SampleLibrary::saveToFile (const juce::File& file) const
{
    auto parent = file.getParentDirectory();
    if (!parent.exists())
        parent.createDirectories();

    const auto tree = toValueTree();
    const auto xml = tree.createXml();
    return xml != nullptr && file.replaceWithText (xml->toString());
}

SampleLibrary SampleLibrary::loadFromFile (const juce::File& file)
{
    if (!file.existsAsFile())
        return SampleLibrary();

    const auto xml = juce::parseXML (file);
    if (xml == nullptr)
        return SampleLibrary();

    const auto tree = juce::ValueTree::fromXml (*xml);
    if (!tree.isValid())
        return SampleLibrary();

    return SampleLibrary::fromValueTree (tree);
}
