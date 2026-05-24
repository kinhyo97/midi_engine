#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "../instrument/PianoInstrument.h"
#include "../instrument/SynthInstrument.h"
#include "../session/AudioSession.h"
#include "../state/MidiNoteState.h"
#include "EngineDashboard.h"

class MainComponent : public juce::Component
{
public:
    enum InstrumentChoice
    {
        pianoChoice = 1,
        synthChoice = 2
    };

    MainComponent();
    ~MainComponent() override;

    void resized() override;

private:
    void updateStatusLabels();
    void changeInstrument(int selectedId);
    const Instrument& getCurrentInstrument() const;

    PianoInstrument pianoInstrument;
    SynthInstrument synthInstrument;
    MidiNoteState midiNoteState;
    AudioSession audioSession;
    EngineDashboard dashboard;
    int currentInstrumentId = pianoChoice;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
