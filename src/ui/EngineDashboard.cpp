#include "EngineDashboard.h"

namespace
{
void configureInfoLabel(juce::Label& label)
{
    label.setJustificationType(juce::Justification::centredLeft);
    label.setFont(juce::FontOptions(16.0f));
}
}

EngineDashboard::EngineDashboard(MidiNoteState& midiNoteState)
    : pianoKeyboardView(midiNoteState)
{
    titleLabel.setText("MIDI Engine", juce::dontSendNotification);
    titleLabel.setJustificationType(juce::Justification::centredLeft);
    titleLabel.setFont(juce::FontOptions(28.0f));

    statusLabel.setJustificationType(juce::Justification::centredLeft);
    statusLabel.setFont(juce::FontOptions(15.0f));

    configureInfoLabel(audioTypeLabel);
    configureInfoLabel(deviceNameLabel);
    configureInfoLabel(sampleRateLabel);
    configureInfoLabel(bufferSizeLabel);
    configureInfoLabel(latencyLabel);
    configureInfoLabel(instrumentLabel);
    configureInfoLabel(midiLabel);

    instrumentSelector.onChange = [this]
    {
        if (onInstrumentSelected)
            onInstrumentSelected(instrumentSelector.getSelectedId());
    };

    addAndMakeVisible(titleLabel);
    addAndMakeVisible(audioTypeLabel);
    addAndMakeVisible(deviceNameLabel);
    addAndMakeVisible(sampleRateLabel);
    addAndMakeVisible(bufferSizeLabel);
    addAndMakeVisible(latencyLabel);
    addAndMakeVisible(instrumentLabel);
    addAndMakeVisible(instrumentSelector);
    addAndMakeVisible(midiLabel);
    addAndMakeVisible(statusLabel);
    addAndMakeVisible(pianoKeyboardView);
}

void EngineDashboard::setInstrumentOptions(const juce::String& primaryName, const juce::String& secondaryName)
{
    instrumentSelector.clear();
    instrumentSelector.addItem(primaryName, 1);
    instrumentSelector.addItem(secondaryName, 2);
}

void EngineDashboard::setSelectedInstrumentId(int selectedId)
{
    instrumentSelector.setSelectedId(selectedId, juce::dontSendNotification);
}

void EngineDashboard::setStatus(const AudioSession::Status& status, const juce::String& instrumentName)
{
    audioTypeLabel.setText("Audio type: " + status.audioDeviceType, juce::dontSendNotification);
    deviceNameLabel.setText("Audio device: " + status.audioDeviceName, juce::dontSendNotification);
    sampleRateLabel.setText("Sample rate: " + status.sampleRate, juce::dontSendNotification);
    bufferSizeLabel.setText("Buffer size: " + status.bufferSize, juce::dontSendNotification);
    latencyLabel.setText("Latency: " + status.latency, juce::dontSendNotification);
    instrumentLabel.setText("Instrument: " + instrumentName, juce::dontSendNotification);
    midiLabel.setText(status.midiInputName.isEmpty() ? "MIDI input: none" : "MIDI input: " + status.midiInputName,
                      juce::dontSendNotification);
    statusLabel.setText(status.message, juce::dontSendNotification);
}

void EngineDashboard::setInstrumentSelectedCallback(InstrumentSelectedCallback callback)
{
    onInstrumentSelected = std::move(callback);
}

void EngineDashboard::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff111315));

    g.setColour(juce::Colour(0xff1d2227));
    g.fillRoundedRectangle(getLocalBounds().toFloat().reduced(16.0f), 18.0f);

    g.setColour(juce::Colour(0xff2f3942));
    g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(16.0f), 18.0f, 1.5f);
}

void EngineDashboard::resized()
{
    auto area = getLocalBounds().reduced(32, 24);

    titleLabel.setBounds(area.removeFromTop(42));
    area.removeFromTop(12);

    const int rowHeight = 28;
    audioTypeLabel.setBounds(area.removeFromTop(rowHeight));
    deviceNameLabel.setBounds(area.removeFromTop(rowHeight));
    sampleRateLabel.setBounds(area.removeFromTop(rowHeight));
    bufferSizeLabel.setBounds(area.removeFromTop(rowHeight));
    latencyLabel.setBounds(area.removeFromTop(rowHeight));
    instrumentLabel.setBounds(area.removeFromTop(rowHeight));
    instrumentSelector.setBounds(area.removeFromTop(32).removeFromLeft(240));
    area.removeFromTop(4);
    midiLabel.setBounds(area.removeFromTop(rowHeight));

    area.removeFromTop(10);
    statusLabel.setBounds(area.removeFromTop(rowHeight));
    area.removeFromTop(14);
    pianoKeyboardView.setBounds(area);
}
