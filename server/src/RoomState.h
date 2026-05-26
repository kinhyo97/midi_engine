#pragma once

#include <map>
#include <vector>

#include "RelayTypes.h"

namespace relay
{
class RoomState
{
public:
    explicit RoomState(juce::String roomIdToUse = {});

    void setPerformer(const juce::String& clientId);
    void clearPerformer();
    void addListener(const juce::String& clientId);
    void removeListener(const juce::String& clientId);
    bool acceptsSequence(int sequence, juce::String& error) const;
    void applyMidiEvent(const MidiRelayEvent& event);

    const juce::String& getRoomId() const;
    const juce::String& getPerformerId() const;
    const juce::StringArray& getListenerIds() const;
    std::vector<ActiveNoteInfo> getActiveNotes() const;
    bool isEmpty() const;
    RoomSummary getSummary() const;

private:
    juce::String roomId;
    juce::String performerId;
    juce::StringArray listenerIds;
    std::map<int, ActiveNoteInfo> activeNotes;
    int lastSequence = 0;
};
}
