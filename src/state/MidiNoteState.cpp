#include "MidiNoteState.h"

void MidiNoteState::setNoteActive(int midiNoteNumber, bool isActive)
{
    if (!juce::isPositiveAndBelow(midiNoteNumber, static_cast<int>(noteStates.size())))
        return;

    {
        const juce::ScopedLock lock(stateLock);

        if (noteStates[static_cast<size_t>(midiNoteNumber)] == isActive)
            return;

        noteStates[static_cast<size_t>(midiNoteNumber)] = isActive;
    }

    triggerAsyncUpdate();
}

bool MidiNoteState::isNoteActive(int midiNoteNumber) const
{
    if (!juce::isPositiveAndBelow(midiNoteNumber, static_cast<int>(noteStates.size())))
        return false;

    const juce::ScopedLock lock(stateLock);
    return noteStates[static_cast<size_t>(midiNoteNumber)];
}

void MidiNoteState::clear()
{
    {
        const juce::ScopedLock lock(stateLock);
        noteStates.fill(false);
    }

    triggerAsyncUpdate();
}

void MidiNoteState::handleAsyncUpdate()
{
    sendChangeMessage();
}
