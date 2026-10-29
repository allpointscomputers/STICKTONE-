#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_core/juce_core.h>

#include "SampleLibrary/SampleLibrary.h"

int main()
{
    SampleLibrary library;

    SampleMetadata hit;
    hit.id = "kick-01";
    hit.name = "Kick 01";
    hit.drumPieceId = "kick";
    hit.filePath = "/samples/kick-01.wav";
    hit.velocityLayer = 1;
    hit.roundRobinIndex = 1;
    hit.sampleRate = 44100;
    hit.bitDepth = 16;
    hit.status = SampleMetadata::SampleStatus::valid;

    library.addSample (hit);
    if (library.getSamplesForPiece ("kick").size() != 1)
        return 1;

    if (library.getStatusSummary().contains ("valid=1") == false)
        return 2;

    return 0;
}

