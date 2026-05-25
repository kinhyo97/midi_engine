#include "EngineDashboard.h"

namespace
{
// 상태 표시용 라벨들의 공통 정렬/폰트 스타일을 맞춘다.
void configureInfoLabel(juce::Label& label)
{
    label.setJustificationType(juce::Justification::centredLeft);
    label.setFont(juce::FontOptions(16.0f));
}
}

EngineDashboard::EngineDashboard(MidiNoteState& midiNoteState)
    : pianoKeyboardView(midiNoteState)
{
    // 상단 타이틀 라벨 스타일을 설정한다.
    titleLabel.setText("MIDI Engine", juce::dontSendNotification);
    titleLabel.setJustificationType(juce::Justification::centredLeft);
    titleLabel.setFont(juce::FontOptions(28.0f));

    // 상태 메시지 라벨은 본문보다 조금 작은 폰트로 표시한다.
    statusLabel.setJustificationType(juce::Justification::centredLeft);
    statusLabel.setFont(juce::FontOptions(15.0f));

    // 오디오/MIDI 정보 라벨들에 공통 스타일을 적용한다.
    configureInfoLabel(audioTypeLabel);
    configureInfoLabel(deviceNameLabel);
    configureInfoLabel(sampleRateLabel);
    configureInfoLabel(bufferSizeLabel);
    configureInfoLabel(latencyLabel);
    configureInfoLabel(instrumentLabel);
    configureInfoLabel(midiLabel);

    // 악기 선택이 바뀌면 외부에서 등록한 콜백으로 선택 ID를 전달한다.
    instrumentSelector.onChange = [this]
    {
        if (onInstrumentSelected)
            onInstrumentSelected(instrumentSelector.getSelectedId());
    };

    // 대시보드에 필요한 모든 UI 컴포넌트를 화면에 등록한다.
    addAndMakeVisible(titleLabel);
    addAndMakeVisible(audioTypeLabel);
    addAndMakeVisible(deviceNameLabel);
    addAndMakeVisible(sampleRateLabel);
    addAndMakeVisible(bufferSizeLabel);
    addAndMakeVisible(latencyLabel);
    addAndMakeVisible(instrumentLabel);
    addAndMakeVisible(instrumentSelector);
    addAndMakeVisible(midiLabel);
    addAndMakeVisible(statusLabel);
    addAndMakeVisible(pianoKeyboardView);
}

void EngineDashboard::setInstrumentOptions(const juce::String& primaryName, const juce::String& secondaryName)
{
    // 악기 선택 콤보박스를 현재 제공되는 악기 목록으로 다시 채운다.
    instrumentSelector.clear();
    instrumentSelector.addItem(primaryName, 1);
    instrumentSelector.addItem(secondaryName, 2);
}

void EngineDashboard::setSelectedInstrumentId(int selectedId)
{
    // 외부 상태에 맞춰 콤보박스 선택값만 조용히 동기화한다.
    instrumentSelector.setSelectedId(selectedId, juce::dontSendNotification);
}

void EngineDashboard::setStatus(const AudioSession::Status& status, const juce::String& instrumentName)
{
    // 세션에서 받아온 오디오/MIDI 상태 문자열을 각 라벨에 반영한다.
    audioTypeLabel.setText("Audio type: " + status.audioDeviceType, juce::dontSendNotification);
    deviceNameLabel.setText("Audio device: " + status.audioDeviceName, juce::dontSendNotification);
    sampleRateLabel.setText("Sample rate: " + status.sampleRate, juce::dontSendNotification);
    bufferSizeLabel.setText("Buffer size: " + status.bufferSize, juce::dontSendNotification);
    latencyLabel.setText("Latency: " + status.latency, juce::dontSendNotification);
    instrumentLabel.setText("Instrument: " + instrumentName, juce::dontSendNotification);
    midiLabel.setText(status.midiInputName.isEmpty() ? "MIDI input: none" : "MIDI input: " + status.midiInputName,
                      juce::dontSendNotification);
    statusLabel.setText(status.message, juce::dontSendNotification);
}

void EngineDashboard::setInstrumentSelectedCallback(InstrumentSelectedCallback callback)
{
    // MainComponent가 악기 변경 로직을 연결할 수 있도록 콜백을 저장한다.
    onInstrumentSelected = std::move(callback);
}

void EngineDashboard::paint(juce::Graphics& g)
{
    // 대시보드 전체 배경과 내부 카드형 패널을 그린다.
    g.fillAll(juce::Colour(0xff111315));

    g.setColour(juce::Colour(0xff1d2227));
    g.fillRoundedRectangle(getLocalBounds().toFloat().reduced(16.0f), 18.0f);

    g.setColour(juce::Colour(0xff2f3942));
    g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(16.0f), 18.0f, 1.5f);
}

void EngineDashboard::resized()
{
    // 상단 정보 영역과 하단 피아노 키보드 영역으로 나눠 배치한다.
    auto area = getLocalBounds().reduced(32, 24);

    titleLabel.setBounds(area.removeFromTop(42));
    area.removeFromTop(12);

    const int rowHeight = 28;
    audioTypeLabel.setBounds(area.removeFromTop(rowHeight));
    deviceNameLabel.setBounds(area.removeFromTop(rowHeight));
    sampleRateLabel.setBounds(area.removeFromTop(rowHeight));
    bufferSizeLabel.setBounds(area.removeFromTop(rowHeight));
    latencyLabel.setBounds(area.removeFromTop(rowHeight));
    instrumentLabel.setBounds(area.removeFromTop(rowHeight));
    instrumentSelector.setBounds(area.removeFromTop(32).removeFromLeft(240));
    area.removeFromTop(4);
    midiLabel.setBounds(area.removeFromTop(rowHeight));

    area.removeFromTop(10);
    statusLabel.setBounds(area.removeFromTop(rowHeight));
    area.removeFromTop(14);
    pianoKeyboardView.setBounds(area);
}
