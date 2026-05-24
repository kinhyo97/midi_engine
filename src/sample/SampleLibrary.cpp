#include "SampleLibrary.h"

#include "../constant/Note.h"

#include <memory>

SampleLibrary::SampleLibrary()
{
    formatManager.registerBasicFormats();
    loadSamples();
}

bool SampleLibrary::isReady() const
{
    return ready;
}

const juce::String& SampleLibrary::getLastError() const
{
    return lastError;
}

const juce::ReferenceCountedArray<juce::SynthesiserSound>& SampleLibrary::getLoadedSounds() const
{
    return loadedSounds;
}

juce::File SampleLibrary::findPianoAssetsDirectory() const
{
    auto current = juce::File::getCurrentWorkingDirectory();

    for (int depth = 0; depth < 8; ++depth)
    {
        const auto candidate = current.getChildFile("assets").getChildFile("wav4416");

        if (candidate.isDirectory())
            return candidate;

        const auto parent = current.getParentDirectory();

        if (parent == current)
            break;

        current = parent;
    }

    current = juce::File::getSpecialLocation(juce::File::currentExecutableFile).getParentDirectory();

    for (int depth = 0; depth < 8; ++depth)
    {
        const auto candidate = current.getChildFile("assets").getChildFile("wav4416");

        if (candidate.isDirectory())
            return candidate;

        const auto parent = current.getParentDirectory();

        if (parent == current)
            break;

        current = parent;
    }

    return {};
}

void SampleLibrary::loadSamples()
{
    const auto pianoDirectory = findPianoAssetsDirectory();

    if (! pianoDirectory.isDirectory())
    {
        ready = false;
        lastError = "Could not find assets/wav4416 directory.";
        return;
    }

    for (int midiNote = note::piano88Start; midiNote <= note::piano88End; ++midiNote)
    {
        const auto sampleFile = pianoDirectory.getChildFile(
            note::getSampleFileNameForMidiNote(midiNote, ".wav"));

        if (! sampleFile.existsAsFile())
            continue;

        std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(sampleFile));

        if (reader == nullptr)
            continue;

        juce::BigInteger noteRange;
        noteRange.setBit(midiNote);

        loadedSounds.add(new juce::SamplerSound(sampleFile.getFileNameWithoutExtension(),
                                                *reader,
                                                noteRange,
                                                midiNote,
                                                0.0,
                                                0.1,
                                                30.0));
    }

    if (loadedSounds.isEmpty())
    {
        ready = false;
        lastError = "No piano samples were loaded from assets/wav4416.";
        return;
    }

    ready = true;
}
