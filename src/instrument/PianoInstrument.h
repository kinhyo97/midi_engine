#pragma once

#include "Instrument.h"

#include "../sample/SampleLibrary.h"

class PianoInstrument : public Instrument
{
public:
    PianoInstrument();

    juce::String getName() const override;
    bool isReady() const override;
    const juce::String& getLastError() const override;
    void configureRenderer(juce::Synthesiser& renderer) const override;

private:
    SampleLibrary sampleLibrary;
};
