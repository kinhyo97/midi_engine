#include "PianoInstrument.h"

PianoInstrument::PianoInstrument() = default;

juce::String PianoInstrument::getName() const
{
    return "Piano";
}

bool PianoInstrument::isReady() const
{
    // 피아노 샘플이 모두 준비되어 있어야 연주 가능 상태로 본다.
    return sampleLibrary.isReady();
}

const juce::String& PianoInstrument::getLastError() const
{
    // 샘플 로딩 중 발생한 오류 메시지를 그대로 노출한다.
    return sampleLibrary.getLastError();
}

void PianoInstrument::configureRenderer(juce::Synthesiser& renderer) const
{
    // 샘플 재생 전용 보이스를 여러 개 등록해 동시 발음을 처리한다.
    for (int i = 0; i < 32; ++i)
        renderer.addVoice(new juce::SamplerVoice());

    // 미리 로드한 피아노 샘플 사운드들을 렌더러에 연결한다.
    for (const auto& sound : sampleLibrary.getLoadedSounds())
        renderer.addSound(sound);
}
