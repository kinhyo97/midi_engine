#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "../session/AudioSession.h"
#include "PianoKeyboardView.h"

class EngineDashboard : public juce::Component
{
public:
    using InstrumentSelectedCallback = std::function<void(int selectedId)>;

    explicit EngineDashboard(MidiNoteState& midiNoteState);

    void setInstrumentOptions(const juce::String& primaryName, const juce::String& secondaryName);
    void setSelectedInstrumentId(int selectedId);
    void setStatus(const AudioSession::Status& status, const juce::String& instrumentName);
    void setInstrumentSelectedCallback(InstrumentSelectedCallback callback);

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    juce::Label titleLabel;
    juce::Label audioTypeLabel;
    juce::Label deviceNameLabel;
    juce::Label sampleRateLabel;
    juce::Label bufferSizeLabel;
    juce::Label latencyLabel;
    juce::Label instrumentLabel;
    juce::ComboBox instrumentSelector;
    juce::Label midiLabel;
    juce::Label statusLabel;
    PianoKeyboardView pianoKeyboardView;
    InstrumentSelectedCallback onInstrumentSelected;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EngineDashboard)
};
