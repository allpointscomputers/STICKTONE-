#include "Audio/DrumSynth.h"

namespace
{
    constexpr double pi = 3.14159265358979323846;

    DrumSynth::DrumType noteToType (int midiNote)
    {
        switch (midiNote)
        {
            case 35:
            case 36:
            case 37:
            case 41:
            case 43:
            case 44:
            case 45:
            case 47:
            case 50:
            case 52:
            case 53:
            case 54:
            case 55:
            case 56:
            case 58:
            case 59:
            case 60:
            case 61:
            case 62:
            case 63:
            case 64:
            case 65:
            case 67:
            case 69:
            case 71:
                return DrumSynth::DrumType::kick;
            case 38:
            case 40:
            case 48:
            case 51:
                return DrumSynth::DrumType::snare;
            case 42:
            case 46:
            case 70:
            case 72:
                return DrumSynth::DrumType::closedHat;
            case 39:
            case 57:
            case 68:
                return DrumSynth::DrumType::crash;
            case 49:
            case 66:
            case 73:
            case 74:
            case 75:
            case 76:
            case 77:
            case 78:
            case 79:
            case 80:
                return DrumSynth::DrumType::tom;
            case 81:
            case 82:
            case 83:
            case 84:
            case 85:
            case 86:
            case 87:
            case 88:
                return DrumSynth::DrumType::ride;
            default:
                return DrumSynth::DrumType::percussion;
        }
    }
}

DrumSynth::DrumSynth()
{
    voices_.resize (48);
}

void DrumSynth::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    sampleRate_ = sampleRate;
    samplesPerBlock_ = samplesPerBlock;
    for (auto& voice : voices_)
        voice.active = false;
}

void DrumSynth::trigger (int midiNoteNumber, float velocity)
{
    const auto safeVelocity = juce::jlimit (0.0f, 1.0f, velocity);
    const auto type = getDrumTypeForMidi (midiNoteNumber);
    trigger (type, safeVelocity);
}

void DrumSynth::trigger (DrumType type, float velocity)
{
    const auto safeVelocity = juce::jlimit (0.0f, 1.0f, velocity);

    for (auto& voice : voices_)
    {
        if (!voice.active)
        {
            voice.type = type;
            voice.frequency = juce::MidiMessage::getMidiNoteInHertz (getMidiNoteForDrumType (type));
            if (voice.frequency <= 0.0)
                voice.frequency = 100.0;
            voice.phase = 0.0;
            voice.amplitude = safeVelocity;
            voice.age = 0.0;
            voice.active = true;
            return;
        }
    }
}

