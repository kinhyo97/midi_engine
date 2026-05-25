#include "MidiNoteState.h"

// 미디노트의 값을 바꾸는 함수
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

// 미디노트의 상태를 읽는 함수
bool MidiNoteState::isNoteActive(int midiNoteNumber) const
{
    if (!juce::isPositiveAndBelow(midiNoteNumber, static_cast<int>(noteStates.size())))
        return false;

    const juce::ScopedLock lock(stateLock);
    return noteStates[static_cast<size_t>(midiNoteNumber)];
}

// 미디노트의 상태를 전부 false로 변경
void MidiNoteState::clear()
{
    {
        const juce::ScopedLock lock(stateLock);
        noteStates.fill(false);
    }

    triggerAsyncUpdate();
}

// sendChangeMessage를 호출해서 나중에 불러주는 콜백
void MidiNoteState::handleAsyncUpdate()
{
    sendChangeMessage();
}
