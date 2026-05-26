#include "EventValidator.h"

namespace relay
{
// note range, velocity range, timestamp, sequence를 검사해 비정상 payload를 걸러낸다.
bool EventValidator::validateMidiEvent(const MidiRelayEvent& event, juce::String& error)
{
    if (event.sequence <= 0)
    {
        error = "seq must be positive.";
        return false;
    }

    if (event.note < 0 || event.note > 127)
    {
        error = "note must be between 0 and 127.";
        return false;
    }

    if (event.velocity < 0.0f || event.velocity > 1.0f)
    {
        error = "velocity must be between 0.0 and 1.0.";
        return false;
    }

    if (event.eventTime < 0.0)
    {
        error = "eventTime must be non-negative.";
        return false;
    }

    error.clear();
    return true;
}
}
