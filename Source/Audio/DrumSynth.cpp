#include "Audio/DrumSynth.h"

namespace
{
    constexpr double pi = 3.14159265358979323846;

    DrumSynth::DrumType noteToType (int midiNote)
    {
        switch (midiNote % 12)
        {
            case 0:
            case 1:
            case 2:
            case 3:
                return DrumSynth::DrumType::kick;
            case 4:
            case 5:
            case 6:
                return DrumSynth::DrumType::snare;
            case 7:
            case 8:
                return DrumSynth::DrumType::hat;
            case 9:
            case 10:
                return DrumSynth::DrumType::tom;
            default:
                return DrumSynth::DrumType::crash;
        }
    }
}

DrumSynth::DrumSynth()
{
    voices_.resize (32);
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

    for (auto& voice : voices_)
    {
        if (!voice.active)
        {
            voice.type = noteToType (midiNoteNumber);
            voice.frequency = juce::MidiMessage::getMidiNoteInHertz (juce::jlimit (0, 127, midiNoteNumber));
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

    for (auto& voice : voices_)
    {
        if (!voice.active)
            continue;

        for (int sample = 0; sample < numSamples; ++sample)
        {
            const double time = (startSample + sample) / sampleRate_;
            const double env = std::max (0.0, 1.0 - (voice.age / 0.35));
            double nextSample = 0.0;

            switch (voice.type)
            {
                case DrumType::kick:
                    nextSample = std::sin (voice.phase * 2.0 * pi * std::max (20.0, voice.frequency * (1.0 - 0.6 * env)));
                    nextSample *= std::max (0.0, 1.0 - (voice.age / 0.25));
                    break;
                case DrumType::snare:
                    nextSample = (juce::Random::getSystemRandom().nextFloat() * 2.0f - 1.0f) * 0.8;
                    nextSample += 0.35 * std::sin (voice.phase * 2.0 * pi * 180.0);
                    nextSample *= std::max (0.0, 1.0 - (voice.age / 0.12));
                    break;
                case DrumType::hat:
                    nextSample = (juce::Random::getSystemRandom().nextFloat() * 2.0f - 1.0f) * 1.1;
                    nextSample *= std::exp (-6.0 * voice.age);
                    break;
                case DrumType::tom:
                    nextSample = std::sin (voice.phase * 2.0 * pi * voice.frequency);
                    nextSample *= std::max (0.0, 1.0 - (voice.age / 0.2));
                    break;
                case DrumType::crash:
                    nextSample = (juce::Random::getSystemRandom().nextFloat() * 2.0f - 1.0f) * 1.4;
                    nextSample *= std::exp (-2.5 * voice.age);
                    break;
            }

            const auto gain = static_cast<float> (voice.amplitude * env * masterGain_ * 0.5f);
            for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
                buffer.addSample (channel, startSample + sample, static_cast<float> (nextSample * gain));

            voice.phase += 1.0;
            voice.age += 1.0 / sampleRate_;

            if (voice.age > 0.35)
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
    switch (midiNote % 12)
    {
        case 0: return "Kick";
        case 4: return "Snare";
        case 7: return "Closed Hat";
        case 9: return "Tom";
        case 11: return "Crash";
        default: return "Drum";
    }
}
