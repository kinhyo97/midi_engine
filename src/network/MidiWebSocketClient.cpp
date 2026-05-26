#include "MidiWebSocketClient.h"

#if JUCE_WINDOWS
 #include <windows.h>
 #include <winhttp.h>
#endif

#include <utility>

struct MidiWebSocketClient::EndpointParts
{
    juce::String host;
    juce::String pathAndQuery;
#if JUCE_WINDOWS
    INTERNET_PORT port = 0;
#else
    int port = 0;
#endif
    bool secure = false;
};

#if JUCE_WINDOWS
class MidiWebSocketClient::NativeConnection
{
public:
    // WinHTTP가 반환하는 핸들을 소유하고 정리하는 얇은 래퍼다.
    ~NativeConnection()
    {
        close();
    }

    // 엔드포인트에 대해 WebSocket 업그레이드를 수행하고 핸들을 저장한다.
    bool connect(const EndpointParts& endpoint, juce::String& error)
    {
        close();

        session = WinHttpOpen(L"midi_engine/0.1.0",
                              WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                              WINHTTP_NO_PROXY_NAME,
                              WINHTTP_NO_PROXY_BYPASS,
                              0);

        if (session == nullptr)
        {
            error = "WinHttpOpen failed: " + juce::String((int) GetLastError());
            return false;
        }

        connection = WinHttpConnect(session, endpoint.host.toWideCharPointer(), endpoint.port, 0);

        if (connection == nullptr)
        {
            error = "WinHttpConnect failed: " + juce::String((int) GetLastError());
            close();
            return false;
        }

        const auto flags = endpoint.secure ? WINHTTP_FLAG_SECURE : 0;

        request = WinHttpOpenRequest(connection,
                                     L"GET",
                                     endpoint.pathAndQuery.toWideCharPointer(),
                                     nullptr,
                                     WINHTTP_NO_REFERER,
                                     WINHTTP_DEFAULT_ACCEPT_TYPES,
                                     flags);

        if (request == nullptr)
        {
            error = "WinHttpOpenRequest failed: " + juce::String((int) GetLastError());
            close();
            return false;
        }

        if (!WinHttpSetOption(request, WINHTTP_OPTION_UPGRADE_TO_WEB_SOCKET, nullptr, 0))
        {
            error = "WinHttpSetOption failed: " + juce::String((int) GetLastError());
            close();
            return false;
        }

        if (!WinHttpSendRequest(request,
                                WINHTTP_NO_ADDITIONAL_HEADERS,
                                0,
                                WINHTTP_NO_REQUEST_DATA,
                                0,
                                0,
                                0))
        {
            error = "WinHttpSendRequest failed: " + juce::String((int) GetLastError());
            close();
            return false;
        }

        if (!WinHttpReceiveResponse(request, nullptr))
        {
            error = "WinHttpReceiveResponse failed: " + juce::String((int) GetLastError());
            close();
            return false;
        }

        DWORD statusCode = 0;
        DWORD statusCodeSize = sizeof(statusCode);

        if (!WinHttpQueryHeaders(request,
                                 WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                                 WINHTTP_HEADER_NAME_BY_INDEX,
                                 &statusCode,
                                 &statusCodeSize,
                                 WINHTTP_NO_HEADER_INDEX))
        {
            error = "WinHttpQueryHeaders failed: " + juce::String((int) GetLastError());
            close();
            return false;
        }

        if (statusCode != 101)
        {
            error = "WebSocket upgrade rejected with status " + juce::String((int) statusCode);
            close();
            return false;
        }

        websocket = WinHttpWebSocketCompleteUpgrade(request, 0);

        if (websocket == nullptr)
        {
            error = "WinHttpWebSocketCompleteUpgrade failed: " + juce::String((int) GetLastError());
            close();
            return false;
        }

        WinHttpCloseHandle(request);
        request = nullptr;
        return true;
    }

    // 이미 열린 WebSocket 핸들에 UTF-8 텍스트 프레임을 보낸다.
    bool sendText(const juce::String& text, juce::String& error)
    {
        if (websocket == nullptr)
        {
            error = "WebSocket is not connected.";
            return false;
        }

        const auto utf8 = text.toUTF8();
        const auto result = WinHttpWebSocketSend(websocket,
                                                 WINHTTP_WEB_SOCKET_UTF8_MESSAGE_BUFFER_TYPE,
                                                 (void*) utf8.getAddress(),
                                                 (DWORD) utf8.sizeInBytes() - 1);

        if (result != NO_ERROR)
        {
            error = "WinHttpWebSocketSend failed: " + juce::String((int) result);
            return false;
        }

        return true;
    }

