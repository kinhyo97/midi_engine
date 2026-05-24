cmake_minimum_required(VERSION 3.22)

project(midi_engine VERSION 0.1.0 LANGUAGES C CXX)

add_subdirectory(external/JUCE)

juce_add_gui_app(midi_engine
    PRODUCT_NAME "midi_engine"
)

target_sources(midi_engine
    PRIVATE
        src/audio/AudioDeviceConfig.cpp
        src/constant/Note.cpp
        src/engine/MusicEngine.cpp
        src/instrument/PianoInstrument.cpp
        src/instrument/SynthInstrument.cpp
        src/main.cpp
        src/sample/SampleLibrary.cpp
        src/session/AudioSession.cpp
        src/state/MidiNoteState.cpp
        src/ui/EngineDashboard.cpp
        src/ui/MainComponent.cpp
        src/ui/PianoKeyboardView.cpp
)

target_compile_features(midi_engine PRIVATE cxx_std_17)

target_compile_definitions(midi_engine
    PRIVATE
        JUCE_ASIO=1
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
)

target_link_libraries(midi_engine
    PRIVATE
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_gui_basics
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags
)
