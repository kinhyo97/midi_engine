#pragma once

#include "BackendBridge.h"

namespace relay
{
class LoggingBackendBridge final : public BackendBridge
{
public:
    void onRoomActivated(const juce::String& roomId, const juce::String& performerId) override;
    void onRoomDeactivated(const juce::String& roomId) override;
    void onEventRelayed(const juce::String& roomId, const MidiRelayEvent& event) override;
};
}
