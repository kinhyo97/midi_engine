#include "LoggingBackendBridge.h"

#include <iostream>

namespace relay
{
// 지금은 메인 백엔드 대신 콘솔 로그로 room 활성화를 확인한다.
void LoggingBackendBridge::onRoomActivated(const juce::String& roomId, const juce::String& performerId)
{
    std::cout << "[backend] room activated: " << roomId.toRawUTF8()
              << " performer=" << performerId.toRawUTF8()
              << std::endl;
}

// room 종료 시점도 같은 방식으로 기록해 나중에 REST 브리지로 바꾸기 쉽게 둔다.
void LoggingBackendBridge::onRoomDeactivated(const juce::String& roomId)
{
    std::cout << "[backend] room deactivated: " << roomId.toRawUTF8() << std::endl;
}

// relay 서버가 어떤 이벤트를 외부로 중계했는지 추적하기 위한 기본 훅이다.
void LoggingBackendBridge::onEventRelayed(const juce::String& roomId, const MidiRelayEvent& event)
{
    std::cout << "[backend] relayed room=" << roomId.toRawUTF8()
              << " seq=" << event.sequence
              << " type=" << event.getTypeName().toRawUTF8()
              << " note=" << event.note
              << std::endl;
}
}
