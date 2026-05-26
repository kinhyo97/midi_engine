# MIDI Engine Server

실시간 MIDI relay/gateway 실험용 C++ 서버입니다.

현재 상태:

- WebSocket transport는 아직 붙이지 않았습니다.
- 대신 콘솔 기반 mock client로 performer/listener 흐름을 검증할 수 있습니다.
- 아래 기능을 독립적으로 구현해 두었습니다.
  - room 단위 performer/listener 관리
  - MIDI JSON payload 파싱
  - 이벤트 검증
  - sequence 순서 검사
  - active note 상태 추적
  - late joiner snapshot 전송
  - listener 브로드캐스트
  - backend bridge 훅

## Build

```powershell
cmake -S server -B build-server
cmake --build build-server --config Debug
```

## Console Commands

```text
help
performer <roomId> <clientId>
listener <roomId> <clientId>
send <clientId> <json>
drop <clientId>
rooms
quit
```

예시:

```text
performer room-a performer-1
listener room-a listener-1
send performer-1 {"seq":1,"type":"noteOn","note":60,"velocity":0.8,"eventTime":12.35}
send performer-1 {"seq":2,"type":"noteOff","note":60,"velocity":0.0,"eventTime":12.70}
rooms
```
