#pragma once

#include "TransportClient.h"

namespace relay
{
class ConsoleClient final : public TransportClient
{
public:
    ConsoleClient(juce::String roomIdToUse, juce::String clientIdToUse, ClientRole roleToUse);

    const juce::String& getClientId() const override;
    ClientRole getRole() const override;
    const juce::String& getRoomId() const override;
    void sendText(const juce::String& payload) override;

private:
    juce::String roomId;
    juce::String clientId;
    ClientRole role = ClientRole::listener;
};
}
