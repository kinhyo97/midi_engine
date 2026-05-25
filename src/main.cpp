#include <juce_gui_basics/juce_gui_basics.h>

#include "ui/MainComponent.h"

class MidiEngineApplication : public juce::JUCEApplication
{
public:
    const juce::String getApplicationName() override { return "midi_engine"; }
    const juce::String getApplicationVersion() override { return "0.1.0"; }
    bool moreThanOneInstanceAllowed() override { return true; }

    void initialise(const juce::String&) override
    {
        mainWindow = std::make_unique<MainWindow>(getApplicationName());
    }

    void shutdown() override
    {
        mainWindow.reset();
    }

    void systemRequestedQuit() override
    {
        quit();
    }

    void anotherInstanceStarted(const juce::String&) override
    {
    }

private:
    class MainWindow : public juce::DocumentWindow
    {
    public:
        // 생성자가 자동 형변환에 사용되는 걸 막고 MainWindow를 객체를 생성
        explicit MainWindow(juce::String name)
            : juce::DocumentWindow(std::move(name),
                                   juce::Colours::black,
                                   juce::DocumentWindow::allButtons)
        {
            // 타이틀 바 설정
            setUsingNativeTitleBar(true);
            // MainWindow안에 MainComponent 객체를 생성해서 사용
            setContentOwned(new MainComponent(), true);
            // 현재 창 크기를 기준으로 화면 중앙에 배치
            centreWithSize(getWidth(), getHeight());
            // 사용자가 창 크기를 조절할 수 있게
            setResizable(true, true);
            // 창을 보여줌
            setVisible(true);
        }

        // 종료눌렀을때 Juce를 정리하고 종료
        void closeButtonPressed() override
        {
            juce::JUCEApplication::getInstance()->systemRequestedQuit();
        }
    };

    std::unique_ptr<MainWindow> mainWindow;
};

START_JUCE_APPLICATION(MidiEngineApplication)
