#include "PianoInstrument.h"

PianoInstrument::PianoInstrument() = default;

juce::String PianoInstrument::getName() const
{
    return "Piano";
}

bool PianoInstrument::isReady() const
{
    return sampleLibrary.isReady();
}

const juce::String& PianoInstrument::getLastError() const
{
    return sampleLibrary.getLastError();
}

void PianoInstrument::configureRenderer(juce::Synthesiser& renderer) const
{
    for (int i = 0; i < 32; ++i)
        renderer.addVoice(new juce::SamplerVoice());

    for (const auto& sound : sampleLibrary.getLoadedSounds())
        renderer.addSound(sound);
}
