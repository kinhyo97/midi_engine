#include "AudioSession.h"

#include "../audio/AudioDeviceConfig.h"
#include "../instrument/Instrument.h"
#include "../state/MidiNoteState.h"

AudioSession::AudioSession(const Instrument& initialInstrument, MidiNoteState& midiNoteState)
    // 세션이 사용할 오디오 엔진을 초기 악기와 함께 준비한다.
    : engine(initialInstrument, midiNoteState)
{
}

AudioSession::~AudioSession()
{
    stop();
}

bool AudioSession::start()
{
    // 엔진 자체가 준비되지 않았다면 오디오/MIDI 세션을 시작할 수 없다.
    if (!engine.isReady())
    {
        lastError = "Engine error: " + engine.getLastError();
        return false;
    }

    // 출력 2채널 기준으로 오디오 디바이스 매니저를 초기화한다.
    auto audioError = deviceManager.initialise(0, 2, nullptr, true);

    if (audioError.isNotEmpty())
    {
        lastError = "Audio init failed: " + audioError;
        return false;
    }

    // 가능한 경우 선호하는 오디오 드라이버/장치 설정을 적용한다.
    audioError = audio::configurePreferredAudioDevice(deviceManager);

    if (audioError.isNotEmpty())
    {
        lastError = "Audio config failed: " + audioError;
        return false;
    }

    // 실제 오디오 렌더링은 engine이 콜백으로 처리한다.
    deviceManager.addAudioCallback(&engine);

    // 현재 시스템에서 사용 가능한 MIDI 입력 장치를 조회한다.
    const auto midiInputs = juce::MidiInput::getAvailableDevices();

    if (midiInputs.isEmpty())
    {
        lastError = "Ready, but no MIDI input device found.";
        return true;
    }

    // 첫 번째 MIDI 입력 장치를 열고, 들어오는 메시지는 engine으로 전달한다.
    midiInput = juce::MidiInput::openDevice(midiInputs[0].identifier, &engine);

    if (midiInput == nullptr)
    {
        lastError = "Failed to open MIDI input device.";
        return false;
    }

    // MIDI 입력 수신을 시작하면 실제 연주 이벤트가 엔진으로 들어간다.
    midiInput->start();
    lastError = "Ready. Play the keyboard.";
    return true;
}

void AudioSession::stop()
{
    // 먼저 MIDI 입력을 멈추고 장치 핸들을 정리한다.
    if (midiInput != nullptr)
    {
        midiInput->stop();
        midiInput.reset();
    }

    // 마지막으로 오디오 콜백을 제거해 엔진 렌더링을 중단한다.
    deviceManager.removeAudioCallback(&engine);
}

bool AudioSession::setInstrument(const Instrument& instrument)
{
    // 현재 세션에서 사용할 악기 구현을 엔진에 교체한다.
    engine.setInstrument(instrument);

    if (!engine.isReady())
    {
        lastError = "Instrument error: " + engine.getLastError();
        return false;
    }

    lastError = "Instrument changed to " + instrument.getName();
    return true;
}

bool AudioSession::isReady() const
{
    return engine.isReady();
}

// UI 조립 레이어가 엔진 이벤트 콜백을 연결할 수 있게 엔진 참조를 돌려준다.
MusicEngine& AudioSession::getEngine()
{
    return engine;
}

const juce::String& AudioSession::getLastError() const
{
    return lastError;
}

AudioSession::Status AudioSession::getStatus() const
{
    Status status;
    status.audioDeviceType = deviceManager.getCurrentAudioDeviceType();
    status.message = lastError;
    status.midiInputName = getMidiInputName();

    // 현재 연결된 오디오 장치가 있으면 표시용 상태 정보를 채운다.
    if (auto* device = deviceManager.getCurrentAudioDevice())
    {
        status.audioDeviceName = device->getName();
        status.sampleRate = juce::String(device->getCurrentSampleRate());
        status.bufferSize = juce::String(device->getCurrentBufferSizeSamples()) + " samples";
        status.latency = "out "
                         + juce::String(device->getOutputLatencyInSamples())
                         + " / in "
                         + juce::String(device->getInputLatencyInSamples())
                         + " samples";

        return status;
    }

    status.audioDeviceName = "none";
    status.sampleRate = "-";
    status.bufferSize = "-";
    status.latency = "-";
    return status;
}

juce::String AudioSession::getMidiInputName() const
{
    // 열려 있는 MIDI 입력 장치가 있으면 그 이름을 반환한다.
    if (midiInput != nullptr)
        return midiInput->getName();

    return {};
}
