#pragma once

#include <vector>

#include <juce_core/juce_core.h>

namespace relay
{
enum class ClientRole
{
    performer,
    listener
};

struct MidiRelayEvent
{
    enum class Type
    {
        noteOn,
        noteOff
    };

    Type type = Type::noteOn;
    int note = 0;
    float velocity = 0.0f;
    double eventTime = 0.0;
    int sequence = 0;
    juce::String sourceName;

    // 로그와 JSON envelope에서 공통으로 쓰는 타입 이름을 반환한다.
    juce::String getTypeName() const
    {
        return type == Type::noteOn ? "noteOn" : "noteOff";
    }
};

struct ActiveNoteInfo
{
    int note = 0;
    float velocity = 0.0f;
    double startedAt = 0.0;
};

struct RoomSummary
{
    juce::String roomId;
    juce::String performerId;
    int listenerCount = 0;
    int activeNoteCount = 0;
    int lastSequence = 0;
};
}
