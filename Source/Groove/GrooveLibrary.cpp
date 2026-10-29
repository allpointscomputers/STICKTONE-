#include "Groove/GrooveLibrary.h"

juce::ValueTree GrooveMetadata::toValueTree() const
{
    juce::ValueTree tree ("groove");
    tree.setProperty ("id", id, nullptr);
    tree.setProperty ("name", name, nullptr);
    tree.setProperty ("category", category, nullptr);
    tree.setProperty ("genre", genre, nullptr);
    tree.setProperty ("bpm", bpm, nullptr);
    tree.setProperty ("beatsPerBar", beatsPerBar, nullptr);
    tree.setProperty ("subdivision", subdivision, nullptr);
    tree.setProperty ("swing", swing, nullptr);

    juce::ValueTree notesNode ("notes");
    for (int note : notes)
        notesNode.appendChild (juce::ValueTree ("note").withProperty ("value", note, nullptr), nullptr);
    tree.appendChild (notesNode, nullptr);

    return tree;
}

GrooveMetadata GrooveMetadata::fromValueTree (const juce::ValueTree& tree)
{
    GrooveMetadata groove;
    groove.id = tree.getProperty ("id").toString();
    groove.name = tree.getProperty ("name").toString();
    groove.category = tree.getProperty ("category").toString();
    groove.genre = tree.getProperty ("genre").toString();
    groove.bpm = static_cast<float> (tree.getProperty ("bpm"));
    groove.beatsPerBar = tree.getProperty ("beatsPerBar");
    groove.subdivision = tree.getProperty ("subdivision");
    groove.swing = tree.getProperty ("swing");

    if (tree.hasChildWithName ("notes"))
    {
        const auto notesNode = tree.getChildWithName ("notes");
        for (const auto& child : notesNode)
            groove.notes.add (child.getProperty ("value"));
    }

    return groove;
}

GrooveLibrary::GrooveLibrary() = default;

void GrooveLibrary::addGroove (const GrooveMetadata& groove)
{
    for (auto it = grooves_.begin(); it != grooves_.end(); ++it)
    {
        if ((*it).id == groove.id)
        {
            *it = groove;
            return;
        }
    }

    grooves_.add (groove);
}

void GrooveLibrary::removeGroove (const juce::String& grooveId)
{
    grooves_.removeIf ([&] (const GrooveMetadata& groove) { return groove.id == grooveId; });
}

juce::Array<GrooveMetadata> GrooveLibrary::getGrooves() const
{
    return grooves_;
}

juce::Array<GrooveMetadata> GrooveLibrary::searchByName (const juce::String& query) const
{
    if (query.isEmpty())
        return grooves_;

    juce::Array<GrooveMetadata> matches;
    const auto lower = query.toLowerCase();
    for (const auto& groove : grooves_)
    {
        if (groove.name.toLowerCase().contains (lower))
            matches.add (groove);
    }
    return matches;
}

juce::Array<GrooveMetadata> GrooveLibrary::getByCategory (const juce::String& genreOrCategory) const
{
    juce::Array<GrooveMetadata> matches;
    const auto lower = genreOrCategory.toLowerCase();
    for (const auto& groove : grooves_)
    {
        if (groove.category.toLowerCase().contains (lower) || groove.genre.toLowerCase().contains (lower))
            matches.add (groove);
    }
    return matches;
}

juce::ValueTree GrooveLibrary::toValueTree() const
{
    juce::ValueTree root ("groove-library");
    for (const auto& groove : grooves_)
        root.appendChild (groove.toValueTree(), nullptr);
    return root;
}

GrooveLibrary GrooveLibrary::fromValueTree (const juce::ValueTree& tree)
{
    GrooveLibrary library;
    for (const auto& child : tree)
    {
        if (child.hasType ("groove"))
            library.addGroove (GrooveMetadata::fromValueTree (child));
    }
    return library;
}

bool GrooveLibrary::saveToFile (const juce::File& file) const
{
    auto parent = file.getParentDirectory();
    if (!parent.exists())
        parent.createDirectories();

    const auto tree = toValueTree();
    const auto xml = tree.createXml();
    return xml != nullptr && file.replaceWithText (xml->toString());
}

GrooveLibrary GrooveLibrary::loadFromFile (const juce::File& file)
{
    if (!file.existsAsFile())
        return GrooveLibrary();

    const auto xml = juce::parseXML (file);
    if (xml == nullptr)
        return GrooveLibrary();

    const auto tree = juce::ValueTree::fromXml (*xml);
    if (!tree.isValid())
        return GrooveLibrary();

    return GrooveLibrary::fromValueTree (tree);
}

