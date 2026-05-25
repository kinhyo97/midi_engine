#include "SampleLibrary.h"

#include "../constant/Note.h"

#include <memory>

SampleLibrary::SampleLibrary()
{
    // WAV/AIFF 같은 기본 오디오 포맷 리더를 등록한 뒤 샘플을 로드한다.
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
    // 현재 작업 디렉터리부터 상위 폴더로 올라가며 assets/wav4416 위치를 찾는다.
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

    // 작업 디렉터리에서 못 찾으면 실행 파일 기준 경로에서도 한 번 더 탐색한다.
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
    // 피아노 샘플이 들어 있는 루트 폴더를 먼저 찾는다.
    const auto pianoDirectory = findPianoAssetsDirectory();

    if (! pianoDirectory.isDirectory())
    {
        ready = false;
        lastError = "Could not find assets/wav4416 directory.";
        return;
    }

    // 88건반 범위의 각 MIDI 노트에 대응하는 샘플 파일을 순회하며 로드한다.
    for (int midiNote = note::piano88Start; midiNote <= note::piano88End; ++midiNote)
    {
        const auto sampleFile = pianoDirectory.getChildFile(
            note::getSampleFileNameForMidiNote(midiNote, ".wav"));

        // 해당 노트 샘플 파일이 없으면 건너뛴다.
        if (! sampleFile.existsAsFile())
            continue;

        std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(sampleFile));

        // JUCE 리더를 만들 수 없는 파일은 사용할 수 없다.
        if (reader == nullptr)
            continue;

        juce::BigInteger noteRange;
        noteRange.setBit(midiNote);

        // 각 샘플 파일을 해당 MIDI 노트에서만 재생되는 SamplerSound로 등록한다.
        loadedSounds.add(new juce::SamplerSound(sampleFile.getFileNameWithoutExtension(),
                                                *reader,
                                                noteRange,
                                                midiNote,
                                                0.0,
                                                0.1,
                                                30.0));
    }

    // 하나도 로드하지 못했으면 피아노 악기를 사용할 수 없는 상태로 남긴다.
    if (loadedSounds.isEmpty())
    {
        ready = false;
        lastError = "No piano samples were loaded from assets/wav4416.";
        return;
    }

    ready = true;
}
