#include "MainComponent.h"

MainComponent::MainComponent()
    // audioSession을 통해 피아노를 불러옴
    : audioSession(pianoInstrument, midiNoteState),
      dashboard(midiNoteState)
{
    // 화면이 열릴 때 연주 이벤트를 서버로 보낼 수 있도록 웹소켓 연결을 준비한다.
    webSocketClient.setEndpoint("ws://127.0.0.1:8080/midi");

    if (!webSocketClient.connect())
        juce::Logger::writeToLog("[MidiWebSocketClient] connect failed: " + webSocketClient.getLastError());

    audioSession.getEngine().onMidiEvent = [this](const MidiPerformanceEvent& event)
    {
        webSocketClient.sendMidiEvent(event);
    };

    dashboard.setInstrumentOptions(pianoInstrument.getName(), synthInstrument.getName());
    dashboard.setSelectedInstrumentId(currentInstrumentId);
    dashboard.setInstrumentSelectedCallback([this](int selectedId) { changeInstrument(selectedId); });

    addAndMakeVisible(dashboard);

    if (!audioSession.start())
        updateStatusLabels();

    setSize(1080, 560);
    updateStatusLabels();
}

MainComponent::~MainComponent()
{
    // 화면이 사라질 때 더 이상 네트워크 송신 시도를 하지 않도록 연결을 끊는다.
    audioSession.getEngine().onMidiEvent = nullptr;
    webSocketClient.disconnect();
}

void MainComponent::resized()
{
    dashboard.setBounds(getLocalBounds());
}

void MainComponent::updateStatusLabels()
{
    dashboard.setStatus(audioSession.getStatus(), getCurrentInstrument().getName());
}

const Instrument& MainComponent::getCurrentInstrument() const
{
    return currentInstrumentId == synthChoice
               ? static_cast<const Instrument&>(synthInstrument)
               : static_cast<const Instrument&>(pianoInstrument);
}

// 악기 변경 
void MainComponent::changeInstrument(int selectedId)
{
    currentInstrumentId = selectedId;
    audioSession.setInstrument(getCurrentInstrument());
    updateStatusLabels();
}
