#pragma once

#include <array>

#include <juce_events/juce_events.h>

class MidiNoteState : private juce::AsyncUpdater,
                      public juce::ChangeBroadcaster
{
public:
    static constexpr int minMidiNote = 0;
    static constexpr int maxMidiNote = 127;

    void setNoteActive(int midiNoteNumber, bool isActive);
    bool isNoteActive(int midiNoteNumber) const;
    void clear();

private:
    void handleAsyncUpdate() override;

    mutable juce::CriticalSection stateLock;
    std::array<bool, 128> noteStates {};
};
