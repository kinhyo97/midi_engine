#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_formats/juce_audio_formats.h>

class SampleLibrary
{
public:
    SampleLibrary();

    bool isReady() const;
    const juce::String& getLastError() const;
    const juce::ReferenceCountedArray<juce::SynthesiserSound>& getLoadedSounds() const;

private:
    juce::File findPianoAssetsDirectory() const;
    void loadSamples();

    juce::AudioFormatManager formatManager;
    juce::ReferenceCountedArray<juce::SynthesiserSound> loadedSounds;
    bool ready = false;
    juce::String lastError;
};
