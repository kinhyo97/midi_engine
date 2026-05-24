#include "AudioDeviceConfig.h"

namespace
{
juce::String findPreferredDeviceName(const juce::StringArray& deviceNames,
                                     const juce::String& preferredSubstring)
{
    for (const auto& name : deviceNames)
    {
        if (name.containsIgnoreCase(preferredSubstring))
            return name;
    }

    return {};
}
}

namespace audio
{
juce::String configurePreferredAudioDevice(juce::AudioDeviceManager& deviceManager)
{
    auto& availableTypes = deviceManager.getAvailableDeviceTypes();
    juce::String preferredTypeName;

    for (auto* type : availableTypes)
    {
        if (type->getTypeName().containsIgnoreCase("ASIO"))
        {
            preferredTypeName = type->getTypeName();
            break;
        }
    }

    if (preferredTypeName.isNotEmpty()
        && deviceManager.getCurrentAudioDeviceType() != preferredTypeName)
    {
        deviceManager.setCurrentAudioDeviceType(preferredTypeName, true);
    }

    auto setup = deviceManager.getAudioDeviceSetup();

    for (auto* type : availableTypes)
    {
        if (type->getTypeName() != deviceManager.getCurrentAudioDeviceType())
            continue;

        type->scanForDevices();

        const auto outputNames = type->getDeviceNames(false);
        const auto preferredOutput = findPreferredDeviceName(outputNames, "Babyface");

        if (preferredOutput.isNotEmpty())
        {
            setup.outputDeviceName = preferredOutput;
            break;
        }
    }

    setup.useDefaultInputChannels = false;
    setup.useDefaultOutputChannels = true;
    setup.sampleRate = 48000.0;
    setup.bufferSize = 64;

    return deviceManager.setAudioDeviceSetup(setup, true);
}
}
