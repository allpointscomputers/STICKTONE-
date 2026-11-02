#pragma once
#include <JuceHeader.h>
#include <array>
#include <atomic>
#include <random>

class DrumForgeAudioProcessor final : public juce::AudioProcessor
{
public:
    DrumForgeAudioProcessor();
    ~DrumForgeAudioProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 3.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return "Default Kit"; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*, int) override;

    void triggerDrum(int midiNote, float velocity);
    void setDrumLevel(int midiNote, float level);
    float getDrumLevel(int midiNote) const;
    static constexpr int kDrumCount = 5;
    static constexpr std::array<int, kDrumCount> kMidiNotes { 36, 38, 42, 46, 49 };
    static constexpr std::array<const char*, kDrumCount> kDrumNames {
        "Kick", "Snare", "Closed Hat", "Open Hat", "Crash"
    };

private:
    struct Voice
    {
        bool active = false;
        int note = 36;
        float velocity = 0.8f;
        double phase = 0.0;
        double age = 0.0;
        double noiseState = 0.0;
        float level = 1.0f;
    };

    static int noteToIndex(int midiNote);
    float renderVoice(Voice& voice);
    void startVoice(int midiNote, float velocity);
    std::array<std::atomic<float>, kDrumCount> pendingTriggers {};
    std::array<Voice, 32> voices {};
    std::array<std::atomic<float>, kDrumCount> levels;
    double currentSampleRate = 44100.0;
    juce::AudioProcessorValueTreeState parameters;
    juce::Random random;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DrumForgeAudioProcessor)
};
