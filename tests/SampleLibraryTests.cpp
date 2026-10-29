#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_core/juce_core.h>

#include "Mixer/MixerState.h"
#include "Presets/PresetManager.h"

int main()
{
    MixerState mixer;
    mixer.setMasterGain (0.9f);

    ChannelStripSettings kick;
    kick.id = "kick";
    kick.name = "Kick";
    kick.gain = 1.2f;
    kick.pan = 0.1f;
    mixer.addChannel (kick);

    if (mixer.getMasterGain() < 0.8f)
        return 1;

    auto roundTrip = MixerState::fromValueTree (mixer.toValueTree());
    if (roundTrip.getChannels().size() < 1)
        return 2;

    PresetManager manager;
    PresetData demo;
    demo.id = "demo";
    demo.name = "Demo";
    demo.kitName = "Wavey Beat Starter Kit";
    demo.mixerState = mixer;
    manager.addPreset (demo);

    if (manager.getPresetById ("demo").name != "Demo")
        return 3;

    return 0;
}

