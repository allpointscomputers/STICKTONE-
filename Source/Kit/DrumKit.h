#pragma once

#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>

struct DrumPieceSettings
{
    juce::String id;
    juce::String name;
    int midiNote = 36;
    float gain = 1.0f;
    float pan = 0.0f;
    float tuning = 1.0f;
    bool enabled = true;
};

class DrumKitDefinition
{
public:
    DrumKitDefinition();
    explicit DrumKitDefinition (const juce::String& kitName);

    static DrumKitDefinition createDefault();

    void setName (const juce::String& newName);
    [[nodiscard]] juce::String getName() const;

    void setPieces (const juce::Array<DrumPieceSettings>& pieces);
    [[nodiscard]] juce::Array<DrumPieceSettings> getPieces() const;

    [[nodiscard]] DrumPieceSettings getPieceById (const juce::String& id) const;
    void setPieceValue (const juce::String& id, const juce::String& propertyName, float value);

    [[nodiscard]] juce::ValueTree toValueTree() const;
    static DrumKitDefinition fromValueTree (const juce::ValueTree& tree);

    bool saveToFile (const juce::File& file) const;
    static DrumKitDefinition loadFromFile (const juce::File& file);

private:
    juce::String name_;
    juce::Array<DrumPieceSettings> pieces_;
};
