#pragma once

#include "RelayTypes.h"

namespace relay
{
class EventValidator
{
public:
    // 서버가 받을 수 있는 최소 MIDI relay 규칙을 검사한다.
    static bool validateMidiEvent(const MidiRelayEvent& event, juce::String& error);
};
}
