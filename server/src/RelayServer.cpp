#include "RelayServer.h"

#include "EventValidator.h"
#include "MessageCodec.h"

namespace relay
{
RelayServer::RelayServer(std::shared_ptr<BackendBridge> backendBridgeToUse)
    : backendBridge(std::move(backendBridgeToUse))
{
}

// 새 performer/listener를 room에 등록하고 listener라면 즉시 snapshot을 보낸다.
bool RelayServer::registerClient(const TransportClientPtr& client, juce::String& error)
{
    if (client == nullptr)
    {
        error = "client must not be null.";
        return false;
    }

    if (client->getClientId().isEmpty() || client->getRoomId().isEmpty())
    {
        error = "clientId and roomId must not be empty.";
        return false;
    }

    if (clients.count(client->getClientId()) != 0)
    {
        error = "clientId already exists.";
        return false;
    }

    auto [roomIt, inserted] = rooms.emplace(client->getRoomId(), RoomState(client->getRoomId()));
    auto& room = roomIt->second;
    juce::ignoreUnused(inserted);
    clients[client->getClientId()] = client;

    if (client->getRole() == ClientRole::performer)
    {
        if (room.getPerformerId().isNotEmpty())
        {
            clients.erase(client->getClientId());
            error = "room already has a performer.";
            return false;
        }

        room.setPerformer(client->getClientId());

        if (backendBridge != nullptr)
            backendBridge->onRoomActivated(room.getRoomId(), client->getClientId());
    }
    else
    {
        room.addListener(client->getClientId());
        sendSnapshotToListener(*client, room);
    }

    error.clear();
    return true;
}

// 연결이 끊긴 클라이언트를 room과 전체 클라이언트 목록에서 함께 제거한다.
void RelayServer::unregisterClient(const juce::String& clientId)
{
    auto clientIt = clients.find(clientId);

    if (clientIt == clients.end())
        return;

    const auto roomId = clientIt->second->getRoomId();
    auto roomIt = rooms.find(roomId);

    if (roomIt != rooms.end())
    {
        auto& room = roomIt->second;

        if (clientIt->second->getRole() == ClientRole::performer)
        {
            room.clearPerformer();

            if (backendBridge != nullptr)
                backendBridge->onRoomDeactivated(roomId);
        }
        else
        {
            room.removeListener(clientId);
        }

        if (room.isEmpty())
            rooms.erase(roomIt);
    }

    clients.erase(clientIt);
}

// performer payload를 파싱하고 검증한 뒤 room 상태 갱신과 listener 브로드캐스트를 수행한다.
bool RelayServer::handleClientPayload(const juce::String& clientId, const juce::String& payload, juce::String& error)
{
    auto* client = findClient(clientId);

    if (client == nullptr)
    {
        error = "unknown clientId.";
        return false;
    }

    if (client->getRole() != ClientRole::performer)
    {
        error = "only performer clients can relay MIDI events.";
        return false;
    }

    auto roomIt = rooms.find(client->getRoomId());

    if (roomIt == rooms.end())
    {
        error = "room not found.";
        return false;
    }

    MidiRelayEvent event;

    if (!MessageCodec::parseMidiEvent(payload, event, error))
        return false;

    if (!EventValidator::validateMidiEvent(event, error))
        return false;

    if (!roomIt->second.acceptsSequence(event.sequence, error))
        return false;

    roomIt->second.applyMidiEvent(event);
    broadcastEventToListeners(roomIt->second, event);

    if (backendBridge != nullptr)
        backendBridge->onEventRelayed(roomIt->second.getRoomId(), event);

    error.clear();
    return true;
}

// 운영상 room 상태를 콘솔 명령으로 확인할 수 있게 요약 벡터를 만든다.
std::vector<RoomSummary> RelayServer::getRoomSummaries() const
{
    std::vector<RoomSummary> summaries;
    summaries.reserve(rooms.size());

    for (const auto& [roomId, room] : rooms)
        summaries.push_back(room.getSummary());

    return summaries;
}

// listener는 접속 즉시 현재 active note 상태를 받아야 noteOff 누락을 피할 수 있다.
void RelayServer::sendSnapshotToListener(TransportClient& listener, const RoomState& room) const
{
    listener.sendText(MessageCodec::makeSnapshotEnvelope(room.getRoomId(), room.getActiveNotes()));
}

// performer를 제외한 같은 room의 listener 전원에게 동일한 JSON event를 보낸다.
void RelayServer::broadcastEventToListeners(const RoomState& room, const MidiRelayEvent& event) const
{
    const auto payload = MessageCodec::makeMidiEventEnvelope(room.getRoomId(), event);

    for (const auto& listenerId : room.getListenerIds())
    {
        auto* listener = findClient(listenerId);

        if (listener != nullptr)
            listener->sendText(payload);
    }
}

// client id로 transport 객체를 찾는 내부 헬퍼다.
TransportClient* RelayServer::findClient(const juce::String& clientId) const
{
    auto it = clients.find(clientId);
    return it != clients.end() ? it->second.get() : nullptr;
}
}
