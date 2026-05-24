#include "Note.h"

namespace note
{
juce::String getSampleFileNameForMidiNote(int midiNoteNumber, const juce::String& extension)
{
    static const juce::String noteNames[] =
    {
        "C", "Cs", "D", "Ds", "E", "F", "Fs", "G", "Gs", "A", "As", "B"
    };

    const auto noteName = noteNames[midiNoteNumber % 12];
    const auto octave = (midiNoteNumber / 12) - 1;
    return noteName + juce::String(octave) + extension;
}
}
