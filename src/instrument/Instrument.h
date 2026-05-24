#pragma once

#include <juce_audio_basics/juce_audio_basics.h>

class Instrument
{
public:
    virtual ~Instrument() = default;

    virtual juce::String getName() const = 0;
    virtual bool isReady() const = 0;
    virtual const juce::String& getLastError() const = 0;
    virtual void configureRenderer(juce::Synthesiser& renderer) const = 0;
};
