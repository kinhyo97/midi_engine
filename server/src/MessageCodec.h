#pragma once

#include <vector>

#include "RelayTypes.h"

namespace relay
{
class MessageCodec
{
public:
    // performer가 보낸 JSON 문자열을 서버 내부 이벤트로 역직렬화한다.
    static bool parseMidiEvent(const juce::String& payload, MidiRelayEvent& event, juce::String& error);
    // listener 브로드캐스트용 event envelope을 JSON 문자열로 만든다.
    static juce::String makeMidiEventEnvelope(const juce::String& roomId, const MidiRelayEvent& event);
    // late joiner가 바로 현재 눌린 상태를 받을 수 있도록 snapshot envelope을 만든다.
    static juce::String makeSnapshotEnvelope(const juce::String& roomId, const std::vector<ActiveNoteInfo>& activeNotes);
    // 운영상태나 에러를 텍스트 JSON notice로 보내기 위한 헬퍼다.
    static juce::String makeNoticeEnvelope(const juce::String& type, const juce::String& message);
};
}
