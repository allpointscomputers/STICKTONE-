#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_core/juce_core.h>

#include "Audio/DrumSynth.h"

int main()
{
    juce::AudioBuffer<float> buffer (2, 256);
    buffer.clear();

    DrumSynth synth;
    synth.prepareToPlay (44100.0, 256);
    synth.trigger (36, 0.9f);
    synth.render (buffer, juce::MidiBuffer(), 0, 256);

    if (buffer.getMagnitude (0, 0, 256) <= 0.0f)
        return 1;

    if (DrumSynth::getNameForMidi (36) != "Kick")
        return 2;

    if (DrumSynth::getNameForMidi (38) != "Snare")
        return 3;

    return 0;
}

