#pragma once

#include <map>
#include <memory>

#include "BackendBridge.h"
#include "RoomState.h"
#include "TransportClient.h"

namespace relay
{
class RelayServer
{
public:
    explicit RelayServer(std::shared_ptr<BackendBridge> backendBridgeToUse);

    bool registerClient(const TransportClientPtr& client, juce::String& error);
    void unregisterClient(const juce::String& clientId);
    bool handleClientPayload(const juce::String& clientId, const juce::String& payload, juce::String& error);
    std::vector<RoomSummary> getRoomSummaries() const;

private:
    void sendSnapshotToListener(TransportClient& listener, const RoomState& room) const;
    void broadcastEventToListeners(const RoomState& room, const MidiRelayEvent& event) const;
    TransportClient* findClient(const juce::String& clientId) const;

    std::shared_ptr<BackendBridge> backendBridge;
    std::map<juce::String, TransportClientPtr> clients;
    std::map<juce::String, RoomState> rooms;
};
}
