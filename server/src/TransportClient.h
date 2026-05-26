#pragma once

#include <memory>

#include "RelayTypes.h"

namespace relay
{
class TransportClient
{
public:
    virtual ~TransportClient() = default;

    // 서버는 transport 구현을 몰라도 클라이언트 식별값은 볼 수 있어야 한다.
    virtual const juce::String& getClientId() const = 0;
    // performer/listener 분기에 쓰는 역할 정보를 제공한다.
    virtual ClientRole getRole() const = 0;
    // 어느 room에 붙어 있는지 서버 라우팅에서 사용한다.
    virtual const juce::String& getRoomId() const = 0;
    // 실제 transport가 텍스트 payload를 전달하는 유일한 출력 지점이다.
    virtual void sendText(const juce::String& payload) = 0;
};

using TransportClientPtr = std::shared_ptr<TransportClient>;
}
