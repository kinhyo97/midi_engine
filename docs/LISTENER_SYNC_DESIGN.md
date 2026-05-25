# Listener Sync Design

## Goal

Performers generate MIDI `noteOn` and `noteOff` events in the JUCE app.
Those events are sent to a server and broadcast to listener browsers.

Listeners may hear the performance slightly later than real time.
However, inside each listener browser:

- piano audio playback must stay rhythmically stable
- piano key visuals must match the sound timing

The main requirement is not zero latency.
The main requirement is stable timing and audio/visual sync on the listener side.

## Core Idea

Do not play incoming events immediately when they arrive over the network.

Network delay changes from packet to packet, so arrival time is not a reliable playback clock.
Instead:

1. the performer sends each MIDI event with its original event timestamp
2. the listener builds a local playback timeline from those timestamps
3. the listener uses a fixed buffer delay before starting playback
4. both audio and visuals are scheduled from the same listener-side timeline

This means:

- network delivery only transports the events
- playback timing is decided by the listener's own scheduled timeline

## Why This Is Needed

If the listener plays notes as soon as packets arrive:

- packet jitter changes note spacing
- rhythm becomes unstable
- visuals and sound can drift apart

If the listener instead uses performer timestamps plus a local buffer:

- note spacing stays consistent
- timing feels musically stable
- sound and key visuals can be synchronized

## Performer Side

The performer client should send MIDI events with a monotonic timestamp captured at the time of input.

Example payload:

```json
{
  "type": "noteOn",
  "note": 60,
  "velocity": 0.8,
  "eventTime": 12.350
}
```

Recommended notes:

- `eventTime` should come from a monotonic clock
- do not use wall-clock time like `Date.now()` as the main musical timestamp
- in the JUCE app, a clock like `juce::Time::getMillisecondCounterHiRes()` is appropriate

## Server Side

The server should broadcast performer events to all connected listeners.

The server does not need to retime the performance for the first version.
Its main job is to relay the event stream.

Recommended additions:

- include a sequence number for ordering safety
- optionally provide current pressed-note state to late joiners

Example extended payload:

```json
{
  "seq": 1842,
  "type": "noteOn",
  "note": 60,
  "velocity": 0.8,
  "eventTime": 12.350
}
```

## Listener Side Timeline

The listener must not play events immediately on receive.

When the first event arrives:

- `firstEventTime = event.eventTime`
- `firstAudioTime = audioContext.currentTime + LISTENER_DELAY`

For every later event:

```js
delta = event.eventTime - firstEventTime
playTime = firstAudioTime + delta
```

Meaning:

- `eventTime` preserves the performer's musical spacing
- `LISTENER_DELAY` creates a safe buffer against network jitter
- `playTime` becomes the listener's scheduled playback time

## Audio Scheduling

Listener audio should be scheduled using `playTime`.

Conceptually:

```js
scheduleAudio(event, playTime)
```

This allows the browser to play notes from a stable local timeline rather than packet arrival time.

## Visual Scheduling

Listener visuals must use the same musical timeline as audio.

Conceptually:

```js
scheduleKeyVisual(event, visualTime)
```

The important rule is:

- do not drive visuals from packet arrival time
- do not use `setTimeout` as the primary sync source
- use the Web Audio time axis as the main reference

## Output Latency Correction

There is another delay besides network delay:

- browser audio output latency

Even if audio is scheduled at `playTime`, the user may hear it slightly later at the speaker output stage.

Because of that, visual timing may need an extra offset:

```js
visualTime = playTime + estimatedOutputLatency
```

Where `estimatedOutputLatency` is based on:

- `audioContext.outputLatency` if available
- otherwise `audioContext.baseLatency`
- otherwise a fallback estimate

This gives a more accurate sync between:

- what the user hears
- what the user sees on the piano keyboard

## Listener Render Loop

The listener should evaluate visual events against the audio clock.

Conceptually:

```js
const LISTENER_DELAY = 0.3;
let firstEventTime = null;
let firstAudioTime = null;
const visualQueue = [];

function handleMidiEvent(event) {
  if (firstEventTime === null) {
    firstEventTime = event.eventTime;
    firstAudioTime = audioContext.currentTime + LISTENER_DELAY;
  }

  const delta = event.eventTime - firstEventTime;
  const playTime = firstAudioTime + delta;
  const visualTime = playTime + getEstimatedOutputLatency();

  scheduleAudio(event, playTime);

  visualQueue.push({
    type: event.type,
    note: event.note,
    time: visualTime,
    done: false
  });
}

function renderLoop() {
  const audioNow = audioContext.currentTime;

  for (const event of visualQueue) {
    if (!event.done && audioNow >= event.time) {
      if (event.type === "noteOn") {
        pressKeyVisual(event.note);
      } else if (event.type === "noteOff") {
        releaseKeyVisual(event.note);
      }

      event.done = true;
    }
  }

  requestAnimationFrame(renderLoop);
}
```

## Late Packet Handling

Some packets may arrive too late.

If:

- `playTime` is already in the past
- or the remaining time before playback is too small

Then the listener should not blindly play the note immediately.

Recommended policy for first implementation:

- if an event misses the scheduling window, drop it
- if late packets happen too often, reset sync and rebuild timeline

Without this rule, late arrivals can damage rhythm and sync.

## Late Joiners

A listener may join while some notes are already being held.

If the listener only receives future events, it may get a `noteOff` without ever seeing the earlier `noteOn`.

Recommended improvement:

- server sends a snapshot of currently active notes when a new listener joins

This is not required for the first prototype, but it is important for correctness.

## Choosing Between Audio Streaming and Browser Playback

For this project, the recommended first approach is:

- send MIDI events
- play piano sound in the browser

Reasons:

- audio and visuals can share the same Web Audio timeline
- bandwidth is much smaller than full audio streaming
- timing control is simpler
- listener-side buffering works naturally with MIDI events

Direct audio streaming may be better only if these are critical:

- the listener must hear exactly the same rendered sound as the performer
- pedal resonance, release behavior, and effects must match perfectly
- browser-side sample playback is not acceptable

## Recommended First Version

1. performer sends `noteOn` and `noteOff` with `eventTime`
2. server broadcasts those events
3. listener creates a local timeline using `firstEventTime` and `firstAudioTime`
4. listener schedules audio from `playTime`
5. listener schedules visuals from `playTime + outputLatencyEstimate`
6. listener uses `requestAnimationFrame` only as a render loop, with `audioContext.currentTime` as the true timing reference

## Key Summary

- network arrival time must not be the playback clock
- performer timestamps define musical spacing
- listener delay absorbs jitter
- audio uses scheduled `playTime`
- visuals use the same timeline, plus output latency correction if needed
- browser-side audio playback is the recommended first architecture for this system
