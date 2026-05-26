#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "../engine/MidiPerformanceEvent.h"
#include "../instrument/PianoInstrument.h"
#include "../instrument/SynthInstrument.h"
#include "../network/MidiWebSocketClient.h"
#include "../session/AudioSession.h"
#include "../state/MidiNoteState.h"
#include "EngineDashboard.h"

class MainComponent : public juce::Component
{
public:
    enum InstrumentChoice
    {
        pianoChoice = 1,
        synthChoice = 2
    };

    MainComponent();
    ~MainComponent() override;

    void resized() override;

private:
    // 상태레이블을 업데이트
    void updateStatusLabels();
    // 선택된 악기의 정보를 변경
    void changeInstrument(int selectedId);
    // 현재 선택된 악기정보를 가져옴
    const Instrument& getCurrentInstrument() const;

    // 피아노와 신스를 가져옴
    PianoInstrument pianoInstrument;
    SynthInstrument synthInstrument;
    // 미디노트의 상태를 가져옴
    MidiNoteState midiNoteState;
    // 오디오 관련 엔진을 불러옴
    AudioSession audioSession;
    // 화면에서 발생한 MIDI 이벤트를 서버용 JSON으로 바꾸는 송신기를 둔다.
    MidiWebSocketClient webSocketClient;
    // 대시보드를 가져옴
    EngineDashboard dashboard;
    int currentInstrumentId = pianoChoice;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
