#pragma once

#include <juce_audio_devices/juce_audio_devices.h>

namespace audio
{
juce::String configurePreferredAudioDevice(juce::AudioDeviceManager& deviceManager);
}
