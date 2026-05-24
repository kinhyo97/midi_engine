#pragma once

#include <juce_audio_devices/juce_audio_devices.h>

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

private:
    void configureInstrument(const Instrument& instrument);

    MidiNoteState& midiNoteState;
    juce::MidiMessageCollector midiCollector;
    juce::Synthesiser renderer;
    bool ready = false;
    juce::String lastError;
};
