#pragma once

#include <juce_audio_devices/juce_audio_devices.h>

#include "../engine/MusicEngine.h"

class Instrument;
class MidiNoteState;

class AudioSession
{
public:
    struct Status
    {
        juce::String audioDeviceType;
        juce::String audioDeviceName;
        juce::String sampleRate;
        juce::String bufferSize;
        juce::String latency;
        juce::String midiInputName;
        juce::String message;
    };

    AudioSession(const Instrument& initialInstrument, MidiNoteState& midiNoteState);
    ~AudioSession();

    bool start();
    void stop();
    bool setInstrument(const Instrument& instrument);

    bool isReady() const;
    const juce::String& getLastError() const;

    Status getStatus() const;

private:
    juce::String getMidiInputName() const;

    MusicEngine engine;
    juce::AudioDeviceManager deviceManager;
    std::unique_ptr<juce::MidiInput> midiInput;
    juce::String lastError;
};
