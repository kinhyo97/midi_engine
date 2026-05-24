#include "SynthInstrument.h"

namespace
{
class SineWaveSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote(int) override { return true; }
    bool appliesToChannel(int) override { return true; }
};

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
        currentAngle = 0.0;
        level = velocity * 0.2;
        tailOff = 0.0;

        const auto cyclesPerSecond = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
        const auto cyclesPerSample = cyclesPerSecond / getSampleRate();
        angleDelta = cyclesPerSample * juce::MathConstants<double>::twoPi;
    }

    void stopNote(float, bool allowTailOff) override
    {
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
        if (angleDelta == 0.0)
            return;

        if (tailOff > 0.0)
        {
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
    for (int i = 0; i < 16; ++i)
        renderer.addVoice(new SineWaveVoice());

    renderer.addSound(new SineWaveSound());
}
