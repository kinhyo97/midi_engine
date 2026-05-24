#pragma once

#include "Instrument.h"

class SynthInstrument : public Instrument
{
public:
    juce::String getName() const override;
    bool isReady() const override;
    const juce::String& getLastError() const override;
    void configureRenderer(juce::Synthesiser& renderer) const override;

private:
    juce::String lastError;
};
