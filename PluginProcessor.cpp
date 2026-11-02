#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>

namespace
{
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
    {
        std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            "master", "Master", juce::NormalisableRange<float>(0.0f, 1.0f), 0.8f));
        return { params.begin(), params.end() };
    }
}

DrumForgeAudioProcessor::DrumForgeAudioProcessor()
    : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      parameters(*this, nullptr, "DRUMFORGE_STATE", createParameterLayout())
{
    for (auto& level : levels)
        level.store(0.8f);
}

void DrumForgeAudioProcessor::prepareToPlay(double sampleRate, int)
{
    currentSampleRate = sampleRate > 0.0 ? sampleRate : 44100.0;
}

void DrumForgeAudioProcessor::releaseResources() {}

bool DrumForgeAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    const auto out = layouts.getMainOutputChannelSet();
    return out == juce::AudioChannelSet::mono() || out == juce::AudioChannelSet::stereo();
}

int DrumForgeAudioProcessor::noteToIndex(int midiNote)
{
    for (int i = 0; i < kDrumCount; ++i)
        if (kMidiNotes[static_cast<size_t>(i)] == midiNote)
            return i;
    return -1;
}

void DrumForgeAudioProcessor::triggerDrum(int midiNote, float velocity)
{
    const int index = noteToIndex(midiNote);
    if (index >= 0)
    {
        // UI/MIDI callers publish a trigger; voice state is changed only by the audio thread.
        auto& pending = pendingTriggers[static_cast<size_t>(index)];
        pending.store(juce::jmax(pending.load(), juce::jlimit(0.0f, 1.0f, velocity)));
    }
}

void DrumForgeAudioProcessor::startVoice(int midiNote, float velocity)
{
    const int index = noteToIndex(midiNote);
    if (index < 0)
        return;

    Voice* target = nullptr;
    for (auto& voice : voices)
    {
        if (!voice.active)
        {
            target = &voice;
            break;
        }
    }
    if (target == nullptr)
    {
        target = &voices.front();
        for (auto& voice : voices)
            if (voice.age > target->age)
                target = &voice;
    }

    target->active = true;
    target->note = midiNote;
    target->velocity = juce::jlimit(0.0f, 1.0f, velocity);
    target->phase = 0.0;
    target->age = 0.0;
    target->noiseState = random.nextFloat() * 2.0 - 1.0;
    target->level = levels[static_cast<size_t>(index)].load();
}

void DrumForgeAudioProcessor::setDrumLevel(int midiNote, float level)
{
    const int index = noteToIndex(midiNote);
    if (index >= 0)
        levels[static_cast<size_t>(index)].store(juce::jlimit(0.0f, 1.0f, level));
}

float DrumForgeAudioProcessor::getDrumLevel(int midiNote) const
{
    const int index = noteToIndex(midiNote);
    return index >= 0 ? levels[static_cast<size_t>(index)].load() : 0.8f;
}

float DrumForgeAudioProcessor::renderVoice(Voice& v)
{
    if (!v.active)
        return 0.0f;

    const double t = v.age;
    double sample = 0.0;
    const double sr = currentSampleRate;

    if (v.note == 36) // Kick: pitch-dropping sine plus a short transient
    {
        const double freq = 48.0 + 115.0 * std::exp(-t * 30.0);
        v.phase += juce::MathConstants<double>::twoPi * freq / sr;
        const double env = std::exp(-t * 8.5);
        const double click = (t < 0.004) ? (1.0 - t / 0.004) * 0.15 : 0.0;
        sample = std::sin(v.phase) * env * 0.9 + click;
        if (t > 0.8) v.active = false;
    }
    else if (v.note == 38) // Snare: noise body plus tuned shell
    {
        v.noiseState = v.noiseState * 0.72 + (random.nextFloat() * 2.0 - 1.0) * 0.28;
        v.phase += juce::MathConstants<double>::twoPi * 185.0 / sr;
        sample = (v.noiseState * std::exp(-t * 17.0) * 0.8)
               + (std::sin(v.phase) * std::exp(-t * 24.0) * 0.25);
        if (t > 0.45) v.active = false;
    }
    else // Hats and crash: filtered-ish noise with different decay envelopes
    {
        v.noiseState = v.noiseState * 0.18 + (random.nextFloat() * 2.0 - 1.0) * 0.82;
        const double decay = v.note == 42 ? 70.0 : (v.note == 46 ? 12.0 : 3.8);
        sample = v.noiseState * std::exp(-t * decay) * 0.65;
        if (t > (v.note == 42 ? 0.18 : (v.note == 46 ? 1.2 : 2.5)))
            v.active = false;
    }

    v.age += 1.0 / sr;
    return static_cast<float>(sample) * v.velocity * v.level * 0.7f;
}

void DrumForgeAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();

    for (const auto metadata : midi)
    {
        const auto message = metadata.getMessage();
        if (message.isNoteOn())
            triggerDrum(message.getNoteNumber(), message.getFloatVelocity());
    }

    for (int i = 0; i < kDrumCount; ++i)
    {
        const float velocity = pendingTriggers[static_cast<size_t>(i)].exchange(0.0f);
        if (velocity > 0.0f)
            startVoice(kMidiNotes[static_cast<size_t>(i)], velocity);
    }

    const auto* masterParam = parameters.getRawParameterValue("master");
    const float master = masterParam != nullptr ? masterParam->load() : 0.8f;
    const int channels = buffer.getNumChannels();
    const int samples = buffer.getNumSamples();

    for (int i = 0; i < samples; ++i)
    {
        float mixed = 0.0f;
        for (auto& voice : voices)
            mixed += renderVoice(voice);
        mixed *= master;
        for (int ch = 0; ch < channels; ++ch)
            buffer.setSample(ch, i, mixed);
    }
}

juce::AudioProcessorEditor* DrumForgeAudioProcessor::createEditor()
{
    return new DrumForgeAudioProcessorEditor(*this);
}

void DrumForgeAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    for (int i = 0; i < kDrumCount; ++i)
        state.setProperty("level_" + juce::String(kMidiNotes[static_cast<size_t>(i)]),
                          levels[static_cast<size_t>(i)].load(), nullptr);
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void DrumForgeAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml != nullptr && xml->hasTagName(parameters.state.getType()))
    {
        parameters.replaceState(juce::ValueTree::fromXml(*xml));
        for (int i = 0; i < kDrumCount; ++i)
        {
            const auto key = "level_" + juce::String(kMidiNotes[static_cast<size_t>(i)]);
            const float level = static_cast<float>(parameters.state.getProperty(key, 0.8f));
            levels[static_cast<size_t>(i)].store(juce::jlimit(0.0f, 1.0f, level));
        }
    }
}
