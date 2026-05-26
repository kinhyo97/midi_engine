#pragma once

#include <functional>

#include <juce_audio_devices/juce_audio_devices.h>

#include "MidiPerformanceEvent.h"

class MidiNoteState;
class Instrument;

class MusicEngine : public juce::AudioIODeviceCallback,
                    public juce::MidiInputCallback
{
public:
    MusicEngine(const Instrument& instrument, MidiNoteState& midiNoteState);

    bool isReady() const;
    const juce::String& getLastError() const;
    void setInstrument(const Instrument& instrument);

    void audioDeviceAboutToStart(juce::AudioIODevice* device) override;
    void audioDeviceStopped() override;
    void audioDeviceIOCallbackWithContext(const float* const* inputChannelData,
                                          int numInputChannels,
                                          float* const* outputChannelData,
                                          int numOutputChannels,
                                          int numSamples,
                                          const juce::AudioIODeviceCallbackContext& context) override;
    void handleIncomingMidiMessage(juce::MidiInput* source,
                                   const juce::MidiMessage& message) override;

    // 외부 레이어가 noteOn/noteOff 이벤트를 구독할 수 있게 콜백을 공개한다.
    std::function<void(const MidiPerformanceEvent&)> onMidiEvent;

private:
    // 연주 로직과 별개로 필요한 외부 구독자에게 MIDI 이벤트를 전달한다.
    void publishMidiEvent(juce::MidiInput* source, const juce::MidiMessage& message);
    void configureInstrument(const Instrument& instrument);

    MidiNoteState& midiNoteState;
    juce::MidiMessageCollector midiCollector;
    juce::Synthesiser renderer;
    bool ready = false;
    int nextEventSequence = 1;
    juce::String lastError;
};
