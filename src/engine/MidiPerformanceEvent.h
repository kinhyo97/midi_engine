#pragma once

#include <juce_core/juce_core.h>

struct MidiPerformanceEvent
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

    // 웹소켓 메시지에서는 문자열 타입이 더 다루기 쉬워서 변환 헬퍼를 둔다.
    juce::String getTypeName() const
    {
        return type == Type::noteOn ? "noteOn" : "noteOff";
    }
};
