#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>

#include "SampleLibrary/SampleLibrary.h"

namespace
{
    juce::String sampleManifestPath()
    {
        return "assets/default-sample-manifest.xml";
    }
}

class SampleLibraryDemo final : public juce::Component
{
public:
    SampleLibraryDemo()
    {
        setSize (420, 260);
        sampleLibrary_ = SampleLibrary::loadFromFile (juce::File (sampleManifestPath()));
        status_.setText (sampleLibrary_.getStatusSummary(), juce::dontSendNotification);
        addAndMakeVisible (status_);
    }

    void paint (juce::Graphics& g) override
    {
        g.fillAll (juce::Colours::black);
        g.setColour (juce::Colours::whitesmoke);
        g.setFont (20.0f);
        g.drawText ("Sample Library", juce::Rectangle<float> (20.0f, 20.0f, 220.0f, 30.0f), juce::Justification::left);
    }

    void resized() override
    {
        status_.setBounds (20, 70, getWidth() - 40, 30);
    }

private:
    SampleLibrary sampleLibrary_;
    juce::Label status_;
};
