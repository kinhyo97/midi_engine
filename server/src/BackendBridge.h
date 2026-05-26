#pragma once

#include "RelayTypes.h"

namespace relay
{
class BackendBridge
{
public:
    virtual ~BackendBridge() = default;

    // room에 performer가 처음 올라왔을 때 메인 백엔드와의 연동 포인트가 된다.
    virtual void onRoomActivated(const juce::String& roomId, const juce::String& performerId) = 0;
    // room이 비워졌을 때 세션 종료나 정리 작업을 연결할 수 있다.
    virtual void onRoomDeactivated(const juce::String& roomId) = 0;
    // 실시간 이벤트를 요약 전달하고 싶을 때 붙일 수 있는 확장 지점이다.
    virtual void onEventRelayed(const juce::String& roomId, const MidiRelayEvent& event) = 0;
};
}
