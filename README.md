# midi_engine

JUCE 기반의 MIDI 입력 테스트 프로젝트입니다.
외부 MIDI 키보드 입력을 받아 오디오를 재생하고, 현재 눌린 노트를 피아노 키보드 UI에 반영합니다.

## What It Does

- MIDI 입력 장치를 열어 `noteOn` / `noteOff` 이벤트를 받습니다.
- `juce::Synthesiser` 기반 엔진으로 실제 오디오를 출력합니다.
- 샘플 기반 피아노와 간단한 사인파 신스를 전환할 수 있습니다.
- 현재 눌린 건반 상태를 UI에서 실시간으로 표시합니다.

## Project Flow

전체 흐름은 아래와 같습니다.

`main.cpp`
- 앱 시작점입니다.
- `MainWindow`를 만들고 `MainComponent`를 화면에 올립니다.

`MainComponent`
- 프로젝트의 조립 지점입니다.
- `PianoInstrument`, `SynthInstrument`, `MidiNoteState`, `AudioSession`, `EngineDashboard`를 묶습니다.

`AudioSession`
- 오디오 디바이스와 MIDI 입력 장치를 연결합니다.
- `MusicEngine`을 오디오 콜백과 MIDI 입력 콜백에 붙입니다.

`MusicEngine`
- 들어온 MIDI 메시지에 타임스탬프를 붙여 큐에 넣습니다.
- `juce::Synthesiser`를 통해 실제 오디오를 렌더링합니다.
- `MidiNoteState`를 갱신해서 UI와 상태를 공유합니다.

`Instrument`
- 악기 공통 인터페이스입니다.
- 현재는 두 구현이 있습니다.
  - `PianoInstrument`: 샘플 기반 피아노
  - `SynthInstrument`: 사인파 기반 간단한 신스

`SampleLibrary`
- `assets/wav4416`의 피아노 샘플을 로드합니다.
- MIDI 노트별 `juce::SamplerSound`를 만들어 피아노 렌더러에 공급합니다.

`MidiNoteState`
- 현재 어떤 MIDI 노트가 눌려 있는지 저장합니다.
- UI가 이 상태를 읽어 피아노 건반 표시를 갱신합니다.

`EngineDashboard` / `PianoKeyboardView`
- 현재 장치 상태, 선택된 악기, 피아노 건반 UI를 보여줍니다.

## Directory Guide

```text
src/
  audio/       오디오 장치 설정
  constant/    노트 번호/파일명 관련 상수
  engine/      MIDI 처리와 오디오 렌더링 엔진
  instrument/  악기 구현체
  sample/      샘플 로딩
  session/     오디오/MIDI 세션 관리
  state/       UI와 엔진이 공유하는 상태
  ui/          화면 구성 요소
assets/
  wav4416/     피아노 샘플 WAV 파일
external/JUCE/ JUCE 서브모듈
```

## Build

기본 전제:

- CMake 3.22+
- C++17 지원 컴파일러
- JUCE 서브모듈 초기화

예시:

```powershell
git submodule update --init --recursive
cmake -S . -B build-msvc
cmake --build build-msvc --config Release
```

## Current Status

- 샘플 피아노 재생 가능
- 사인파 신스 재생 가능
- MIDI 입력 상태 UI 반영 가능
- 청취자 동기화 관련 설계는 별도 로컬 문서로 관리 중
