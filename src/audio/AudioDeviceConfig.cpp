#include "AudioDeviceConfig.h"

namespace
{
// Speaker,Asio4all, Headphones등 장치 이름 문자열을 출력
juce::String findPreferredDeviceName(const juce::StringArray& deviceNames,
                                     const juce::String& preferredSubstring)
{
    // deviceName안에 들어 있는 각 원소를 자동으로 추론
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
    // 디바이스의 타입을 가져옴
    auto& availableTypes = deviceManager.getAvailableDeviceTypes();
    // juce소속의 String으로 preferredTypeName 변수 선언
    juce::String preferredTypeName;

    // 사용가능한 타입을 불러옴
    for (auto* type : availableTypes)
    {
        if (type->getTypeName().containsIgnoreCase("ASIO"))
        {
            preferredTypeName = type->getTypeName();
            break;
        }
    }

    // 사용가능한 타입이 없다면 디바이스 타입을 설정
    if (preferredTypeName.isNotEmpty()
        && deviceManager.getCurrentAudioDeviceType() != preferredTypeName)
    {
        deviceManager.setCurrentAudioDeviceType(preferredTypeName, true);
    }

    // 오디오 디바이스를 셋팅
    auto setup = deviceManager.getAudioDeviceSetup();
    for (auto* type : availableTypes)
    {
        if (type->getTypeName() != deviceManager.getCurrentAudioDeviceType())
            continue;

        type->scanForDevices();

        // 사용가능한 디바이스를 등록
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
