#include "MainComponent.h"

MainComponent::MainComponent()
    // audioSession을 통해 피아노를 불러옴
    : audioSession(pianoInstrument, midiNoteState),
      dashboard(midiNoteState)
{
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
