#include "MusicEngine.h"

#include "../instrument/Instrument.h"
#include "../state/MidiNoteState.h"

#include <iostream>

MusicEngine::MusicEngine(const Instrument& instrument, MidiNoteState& midiNoteStateToUse)
    : midiNoteState(midiNoteStateToUse)
{
    configureInstrument(instrument);
}

void MusicEngine::configureInstrument(const Instrument& instrument)
{
    renderer.clearSounds();
    renderer.clearVoices();

    // 악기 구현이 어떤 사운드/보이스를 쓸지 결정하고 엔진 렌더러에 주입한다.
    if (!instrument.isReady())
    {
        ready = false;
        lastError = instrument.getLastError();
        return;
    }

    instrument.configureRenderer(renderer);
    ready = true;
}

void MusicEngine::setInstrument(const Instrument& instrument)
{
    configureInstrument(instrument);
}

bool MusicEngine::isReady() const
{
    return ready;
}

const juce::String& MusicEngine::getLastError() const
{
    return lastError;
}

void MusicEngine::audioDeviceAboutToStart(juce::AudioIODevice* device)
{
    // 오디오 장치가 시작될 때 실제 재생 샘플레이트를 렌더러와 MIDI 큐에 맞춘다.
    renderer.setCurrentPlaybackSampleRate(device->getCurrentSampleRate());
    midiCollector.reset(device->getCurrentSampleRate());
}

void MusicEngine::audioDeviceStopped()
{
    midiNoteState.clear();
}

void MusicEngine::audioDeviceIOCallbackWithContext(const float* const*,
                                                   int,
                                                   float* const* outputChannelData,
                                                   int numOutputChannels,
                                                   int numSamples,
                                                   const juce::AudioIODeviceCallbackContext&)
{
    juce::AudioBuffer<float> outputBuffer(outputChannelData, numOutputChannels, numSamples);
    outputBuffer.clear();

    juce::MidiBuffer incomingMidi;
    midiCollector.removeNextBlockOfMessages(incomingMidi, numSamples);

    // 이 지점에서 MIDI 이벤트를 기반으로 실제 오디오 샘플을 출력 버퍼에 렌더링한다.
    renderer.renderNextBlock(outputBuffer, incomingMidi, 0, numSamples);
}

void MusicEngine::handleIncomingMidiMessage(juce::MidiInput* source,
                                            const juce::MidiMessage& message)
{
    if (message.isActiveSense() || message.isMidiClock() || message.isMidiStart()
        || message.isMidiContinue() || message.isMidiStop())
    {
        return;
    }

    // 입력 스레드에서 들어온 MIDI를 오디오 콜백에서 소비할 수 있도록 큐에 넣는다.
    auto timestampedMessage = message;
    timestampedMessage.setTimeStamp(juce::Time::getMillisecondCounterHiRes() * 0.001);
    midiCollector.addMessageToQueue(timestampedMessage);

    if (timestampedMessage.isNoteOn())
        midiNoteState.setNoteActive(timestampedMessage.getNoteNumber(), true);
    else if (timestampedMessage.isNoteOff())
        midiNoteState.setNoteActive(timestampedMessage.getNoteNumber(), false);

    if (!timestampedMessage.isNoteOn())
        return;

    const auto noteNumber = timestampedMessage.getNoteNumber();

    std::cout << "[" << source->getName() << "] "
              << timestampedMessage.getMidiNoteName(noteNumber, true, true, 3)
              << " | velocity " << timestampedMessage.getVelocity()
              << std::endl;
}