    // 열려 있는 핸들을 역순으로 닫아 세션을 안전하게 끝낸다.
    void close()
    {
        if (websocket != nullptr)
        {
            WinHttpWebSocketClose(websocket, WINHTTP_WEB_SOCKET_SUCCESS_CLOSE_STATUS, nullptr, 0);
            WinHttpCloseHandle(websocket);
            websocket = nullptr;
        }

        if (request != nullptr)
        {
            WinHttpCloseHandle(request);
            request = nullptr;
        }

        if (connection != nullptr)
        {
            WinHttpCloseHandle(connection);
            connection = nullptr;
        }

        if (session != nullptr)
        {
            WinHttpCloseHandle(session);
            session = nullptr;
        }
    }

private:
    HINTERNET session = nullptr;
    HINTERNET connection = nullptr;
    HINTERNET request = nullptr;
    HINTERNET websocket = nullptr;
};
#endif

MidiWebSocketClient::~MidiWebSocketClient() = default;

void MidiWebSocketClient::setEndpoint(juce::String newEndpointUrl)
{
    endpointUrl = std::move(newEndpointUrl);
}

bool MidiWebSocketClient::connect()
{
    lastError.clear();

    if (endpointUrl.isEmpty())
    {
        lastError = "Endpoint is empty.";
        return false;
    }

#if JUCE_WINDOWS
    EndpointParts endpoint;

    if (!parseEndpoint(endpointUrl, endpoint, lastError))
        return false;

    nativeConnection = std::make_unique<NativeConnection>();
    connected = nativeConnection->connect(endpoint, lastError);

    if (!connected)
        nativeConnection.reset();

    return connected;
#else
    lastError = "Native WebSocket transport is implemented only for Windows right now.";
    return false;
#endif
}

void MidiWebSocketClient::disconnect()
{
#if JUCE_WINDOWS
    if (nativeConnection != nullptr)
        nativeConnection->close();

    nativeConnection.reset();
#endif

    connected = false;
}

void MidiWebSocketClient::setSendTextCallback(std::function<void(const juce::String&)> callback)
{
    sendTextCallback = std::move(callback);
}

void MidiWebSocketClient::sendMidiEvent(const MidiPerformanceEvent& event)
{
    if (!connected)
        return;

    lastPayload = serialiseMidiEvent(event);

    if (sendTextCallback != nullptr)
    {
        sendTextCallback(lastPayload);
        return;
    }

#if JUCE_WINDOWS
    if (nativeConnection == nullptr || !nativeConnection->sendText(lastPayload, lastError))
    {
        juce::Logger::writeToLog("[MidiWebSocketClient] send failed: " + lastError);
        connected = false;
        return;
    }
#else
    juce::Logger::writeToLog("[MidiWebSocketClient] " + lastPayload);
#endif
}

bool MidiWebSocketClient::isConnected() const
{
    return connected;
}

const juce::String& MidiWebSocketClient::getLastPayload() const
{
    return lastPayload;
}

const juce::String& MidiWebSocketClient::getLastError() const
{
    return lastError;
}

juce::String MidiWebSocketClient::serialiseMidiEvent(const MidiPerformanceEvent& event) const
{
    auto payload = juce::DynamicObject::Ptr(new juce::DynamicObject());
    payload->setProperty("seq", event.sequence);
    payload->setProperty("type", event.getTypeName());
    payload->setProperty("note", event.note);
    payload->setProperty("velocity", event.velocity);
    payload->setProperty("eventTime", event.eventTime);

    if (event.sourceName.isNotEmpty())
        payload->setProperty("source", event.sourceName);

    return juce::JSON::toString(juce::var(payload.get()), true);
}

bool MidiWebSocketClient::parseEndpoint(const juce::String& endpoint, EndpointParts& parts, juce::String& error)
{
    const auto schemeSeparator = endpoint.indexOf("://");

    if (schemeSeparator < 0)
    {
        error = "Endpoint must start with ws:// or wss://";
        return false;
    }

    const auto scheme = endpoint.substring(0, schemeSeparator).toLowerCase();
    const auto remainder = endpoint.substring(schemeSeparator + 3);
    const auto firstSlash = remainder.indexOfChar('/');
    const auto hostPort = firstSlash >= 0 ? remainder.substring(0, firstSlash) : remainder;

    if (hostPort.isEmpty())
    {
        error = "Endpoint host is empty.";
        return false;
    }

    parts.secure = scheme == "wss";

    if (!parts.secure && scheme != "ws")
    {
        error = "Unsupported endpoint scheme: " + scheme;
        return false;
    }

    const auto colon = hostPort.lastIndexOfChar(':');

    if (colon >= 0)
    {
        parts.host = hostPort.substring(0, colon);
        parts.port = (INTERNET_PORT) hostPort.substring(colon + 1).getIntValue();
    }
    else
    {
        parts.host = hostPort;
        parts.port = parts.secure ? INTERNET_DEFAULT_HTTPS_PORT : INTERNET_DEFAULT_HTTP_PORT;
    }

    if (parts.host.isEmpty() || parts.port == 0)
    {
        error = "Endpoint host or port is invalid.";
        return false;
    }

    parts.pathAndQuery = firstSlash >= 0 ? remainder.substring(firstSlash) : "/";
    return true;
}
