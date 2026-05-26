#include "MessageCodec.h"

namespace relay
{
namespace
{
juce::String typeToString(MidiRelayEvent::Type type)
{
    return type == MidiRelayEvent::Type::noteOn ? "noteOn" : "noteOff";
}
}

// JSON payload를 파싱해서 relay 이벤트 필드로 변환한다.
bool MessageCodec::parseMidiEvent(const juce::String& payload, MidiRelayEvent& event, juce::String& error)
{
    const auto parsed = juce::JSON::parse(payload);
    auto* object = parsed.getDynamicObject();

    if (object == nullptr)
    {
        error = "payload must be a JSON object.";
        return false;
    }

    const auto type = object->getProperty("type").toString();

    if (type == "noteOn")
        event.type = MidiRelayEvent::Type::noteOn;
    else if (type == "noteOff")
        event.type = MidiRelayEvent::Type::noteOff;
    else
    {
        error = "type must be noteOn or noteOff.";
        return false;
    }

    event.sequence = static_cast<int>(object->getProperty("seq"));
    event.note = static_cast<int>(object->getProperty("note"));
    event.velocity = static_cast<float>(static_cast<double>(object->getProperty("velocity")));
    event.eventTime = static_cast<double>(object->getProperty("eventTime"));
    event.sourceName = object->getProperty("source").toString();

    error.clear();
    return true;
}

// listener는 room 정보까지 함께 받는 게 디버깅과 멀티룸 라우팅에 편하다.
juce::String MessageCodec::makeMidiEventEnvelope(const juce::String& roomId, const MidiRelayEvent& event)
{
    auto payload = juce::DynamicObject::Ptr(new juce::DynamicObject());
    payload->setProperty("messageType", "midiEvent");
    payload->setProperty("roomId", roomId);
    payload->setProperty("seq", event.sequence);
    payload->setProperty("type", typeToString(event.type));
    payload->setProperty("note", event.note);
    payload->setProperty("velocity", event.velocity);
    payload->setProperty("eventTime", event.eventTime);

    if (event.sourceName.isNotEmpty())
        payload->setProperty("source", event.sourceName);

    return juce::JSON::toString(juce::var(payload.get()), true);
}

// 현재 active note 목록을 배열로 내보내 listener가 초기 상태를 재구성할 수 있게 한다.
juce::String MessageCodec::makeSnapshotEnvelope(const juce::String& roomId, const std::vector<ActiveNoteInfo>& activeNotes)
{
    auto payload = juce::DynamicObject::Ptr(new juce::DynamicObject());
    payload->setProperty("messageType", "activeNotesSnapshot");
    payload->setProperty("roomId", roomId);

    juce::Array<juce::var> notes;

    for (const auto& activeNote : activeNotes)
    {
        auto note = juce::DynamicObject::Ptr(new juce::DynamicObject());
        note->setProperty("note", activeNote.note);
        note->setProperty("velocity", activeNote.velocity);
        note->setProperty("startedAt", activeNote.startedAt);
        notes.add(juce::var(note.get()));
    }

    payload->setProperty("notes", juce::var(notes));
    return juce::JSON::toString(juce::var(payload.get()), true);
}

// transport나 운영 로그에서 공통으로 쓸 수 있는 notice JSON을 만든다.
juce::String MessageCodec::makeNoticeEnvelope(const juce::String& type, const juce::String& message)
{
    auto payload = juce::DynamicObject::Ptr(new juce::DynamicObject());
    payload->setProperty("messageType", type);
    payload->setProperty("message", message);
    return juce::JSON::toString(juce::var(payload.get()), true);
}
}
