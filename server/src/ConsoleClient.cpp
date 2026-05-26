#include "ConsoleClient.h"

#include <iostream>

namespace relay
{
ConsoleClient::ConsoleClient(juce::String roomIdToUse, juce::String clientIdToUse, ClientRole roleToUse)
    : roomId(std::move(roomIdToUse)),
      clientId(std::move(clientIdToUse)),
      role(roleToUse)
{
}

// 콘솔 mock client도 실제 transport와 같은 식별 인터페이스를 제공한다.
const juce::String& ConsoleClient::getClientId() const
{
    return clientId;
}

// 서버가 performer/listener를 구분할 수 있도록 역할을 반환한다.
ClientRole ConsoleClient::getRole() const
{
    return role;
}

// room 라우팅은 mock transport에서도 동일하게 유지한다.
const juce::String& ConsoleClient::getRoomId() const
{
    return roomId;
}

// listener에게 브로드캐스트된 JSON payload를 콘솔 로그로 확인한다.
void ConsoleClient::sendText(const juce::String& payload)
{
    std::cout << "[client " << clientId.toRawUTF8() << "] " << payload.toRawUTF8() << std::endl;
}
}
