#include <iostream>
#include <memory>

#include "ConsoleClient.h"
#include "LoggingBackendBridge.h"
#include "RelayServer.h"

namespace
{
// 콘솔 사용법을 빠르게 확인할 수 있게 명령 목록을 출력한다.
void printHelp()
{
    std::cout << "Commands:\n"
              << "  help\n"
              << "  performer <roomId> <clientId>\n"
              << "  listener <roomId> <clientId>\n"
              << "  send <clientId> <json>\n"
              << "  drop <clientId>\n"
              << "  rooms\n"
              << "  quit\n";
}

// 공백 기준으로 앞쪽 토큰만 나누고 JSON 본문은 원문 형태로 남겨둔다.
juce::StringArray splitCommand(const juce::String& line)
{
    juce::StringArray parts;
    int index = 0;

    while (index < line.length())
    {
        while (index < line.length() && juce::CharacterFunctions::isWhitespace(line[index]))
            ++index;

        if (index >= line.length())
            break;

        if (parts.size() == 2 && parts[0] == "send")
        {
            parts.add(line.substring(index));
            break;
        }

        int next = index;

        while (next < line.length() && !juce::CharacterFunctions::isWhitespace(line[next]))
            ++next;

        parts.add(line.substring(index, next));
        index = next;
    }

    return parts;
}

// room 요약을 사람이 읽기 좋은 한 줄 로그로 출력한다.
void printRooms(const relay::RelayServer& server)
{
    const auto rooms = server.getRoomSummaries();

    if (rooms.empty())
    {
        std::cout << "No active rooms.\n";
        return;
    }

    for (const auto& room : rooms)
    {
        std::cout << "room=" << room.roomId.toRawUTF8()
                  << " performer=" << (room.performerId.isEmpty() ? "-" : room.performerId.toRawUTF8())
                  << " listeners=" << room.listenerCount
                  << " activeNotes=" << room.activeNoteCount
                  << " lastSeq=" << room.lastSequence
                  << '\n';
    }
}
}

int main()
{
    auto backendBridge = std::make_shared<relay::LoggingBackendBridge>();
    relay::RelayServer server(backendBridge);

    std::cout << "midi_engine_server console relay\n";
    printHelp();

    for (;;)
    {
        std::cout << "> ";
        std::string rawLine;

        if (!std::getline(std::cin, rawLine))
            break;

        const auto line = juce::String(rawLine).trim();

        if (line.isEmpty())
            continue;

        const auto parts = splitCommand(line);
        const auto command = parts[0];
        juce::String error;

        if (command == "help")
        {
            printHelp();
            continue;
        }

        if (command == "quit" || command == "exit")
            break;

        if (command == "performer" || command == "listener")
        {
            if (parts.size() != 3)
            {
                std::cout << "usage: " << command.toRawUTF8() << " <roomId> <clientId>\n";
                continue;
            }

            const auto role = command == "performer" ? relay::ClientRole::performer
                                                     : relay::ClientRole::listener;
            auto client = std::make_shared<relay::ConsoleClient>(parts[1], parts[2], role);

            if (!server.registerClient(client, error))
            {
                std::cout << "register failed: " << error.toRawUTF8() << '\n';
                continue;
            }

            std::cout << "registered " << command.toRawUTF8()
                      << ' ' << parts[2].toRawUTF8()
                      << " in room " << parts[1].toRawUTF8()
                      << '\n';
            continue;
        }

        if (command == "send")
        {
            if (parts.size() != 3)
            {
                std::cout << "usage: send <clientId> <json>\n";
                continue;
            }

            if (!server.handleClientPayload(parts[1], parts[2], error))
            {
                std::cout << "relay failed: " << error.toRawUTF8() << '\n';
                continue;
            }

            std::cout << "relayed event from " << parts[1].toRawUTF8() << '\n';
            continue;
        }

        if (command == "drop")
        {
            if (parts.size() != 2)
            {
                std::cout << "usage: drop <clientId>\n";
                continue;
            }

            server.unregisterClient(parts[1]);
            std::cout << "dropped " << parts[1].toRawUTF8() << '\n';
            continue;
        }

        if (command == "rooms")
        {
            printRooms(server);
            continue;
        }

        std::cout << "unknown command: " << command.toRawUTF8() << '\n';
    }

    return 0;
}