void DrumSynth::render (juce::AudioBuffer<float>& buffer,
                       const juce::MidiBuffer& midi,
                       int startSample,
                       int numSamples)
{
    if (numSamples <= 0)
        return;

    juce::ignoreUnused (midi);

    for (auto& voice : voices_)
    {
        if (!voice.active)
            continue;

        for (int sample = 0; sample < numSamples; ++sample)
        {
            const double age = voice.age;
            const double env = std::max (0.0, 1.0 - (age / 0.35));
            double nextSample = 0.0;

            switch (voice.type)
            {
                case DrumType::kick:
                    nextSample = std::sin (voice.phase * 2.0 * pi * std::max (20.0, voice.frequency * (1.0 - 0.7 * env)));
                    nextSample *= std::max (0.0, 1.0 - (age / 0.28));
                    break;
                case DrumType::snare:
                    nextSample = (juce::Random::getSystemRandom().nextFloat() * 2.0f - 1.0f) * 0.85;
                    nextSample += 0.4 * std::sin (voice.phase * 2.0 * pi * 180.0);
                    nextSample *= std::max (0.0, 1.0 - (age / 0.13));
                    break;
                case DrumType::closedHat:
                    nextSample = (juce::Random::getSystemRandom().nextFloat() * 2.0f - 1.0f) * 1.25;
                    nextSample *= std::exp (-7.0 * age);
                    break;
                case DrumType::openHat:
                    nextSample = (juce::Random::getSystemRandom().nextFloat() * 2.0f - 1.0f) * 1.5;
                    nextSample *= std::exp (-3.5 * age);
                    break;
                case DrumType::tom:
                    nextSample = std::sin (voice.phase * 2.0 * pi * voice.frequency);
                    nextSample *= std::max (0.0, 1.0 - (age / 0.18));
                    break;
                case DrumType::crash:
                    nextSample = (juce::Random::getSystemRandom().nextFloat() * 2.0f - 1.0f) * 1.6;
                    nextSample *= std::exp (-2.2 * age);
                    break;
                case DrumType::ride:
                    nextSample = std::sin (voice.phase * 2.0 * pi * voice.frequency * 0.7);
                    nextSample *= std::exp (-4.0 * age);
                    break;
                case DrumType::percussion:
                    nextSample = std::sin (voice.phase * 2.0 * pi * voice.frequency * 0.5);
                    nextSample *= std::max (0.0, 1.0 - (age / 0.22));
                    break;
            }

            const auto gain = static_cast<float> (voice.amplitude * env * masterGain_ * 0.6f);
            const auto index = startSample + sample;
            for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
                buffer.addSample (channel, index, static_cast<float> (nextSample * gain));

            voice.phase += 1.0;
            voice.age += 1.0 / sampleRate_;

            if (voice.age > 0.38)
                voice.active = false;
        }
    }
}

void DrumSynth::setMasterGain (float gain)
{
    masterGain_ = juce::jlimit (0.0f, 2.0f, gain);
}

float DrumSynth::getMasterGain() const noexcept
{
    return masterGain_;
}

juce::String DrumSynth::getNameForMidi (int midiNote)
{
    switch (getDrumTypeForMidi (midiNote))
    {
        case DrumType::kick: return "Kick";
        case DrumType::snare: return "Snare";
        case DrumType::closedHat: return "Closed Hat";
        case DrumType::openHat: return "Open Hat";
        case DrumType::tom: return "Tom";
        case DrumType::crash: return "Crash";
        case DrumType::ride: return "Ride";
        case DrumType::percussion: return "Percussion";
    }

    return "Drum";
}

DrumSynth::DrumType DrumSynth::getDrumTypeForMidi (int midiNote)
{
    switch (midiNote)
    {
        case 35:
        case 36:
        case 37:
        case 41:
        case 43:
        case 44:
        case 45:
        case 47:
        case 50:
        case 52:
        case 53:
        case 54:
        case 55:
        case 56:
        case 58:
        case 59:
        case 60:
        case 61:
        case 62:
        case 63:
        case 64:
        case 65:
        case 67:
        case 69:
        case 71:
            return DrumType::kick;
        case 38:
        case 40:
        case 48:
        case 51:
            return DrumType::snare;
        case 42:
        case 46:
        case 70:
        case 72:
            return DrumType::closedHat;
        case 39:
        case 57:
        case 68:
            return DrumType::crash;
        case 49:
        case 66:
        case 73:
        case 74:
        case 75:
        case 76:
        case 77:
        case 78:
        case 79:
        case 80:
            return DrumType::tom;
        case 81:
        case 82:
        case 83:
        case 84:
        case 85:
        case 86:
        case 87:
        case 88:
            return DrumType::ride;
        default:
            return DrumType::percussion;
    }
}

int DrumSynth::getMidiNoteForDrumType (DrumType type)
{
    switch (type)
    {
        case DrumType::kick: return 36;
        case DrumType::snare: return 38;
        case DrumType::closedHat: return 42;
        case DrumType::openHat: return 46;
        case DrumType::tom: return 48;
        case DrumType::crash: return 49;
        case DrumType::ride: return 51;
        case DrumType::percussion: return 53;
    }

    return 36;
}
