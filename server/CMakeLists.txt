cmake_minimum_required(VERSION 3.22)

project(midi_engine_server VERSION 0.1.0 LANGUAGES CXX)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../external/JUCE ${CMAKE_CURRENT_BINARY_DIR}/juce)

add_executable(midi_engine_server
    src/main.cpp
    src/ConsoleClient.cpp
    src/EventValidator.cpp
    src/LoggingBackendBridge.cpp
    src/MessageCodec.cpp
    src/RelayServer.cpp
    src/RoomState.cpp
)

target_compile_features(midi_engine_server PRIVATE cxx_std_17)

target_compile_definitions(midi_engine_server
    PRIVATE
        JUCE_USE_CURL=0
)

target_link_libraries(midi_engine_server
    PRIVATE
        juce::juce_core
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags
)
