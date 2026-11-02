#include "PluginEditor.h"

DrumForgeAudioProcessorEditor::DrumForgeAudioProcessorEditor(DrumForgeAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    setSize(820, 570);
    title.setText("STICKTONE", juce::dontSendNotification);
    title.setFont(juce::Font(30.0f, juce::Font::bold));
    title.setColour(juce::Label::textColourId, juce::Colour(0xfff2f3f5));
    addAndMakeVisible(title);

    subtitle.setText("BY WAVEY BEAT  •  CROSS-PLATFORM DRUM INSTRUMENT  •  PROTOTYPE 0.1", juce::dontSendNotification);
    subtitle.setFont(juce::Font(12.0f));
    subtitle.setColour(juce::Label::textColourId, juce::Colour(0xffa8b0bd));
    addAndMakeVisible(subtitle);

    for (int i = 0; i < DrumForgeAudioProcessor::kDrumCount; ++i)
    {
        const auto index = static_cast<size_t>(i);
        drumButtons[index].setButtonText("HIT");
        drumButtons[index].onClick = [this, i]
        {
            processor.triggerDrum(DrumForgeAudioProcessor::kMidiNotes[static_cast<size_t>(i)], 0.9f);
            status.setText("Triggered " + juce::String(DrumForgeAudioProcessor::kDrumNames[static_cast<size_t>(i)]),
                           juce::dontSendNotification);
        };
        addAndMakeVisible(drumButtons[index]);

        drumLabels[index].setText(DrumForgeAudioProcessor::kDrumNames[index], juce::dontSendNotification);
        drumLabels[index].setJustificationType(juce::Justification::centred);
        drumLabels[index].setColour(juce::Label::textColourId, juce::Colour(0xffd9dee7));
        addAndMakeVisible(drumLabels[index]);

        levelSliders[index].setSliderStyle(juce::Slider::LinearHorizontal);
        levelSliders[index].setRange(0.0, 1.0, 0.01);
        levelSliders[index].setValue(processor.getDrumLevel(DrumForgeAudioProcessor::kMidiNotes[index]));
        levelSliders[index].setTextBoxStyle(juce::Slider::TextBoxRight, false, 48, 22);
        levelSliders[index].onValueChange = [this, i]
        {
            processor.setDrumLevel(DrumForgeAudioProcessor::kMidiNotes[static_cast<size_t>(i)],
                                   static_cast<float>(levelSliders[static_cast<size_t>(i)].getValue()));
        };
        addAndMakeVisible(levelSliders[index]);
    }

    grooveButton.onClick = [this] { groovePlaying ? stopGroove() : startGroove(); };
    stopButton.onClick = [this] { stopGroove(); };
    addAndMakeVisible(grooveButton);
    addAndMakeVisible(stopButton);

    tempoLabel.setText("TEMPO", juce::dontSendNotification);
    tempoLabel.setColour(juce::Label::textColourId, juce::Colour(0xffcbd2dc));
    addAndMakeVisible(tempoLabel);
    tempoSlider.setRange(50.0, 220.0, 1.0);
    tempoSlider.setValue(110.0);
    tempoSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 54, 24);
    addAndMakeVisible(tempoSlider);

    for (auto* button : { &exportButton, &saveButton, &loadButton, &tapButton, &songButton, &mixerButton })
    {
        button->setEnabled(false);
        addAndMakeVisible(*button);
    }

    status.setText("Ready • MIDI notes: kick 36, snare 38, closed hat 42, open hat 46, crash 49",
                   juce::dontSendNotification);
    status.setColour(juce::Label::textColourId, juce::Colour(0xffa8b0bd));
    addAndMakeVisible(status);
}

void DrumForgeAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff10141b));
    g.setColour(juce::Colour(0xff1a202b));
    g.fillRoundedRectangle(20.0f, 104.0f, static_cast<float>(getWidth() - 40), 230.0f, 14.0f);
    g.setColour(juce::Colour(0xff303a49));
    g.drawRoundedRectangle(20.0f, 104.0f, static_cast<float>(getWidth() - 40), 230.0f, 14.0f, 1.0f);
    g.setColour(juce::Colour(0xffa8b0bd));
    g.setFont(12.0f);
    g.drawText("KIT PIECES  /  CLICK TO AUDITION", 36, 116, 360, 20, juce::Justification::left);
    g.drawText("CHANNEL LEVELS", 36, 354, 180, 20, juce::Justification::left);
    g.setColour(juce::Colour(0xff252d39));
    g.fillRoundedRectangle(20.0f, 390.0f, static_cast<float>(getWidth() - 40), 146.0f, 14.0f);
}

void DrumForgeAudioProcessorEditor::resized()
{
    title.setBounds(28, 18, 300, 42);
    subtitle.setBounds(30, 60, 430, 24);

    const int left = 38;
    const int gap = 12;
    const int colWidth = (getWidth() - 2 * left - 4 * gap) / 5;
    for (int i = 0; i < DrumForgeAudioProcessor::kDrumCount; ++i)
    {
        const int x = left + i * (colWidth + gap);
        drumLabels[static_cast<size_t>(i)].setBounds(x, 148, colWidth, 28);
        drumButtons[static_cast<size_t>(i)].setBounds(x, 184, colWidth, 88);
        levelSliders[static_cast<size_t>(i)].setBounds(180, 354 + i * 31, getWidth() - 220, 26);
    }

    tempoLabel.setBounds(36, 408, 70, 24);
    tempoSlider.setBounds(100, 405, 150, 28);
    grooveButton.setBounds(270, 405, 170, 30);
    stopButton.setBounds(448, 405, 90, 30);
    exportButton.setBounds(550, 405, 230, 30);
    tapButton.setBounds(36, 447, 220, 30);
    songButton.setBounds(270, 447, 220, 30);
    mixerButton.setBounds(505, 447, 190, 30);
    saveButton.setBounds(36, 485, 120, 28);
    loadButton.setBounds(165, 485, 120, 28);
    status.setBounds(30, 540, getWidth() - 60, 22);
}

void DrumForgeAudioProcessorEditor::startGroove()
{
    groovePlaying = true;
    grooveStep = 0;
    timerTicks = 0;
    grooveButton.setButtonText("Groove playing…");
    const auto bpm = tempoSlider.getValue();
    startTimerHz(100);
    status.setText("Playing built-in prototype groove at " + juce::String(static_cast<int>(bpm)) + " BPM",
                   juce::dontSendNotification);
}

void DrumForgeAudioProcessorEditor::stopGroove()
{
    groovePlaying = false;
    stopTimer();
    grooveButton.setButtonText("Play 1-bar groove");
    status.setText("Stopped", juce::dontSendNotification);
}

void DrumForgeAudioProcessorEditor::timerCallback()
{
    if (!groovePlaying)
        return;

    ++timerTicks;
    const int ticksPerSixteenth = juce::jmax(1, static_cast<int>(1500.0 / tempoSlider.getValue()));
    if (timerTicks < ticksPerSixteenth)
        return;

    timerTicks = 0;
    const int step = grooveStep % 16;
    if (step == 0 || step == 8)
        processor.triggerDrum(36, 0.95f);
    if (step == 4 || step == 12)
        processor.triggerDrum(38, 0.82f);
    if (step % 2 == 0)
        processor.triggerDrum(42, step % 4 == 0 ? 0.65f : 0.38f);
    grooveStep = (grooveStep + 1) % 16;
}
