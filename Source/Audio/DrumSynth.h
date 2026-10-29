#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_core/juce_core.h>
#include <vector>

class DrumSynth
{
public:
    enum class DrumType
    {
        kick,
        snare,
        closedHat,
        openHat,
        tom,
        crash,
        ride,
        percussion
    };

    struct Voice
    {
        DrumType type = DrumType::kick;
        double frequency = 60.0;
        double phase = 0.0;
        double amplitude = 0.0;
        double age = 0.0;
        bool active = false;
    };

    DrumSynth();

    void prepareToPlay (double sampleRate, int samplesPerBlock);
    void trigger (int midiNoteNumber, float velocity);
    void trigger (DrumType type, float velocity);
    void render (juce::AudioBuffer<float>& buffer,
                 const juce::MidiBuffer& midi,
                 int startSample,
                 int numSamples);

    void setMasterGain (float gain);
    [[nodiscard]] float getMasterGain() const noexcept;
    [[nodiscard]] static juce::String getNameForMidi (int midiNote);
    [[nodiscard]] static DrumType getDrumTypeForMidi (int midiNote);
    [[nodiscard]] static int getMidiNoteForDrumType (DrumType type);

private:
    double sampleRate_ = 44100.0;
    int samplesPerBlock_ = 512;
    float masterGain_ = 0.8f;
    std::vector<Voice> voices_;
};
