#include "RoomState.h"

namespace relay
{
RoomState::RoomState(juce::String roomIdToUse)
    : roomId(std::move(roomIdToUse))
{
}

// room당 performer는 한 명으로 가정하고 현재 주체를 기록한다.
void RoomState::setPerformer(const juce::String& clientId)
{
    performerId = clientId;
}

// performer가 떠나면 남아 있던 active state도 같이 비워 세션을 초기화한다.
void RoomState::clearPerformer()
{
    performerId.clear();
    activeNotes.clear();
    lastSequence = 0;
}

// listener는 snapshot과 브로드캐스트 대상으로 유지한다.
void RoomState::addListener(const juce::String& clientId)
{
    if (!listenerIds.contains(clientId))
        listenerIds.add(clientId);
}

// 연결이 종료된 listener를 room 라우팅 대상에서 제거한다.
void RoomState::removeListener(const juce::String& clientId)
{
    listenerIds.removeString(clientId);
}

// 늦거나 역순으로 들어온 seq는 초기에 바로 거절해서 상태 꼬임을 줄인다.
bool RoomState::acceptsSequence(int sequence, juce::String& error) const
{
    if (sequence <= lastSequence)
    {
        error = "sequence must be greater than the last relayed sequence.";
        return false;
    }

    error.clear();
    return true;
}

// noteOn/noteOff를 active note 상태에 반영하고 마지막 sequence를 갱신한다.
void RoomState::applyMidiEvent(const MidiRelayEvent& event)
{
    lastSequence = event.sequence;

    if (event.type == MidiRelayEvent::Type::noteOn)
    {
        activeNotes[event.note] = ActiveNoteInfo { event.note, event.velocity, event.eventTime };
        return;
    }

    activeNotes.erase(event.note);
}

// 상위 서버가 room 라우팅 키로 사용하는 식별자를 반환한다.
const juce::String& RoomState::getRoomId() const
{
    return roomId;
}

// 현재 performer id는 backend bridge와 운영 로그에서 사용한다.
const juce::String& RoomState::getPerformerId() const
{
    return performerId;
}

// 브로드캐스트 대상 listener 목록을 상위 서버가 순회할 수 있게 노출한다.
const juce::StringArray& RoomState::getListenerIds() const
{
    return listenerIds;
}

// late joiner snapshot을 만들기 위해 현재 active note들을 벡터로 반환한다.
std::vector<ActiveNoteInfo> RoomState::getActiveNotes() const
{
    std::vector<ActiveNoteInfo> notes;
    notes.reserve(activeNotes.size());

    for (const auto& [note, info] : activeNotes)
        notes.push_back(info);

    return notes;
}

// performer와 listener가 모두 없어졌는지 판단해 room 수명을 정리한다.
bool RoomState::isEmpty() const
{
    return performerId.isEmpty() && listenerIds.isEmpty();
}

// 운영 상태를 콘솔에서 빠르게 확인할 수 있는 room 요약본을 만든다.
RoomSummary RoomState::getSummary() const
{
    return RoomSummary { roomId, performerId, listenerIds.size(), (int) activeNotes.size(), lastSequence };
}
}
