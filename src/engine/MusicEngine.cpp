#include "MusicEngine.h"

#include "../instrument/Instrument.h"
#include "../state/MidiNoteState.h"

#include <iostream>

MusicEngine::MusicEngine(const Instrument& instrument, MidiNoteState& midiNoteStateToUse)
    : midiNoteState(midiNoteStateToUse)
{
    configureInstrument(instrument);
}

// 악기 준비 상태에 따라 렌더러 구성을 초기화한다.
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

// 실행 중에 다른 악기로 바꿔도 엔진의 나머지 연결은 유지한다.
void MusicEngine::setInstrument(const Instrument& instrument)
{
    configureInstrument(instrument);
}

// 세션이 시작되기 전에 악기 준비 여부를 확인할 수 있게 한다.
bool MusicEngine::isReady() const
{
    return ready;
}

// 악기 초기화 실패 이유를 상위 레이어로 전달한다.
const juce::String& MusicEngine::getLastError() const
{
    return lastError;
}

// 오디오 시작 시 샘플레이트와 MIDI 큐 타이밍 기준을 다시 맞춘다.
void MusicEngine::audioDeviceAboutToStart(juce::AudioIODevice* device)
{
    // 오디오 장치가 시작될 때 실제 재생 샘플레이트를 렌더러와 MIDI 큐에 맞춘다.
    renderer.setCurrentPlaybackSampleRate(device->getCurrentSampleRate());
    midiCollector.reset(device->getCurrentSampleRate());
}

// 장치가 멈추면 눌린 노트 상태도 같이 초기화한다.
void MusicEngine::audioDeviceStopped()
{
    midiNoteState.clear();
}

// 오디오 콜백에서는 큐에 쌓인 MIDI 이벤트를 꺼내 실제 소리를 렌더링한다.
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

// MIDI 입력을 받아 오디오와 UI, 외부 구독자에게 필요한 정보를 각각 분기한다.
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

    publishMidiEvent(source, timestampedMessage);

    if (!timestampedMessage.isNoteOn())
        return;

    const auto noteNumber = timestampedMessage.getNoteNumber();

    std::cout << "[" << source->getName() << "] "
              << timestampedMessage.getMidiNoteName(noteNumber, true, true, 3)
              << " | velocity " << timestampedMessage.getVelocity()
              << std::endl;
}

// 외부 전송 레이어는 이 이벤트만 구독하고 실제 네트워크 구현은 별도로 가진다.
void MusicEngine::publishMidiEvent(juce::MidiInput* source, const juce::MidiMessage& message)
{
    if (onMidiEvent == nullptr)
        return;

    if (!message.isNoteOn() && !message.isNoteOff())
        return;

    MidiPerformanceEvent event;
    event.type = message.isNoteOn() ? MidiPerformanceEvent::Type::noteOn
                                    : MidiPerformanceEvent::Type::noteOff;
    event.note = message.getNoteNumber();
    event.velocity = message.getVelocity();
    event.eventTime = message.getTimeStamp();
    event.sequence = nextEventSequence++;

    if (source != nullptr)
        event.sourceName = source->getName();

    onMidiEvent(event);
}
