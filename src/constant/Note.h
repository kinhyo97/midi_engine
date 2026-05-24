#pragma once

#include <juce_core/juce_core.h>

namespace note
{
constexpr int piano88Start = 21;
constexpr int piano88End = 108;

juce::String getSampleFileNameForMidiNote(int midiNoteNumber, const juce::String& extension);
}
