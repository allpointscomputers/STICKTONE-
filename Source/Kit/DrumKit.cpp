#include "Kit/DrumKit.h"

namespace
{
    DrumPieceSettings makePiece (const juce::String& id,
                                const juce::String& name,
                                int midiNote,
                                float gain,
                                float pan,
                                float tuning,
                                bool enabled = true)
    {
        DrumPieceSettings piece;
        piece.id = id;
        piece.name = name;
        piece.midiNote = midiNote;
        piece.gain = gain;
        piece.pan = pan;
        piece.tuning = tuning;
        piece.enabled = enabled;
        return piece;
    }
}

DrumKitDefinition::DrumKitDefinition() : name_ ("Starter Kit")
{
    *this = createDefault();
}

DrumKitDefinition::DrumKitDefinition (const juce::String& kitName) : name_ (kitName)
{
    pieces_.add (makePiece ("kick", "Kick", 36, 1.0f, 0.0f, 1.0f, true));
    pieces_.add (makePiece ("snare", "Snare", 38, 0.95f, 0.1f, 1.0f, true));
    pieces_.add (makePiece ("hat", "Hi-Hat", 42, 0.75f, 0.2f, 1.0f, true));
    pieces_.add (makePiece ("tom", "Tom", 48, 0.85f, -0.1f, 1.0f, true));
    pieces_.add (makePiece ("crash", "Crash", 49, 0.8f, 0.3f, 1.0f, true));
    pieces_.add (makePiece ("ride", "Ride", 51, 0.7f, 0.4f, 1.0f, true));
}

DrumKitDefinition DrumKitDefinition::createDefault()
{
    DrumKitDefinition kit ("Wavey Beat Starter Kit");
    return kit;
}

void DrumKitDefinition::setName (const juce::String& newName)
{
    name_ = newName;
}

juce::String DrumKitDefinition::getName() const
{
    return name_;
}

void DrumKitDefinition::setPieces (const juce::Array<DrumPieceSettings>& pieces)
{
    pieces_ = pieces;
}

juce::Array<DrumPieceSettings> DrumKitDefinition::getPieces() const
{
    return pieces_;
}

DrumPieceSettings DrumKitDefinition::getPieceById (const juce::String& id) const
{
    for (const auto& piece : pieces_)
    {
        if (piece.id == id)
            return piece;
    }

    return makePiece (id, id, 36, 1.0f, 0.0f, 1.0f, true);
}

void DrumKitDefinition::setPieceValue (const juce::String& id, const juce::String& propertyName, float value)
{
    for (auto& piece : pieces_)
    {
        if (piece.id == id)
        {
            if (propertyName == "gain")
                piece.gain = value;
            else if (propertyName == "pan")
                piece.pan = value;
            else if (propertyName == "tuning")
                piece.tuning = value;
            return;
        }
    }
}

juce::ValueTree DrumKitDefinition::toValueTree() const
{
    juce::ValueTree tree ("kit");
    tree.setProperty ("name", name_, nullptr);

    for (const auto& piece : pieces_)
    {
        juce::ValueTree pieceTree ("piece");
        pieceTree.setProperty ("id", piece.id, nullptr);
        pieceTree.setProperty ("name", piece.name, nullptr);
        pieceTree.setProperty ("midiNote", piece.midiNote, nullptr);
        pieceTree.setProperty ("gain", piece.gain, nullptr);
        pieceTree.setProperty ("pan", piece.pan, nullptr);
        pieceTree.setProperty ("tuning", piece.tuning, nullptr);
        pieceTree.setProperty ("enabled", piece.enabled, nullptr);
        tree.appendChild (pieceTree, nullptr);
    }

    return tree;
}

DrumKitDefinition DrumKitDefinition::fromValueTree (const juce::ValueTree& tree)
{
    DrumKitDefinition kit (tree.hasProperty ("name") ? tree.getProperty ("name").toString() : "Starter Kit");
    juce::Array<DrumPieceSettings> pieces;

    for (const auto& child : tree)
    {
        if (child.hasType ("piece"))
        {
            DrumPieceSettings piece;
            piece.id = child.getProperty ("id").toString();
            piece.name = child.getProperty ("name").toString();
            piece.midiNote = child.getProperty ("midiNote");
            piece.gain = static_cast<float> (child.getProperty ("gain"));
            piece.pan = static_cast<float> (child.getProperty ("pan"));
            piece.tuning = static_cast<float> (child.getProperty ("tuning"));
            piece.enabled = child.getProperty ("enabled");
            pieces.add (piece);
        }
    }

    if (pieces.size() > 0)
        kit.setPieces (pieces);

    return kit;
}

bool DrumKitDefinition::saveToFile (const juce::File& file) const
{
    const auto parent = file.getParentDirectory();
    if (!parent.exists())
        parent.createDirectories();

    const auto tree = toValueTree();
    const auto xml = tree.createXml();
    if (xml == nullptr)
        return false;

    return file.replaceWithText (xml->toString());
}

DrumKitDefinition DrumKitDefinition::loadFromFile (const juce::File& file)
{
    if (!file.existsAsFile())
        return createDefault();

    const auto xml = juce::parseXML (file);
    if (xml == nullptr)
        return createDefault();

    const auto root = juce::ValueTree::fromXml (*xml);
    if (root.isValid())
        return fromValueTree (root);

    return createDefault();
}
