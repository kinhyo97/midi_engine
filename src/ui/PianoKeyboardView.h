#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "../state/MidiNoteState.h"

class PianoKeyboardView : public juce::Component,
                          private juce::ChangeListener
{
public:
    explicit PianoKeyboardView(MidiNoteState& midiNoteState);
    ~PianoKeyboardView() override;

    void paint(juce::Graphics& g) override;

private:
    struct KeyLayout
    {
        int midiNoteNumber = 0;
        juce::Rectangle<float> bounds;
        bool isBlackKey = false;
    };

    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    bool isBlackKey(int midiNoteNumber) const;
    juce::String getNoteName(int midiNoteNumber) const;

    MidiNoteState& midiNoteState;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PianoKeyboardView)
};
