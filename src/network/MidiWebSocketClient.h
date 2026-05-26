#pragma once

#include <functional>
#include <memory>

#include <juce_core/juce_core.h>

#include "../engine/MidiPerformanceEvent.h"

class MidiWebSocketClient
{
public:
    MidiWebSocketClient() = default;
    ~MidiWebSocketClient();

    // 나중에 실제 서버 주소를 바꿀 수 있도록 엔드포인트를 분리해 둔다.
    void setEndpoint(juce::String newEndpointUrl);
    // 현재는 송신 준비 상태만 관리하고 실제 프레임 전송은 외부 콜백에 위임한다.
    bool connect();
    // 화면이 내려갈 때 더 이상 이벤트를 보내지 않도록 연결 상태를 정리한다.
    void disconnect();
    // 상위 레이어가 실제 웹소켓 라이브러리의 send 함수를 주입할 수 있게 한다.
    void setSendTextCallback(std::function<void(const juce::String&)> callback);
    // MIDI 이벤트를 JSON 문자열로 직렬화해 외부 전송 경로로 넘긴다.
    void sendMidiEvent(const MidiPerformanceEvent& event);

    bool isConnected() const;
    const juce::String& getLastPayload() const;
    const juce::String& getLastError() const;

private:
    struct EndpointParts;
    class NativeConnection;

    // JUCE JSON 유틸리티를 써서 서버 친화적인 텍스트 payload를 만든다.
    juce::String serialiseMidiEvent(const MidiPerformanceEvent& event) const;
    // ws:// 또는 wss:// 주소를 WinHTTP 연결 정보로 분해한다.
    static bool parseEndpoint(const juce::String& endpoint, EndpointParts& parts, juce::String& error);

    juce::String endpointUrl;
    juce::String lastPayload;
    juce::String lastError;
    bool connected = false;
    std::function<void(const juce::String&)> sendTextCallback;
    std::unique_ptr<NativeConnection> nativeConnection;
};
