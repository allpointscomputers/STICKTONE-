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

        void trigger (DrumSynth::DrumType type, float velocity)
        {
            synth_.trigger (type, velocity);
        }

        void setMasterGain (float gain)
        {
            synth_.setMasterGain (gain);
        }

        [[nodiscard]] float getMasterGain() const noexcept
        {
            return synth_.getMasterGain();
        }

    private:
        DrumSynth synth_;
    };

    class StickToneMainComponent final : public juce::Component
    {
    public:
        StickToneMainComponent()
        {
            setSize (620, 420);

            const auto addButton = [this] (juce::TextButton& button, const juce::String& label, int note, float velocity)
            {
                button.setButtonText (label);
                button.onClick = [this, note, velocity] { triggerDrum (note, velocity); };
                addAndMakeVisible (button);
            };

            addButton (kickButton_, "Kick", 36, 0.92f);
            addButton (snareButton_, "Snare", 38, 0.88f);
            addButton (closedHatButton_, "Closed Hat", 42, 0.68f);
            addButton (openHatButton_, "Open Hat", 46, 0.75f);
            addButton (tomButton_, "Tom", 48, 0.78f);
            addButton (crashButton_, "Crash", 49, 0.82f);
            addButton (rideButton_, "Ride", 51, 0.72f);

            masterSlider_.setRange (0.0, 1.5, 0.01);
            masterSlider_.setValue (0.8);
            masterSlider_.onValueChange = [this]
            {
                drumSource_.setMasterGain (static_cast<float> (masterSlider_.getValue()));
            };
            addAndMakeVisible (masterSlider_);

            gainLabel_.setText ("Master Gain", juce::dontSendNotification);
            addAndMakeVisible (gainLabel_);

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
            g.setFont (28.0f);
            g.drawText ("STICKTONE BY WAVEY BEAT",
                        juce::Rectangle<float> (20.0f, 16.0f, 560.0f, 34.0f),
                        juce::Justification::left);
            g.setFont (14.0f);
            g.drawText ("Procedural fallback voices • buildable studio foundation",
                        juce::Rectangle<float> (20.0f, 52.0f, 550.0f, 22.0f),
                        juce::Justification::left);
        }

        void resized() override
        {
            const auto colCount = 4;
            const auto buttonWidth = (getWidth() - 80) / colCount;
            const auto buttonHeight = 90;

            kickButton_.setBounds (20, 100, buttonWidth, buttonHeight);
            snareButton_.setBounds (30 + buttonWidth, 100, buttonWidth, buttonHeight);
            closedHatButton_.setBounds (40 + (buttonWidth * 2), 100, buttonWidth, buttonHeight);
            openHatButton_.setBounds (50 + (buttonWidth * 3), 100, buttonWidth, buttonHeight);
            tomButton_.setBounds (20, 210, buttonWidth, buttonHeight);
            crashButton_.setBounds (30 + buttonWidth, 210, buttonWidth, buttonHeight);
            rideButton_.setBounds (40 + (buttonWidth * 2), 210, buttonWidth, buttonHeight);

            gainLabel_.setBounds (20, 320, 120, 24);
            masterSlider_.setBounds (140, 318, getWidth() - 180, 28);
        }

    private:
        void triggerDrum (int midiNote, float velocity)
        {
            drumSource_.trigger (midiNote, velocity);
        }

        juce::TextButton kickButton_ { "Kick" };
        juce::TextButton snareButton_ { "Snare" };
        juce::TextButton closedHatButton_ { "Closed Hat" };
        juce::TextButton openHatButton_ { "Open Hat" };
        juce::TextButton tomButton_ { "Tom" };
        juce::TextButton crashButton_ { "Crash" };
        juce::TextButton rideButton_ { "Ride" };
        juce::Label gainLabel_;
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
        mainWindow_->centreWithSize (620, 420);
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
