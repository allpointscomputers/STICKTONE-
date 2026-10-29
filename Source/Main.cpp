#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "Audio/DrumSynth.h"

namespace
{
    class DrumAudioSource final : public juce::AudioSource
    {
    public:
        void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override
        {
            synth_.prepareToPlay (sampleRate, samplesPerBlockExpected);
        }

        void releaseResources() override {}

        void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override
        {
            juce::AudioBuffer<float> temp (2, bufferToFill.numSamples);
            temp.clear();
            synth_.render (temp, juce::MidiBuffer(), 0, bufferToFill.numSamples);

            for (int channel = 0; channel < temp.getNumChannels(); ++channel)
            {
                const auto* source = temp.getReadPointer (channel);
                auto* dest = bufferToFill.buffer->getWritePointer (channel, bufferToFill.startSample);
                for (int sample = 0; sample < bufferToFill.numSamples; ++sample)
                    dest[sample] = source[sample];
            }
        }

        void trigger (int note, float velocity)
        {
            synth_.trigger (note, velocity);
        }

    private:
        DrumSynth synth_;
    };

    class StickToneMainComponent final : public juce::Component
    {
    public:
        StickToneMainComponent()
        {
            setSize (560, 360);

            kickButton_.setButtonText ("Kick");
            kickButton_.onClick = [this] { triggerDrum (36, 0.9f); };
            addAndMakeVisible (kickButton_);

            snareButton_.setButtonText ("Snare");
            snareButton_.onClick = [this] { triggerDrum (38, 0.8f); };
            addAndMakeVisible (snareButton_);

            hatButton_.setButtonText ("Hi-Hat");
            hatButton_.onClick = [this] { triggerDrum (42, 0.7f); };
            addAndMakeVisible (hatButton_);

            masterSlider_.setRange (0.0, 1.5, 0.01);
            masterSlider_.setValue (0.8);
            masterSlider_.onValueChange = [this] { drumSource_.synth_.setMasterGain(static_cast<float>(masterSlider_.getValue())); };
            addAndMakeVisible (masterSlider_);

            deviceManager_ = std::make_unique<juce::AudioDeviceManager>();
            deviceManager_->initialiseWithDefaultDevices (2, 2);
            audioSourcePlayer_.setSource (&drumSource_);
            deviceManager_->addAudioCallback (&audioSourcePlayer_);
        }

        ~StickToneMainComponent() override
        {
            deviceManager_->removeAudioCallback (&audioSourcePlayer_);
            deviceManager_->closeAudioDevice();
        }

        void paint (juce::Graphics& g) override
        {
            g.fillAll (juce::Colours::black);
            g.setColour (juce::Colours::whitesmoke);
            g.setFont (24.0f);
            g.drawText ("STICKTONE BY WAVEY BEAT",
                        juce::Rectangle<float> (20.0f, 18.0f, 500.0f, 32.0f),
                        juce::Justification::left);
            g.setFont (14.0f);
            g.drawText ("Procedural fallback drum voices for the initial buildable slice.",
                        juce::Rectangle<float> (20.0f, 52.0f, 500.0f, 20.0f),
                        juce::Justification::left);
        }

        void resized() override
        {
            const auto buttonWidth = (getWidth() - 80) / 3;
            kickButton_.setBounds (20, 110, buttonWidth, 120);
            snareButton_.setBounds (30 + buttonWidth, 110, buttonWidth, 120);
            hatButton_.setBounds (40 + (buttonWidth * 2), 110, buttonWidth, 120);
            masterSlider_.setBounds (20, 250, getWidth() - 40, 30);
        }

    private:
        void triggerDrum (int midiNote, float velocity)
        {
            drumSource_.trigger (midiNote, velocity);
        }

        juce::TextButton kickButton_ { "Kick" };
        juce::TextButton snareButton_ { "Snare" };
        juce::TextButton hatButton_ { "Hi-Hat" };
        juce::Slider masterSlider_;
        DrumAudioSource drumSource_;
        juce::AudioSourcePlayer audioSourcePlayer_;
        std::unique_ptr<juce::AudioDeviceManager> deviceManager_;
    };
}

class StickToneApplication final : public juce::JUCEApplication
{
public:
    StickToneApplication() = default;

    const juce::String getName() override { return "STICKTONE BY WAVEY BEAT"; }
    const juce::String getApplicationName() override { return getName(); }
    bool moreThanOneInstanceAllowed() override { return true; }

    void initialise (const juce::String&) override
    {
        mainWindow_ = std::make_unique<juce::DocumentWindow> (
            "STICKTONE BY WAVEY BEAT",
            juce::Colours::black,
            juce::DocumentWindow::allButtons);

        mainWindow_->setUsingNativeTitleBar (true);
        mainWindow_->setContentOwned (new StickToneMainComponent(), true);
        mainWindow_->centreWithSize (560, 340);
        mainWindow_->setVisible (true);
    }

    void shutdown() override
    {
        mainWindow_ = nullptr;
    }

    void systemRequestedQuit() override
    {
        quit();
    }

    void anotherInstanceStarted (const juce::String&) override {}

private:
    std::unique_ptr<juce::DocumentWindow> mainWindow_;
};

START_JUCE_APPLICATION (StickToneApplication)
