#include "AudioSession.h"

#include "../audio/AudioDeviceConfig.h"
#include "../instrument/Instrument.h"
#include "../state/MidiNoteState.h"

AudioSession::AudioSession(const Instrument& initialInstrument, MidiNoteState& midiNoteState)
    : engine(initialInstrument, midiNoteState)
{
}

AudioSession::~AudioSession()
{
    stop();
}

bool AudioSession::start()
{
    if (!engine.isReady())
    {
        lastError = "Engine error: " + engine.getLastError();
        return false;
    }

    auto audioError = deviceManager.initialise(0, 2, nullptr, true);

    if (audioError.isNotEmpty())
    {
        lastError = "Audio init failed: " + audioError;
        return false;
    }

    audioError = audio::configurePreferredAudioDevice(deviceManager);

    if (audioError.isNotEmpty())
    {
        lastError = "Audio config failed: " + audioError;
        return false;
    }

    deviceManager.addAudioCallback(&engine);

    const auto midiInputs = juce::MidiInput::getAvailableDevices();

    if (midiInputs.isEmpty())
    {
        lastError = "Ready, but no MIDI input device found.";
        return true;
    }

    midiInput = juce::MidiInput::openDevice(midiInputs[0].identifier, &engine);

    if (midiInput == nullptr)
    {
        lastError = "Failed to open MIDI input device.";
        return false;
    }

    midiInput->start();
    lastError = "Ready. Play the keyboard.";
    return true;
}

void AudioSession::stop()
{
    if (midiInput != nullptr)
    {
        midiInput->stop();
        midiInput.reset();
    }

    deviceManager.removeAudioCallback(&engine);
}

bool AudioSession::setInstrument(const Instrument& instrument)
{
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
    if (midiInput != nullptr)
        return midiInput->getName();

    return {};
}
