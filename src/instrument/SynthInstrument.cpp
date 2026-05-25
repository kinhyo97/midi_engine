#include "SynthInstrument.h"

namespace
{
// 모든 노트/채널에서 재생 가능한 간단한 사인파 사운드 정의.
class SineWaveSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote(int) override { return true; }
    bool appliesToChannel(int) override { return true; }
};

// 실제 사인파 샘플을 계산해서 출력 버퍼에 더하는 보이스 구현.
class SineWaveVoice : public juce::SynthesiserVoice
{
public:
    bool canPlaySound(juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<SineWaveSound*>(sound) != nullptr;
    }

    void startNote(int midiNoteNumber,
                   float velocity,
                   juce::SynthesiserSound*,
                   int) override
    {
        // 노트 시작 시 위상과 볼륨을 초기화하고 주파수를 계산한다.
        currentAngle = 0.0;
        level = velocity * 0.2;
        tailOff = 0.0;

        const auto cyclesPerSecond = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
        const auto cyclesPerSample = cyclesPerSecond / getSampleRate();
        angleDelta = cyclesPerSample * juce::MathConstants<double>::twoPi;
    }

    void stopNote(float, bool allowTailOff) override
    {
        // 즉시 끊지 않고 릴리즈를 주면 소리가 자연스럽게 감쇠되도록 한다.
        if (allowTailOff)
        {
            if (tailOff == 0.0)
                tailOff = 1.0;
        }
        else
        {
            clearCurrentNote();
            angleDelta = 0.0;
        }
    }

    void pitchWheelMoved(int) override {}
    void controllerMoved(int, int) override {}

    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override
    {
        // 현재 재생 중인 주파수가 없으면 출력할 샘플도 없다.
        if (angleDelta == 0.0)
            return;

        if (tailOff > 0.0)
        {
            // 노트가 끝난 뒤에는 감쇠 계수를 줄여가며 릴리즈 구간을 만든다.
            while (--numSamples >= 0)
            {
                const auto currentSample = static_cast<float>(std::sin(currentAngle) * level * tailOff);

                for (int channel = outputBuffer.getNumChannels(); --channel >= 0;)
                    outputBuffer.addSample(channel, startSample, currentSample);

                currentAngle += angleDelta;
                ++startSample;
                tailOff *= 0.99;

                if (tailOff <= 0.005)
                {
                    clearCurrentNote();
                    angleDelta = 0.0;
                    break;
                }
            }

            return;
        }

        // 노트가 유지 중일 때는 순수 사인파를 계속 출력한다.
        while (--numSamples >= 0)
        {
            const auto currentSample = static_cast<float>(std::sin(currentAngle) * level);

            for (int channel = outputBuffer.getNumChannels(); --channel >= 0;)
                outputBuffer.addSample(channel, startSample, currentSample);

            currentAngle += angleDelta;
            ++startSample;
        }
    }

private:
    double currentAngle = 0.0;
    double angleDelta = 0.0;
    double level = 0.0;
    double tailOff = 0.0;
};
}

juce::String SynthInstrument::getName() const
{
    return "Sine Synth";
}

bool SynthInstrument::isReady() const
{
    return true;
}

const juce::String& SynthInstrument::getLastError() const
{
    return lastError;
}

void SynthInstrument::configureRenderer(juce::Synthesiser& renderer) const
{
    // 간단한 신스 연주를 위해 사인파 보이스 여러 개를 등록한다.
    for (int i = 0; i < 16; ++i)
        renderer.addVoice(new SineWaveVoice());

    // 위 보이스들이 재생할 공통 사운드 타입 하나를 추가한다.
    renderer.addSound(new SineWaveSound());
}
