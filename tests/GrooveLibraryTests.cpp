#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>

#include "Groove/GrooveLibrary.h"
#include "Core/ProjectState.h"

int main()
{
    GrooveMetadata groove;
    groove.id = "demo-1";
    groove.name = "Basic Rock";
    groove.category = "Verse";
    groove.genre = "Rock";
    groove.bpm = 110.0f;
    groove.notes.add (36);
    groove.notes.add (38);
    groove.notes.add (42);

    GrooveLibrary library;
    library.addGroove (groove);

    if (library.searchByName ("Basic").size() != 1)
        return 1;

    if (library.getByCategory ("Rock").size() != 1)
        return 2;

    ProjectState state;
    state.setVersion (1);
    state.setKitName ("Wavey Beat Starter Kit");
    state.setCurrentPreset ("Factory Demo");
    state.setGrooveCount (1);

    if (state.getKitName() != "Wavey Beat Starter Kit")
        return 3;

    return 0;
}
