#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "../session/AudioSession.h"
#include "PianoKeyboardView.h"

class EngineDashboard : public juce::Component
{
public:
    // 선택된 악기에 대한 정보를 불러옴
    using InstrumentSelectedCallback = std::function<void(int selectedId)>;

    // 대시보드에 미디노트 정보를 전달
    explicit EngineDashboard(MidiNoteState& midiNoteState);

    // 악기옵션 지정
    void setInstrumentOptions(const juce::String& primaryName, const juce::String& secondaryName);
    // 선택된 악기의 id를 전달
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
