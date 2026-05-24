#include "PianoKeyboardView.h"

#include <array>

namespace
{
constexpr int firstPianoMidiNote = 21; // A0
constexpr int lastPianoMidiNote = 108; // C8

std::array<int, 88> buildWhiteKeyIndexes()
{
    std::array<int, 88> indexes {};
    int whiteIndex = 0;

    for (int midiNote = firstPianoMidiNote; midiNote <= lastPianoMidiNote; ++midiNote)
    {
        const int pitchClass = midiNote % 12;
        const bool isBlack = pitchClass == 1 || pitchClass == 3 || pitchClass == 6 || pitchClass == 8 || pitchClass == 10;

        indexes[static_cast<size_t>(midiNote - firstPianoMidiNote)] = whiteIndex;

        if (!isBlack)
            ++whiteIndex;
    }

    return indexes;
}

const auto whiteKeyIndexes = buildWhiteKeyIndexes();
constexpr int totalWhiteKeys = 52;
}

PianoKeyboardView::PianoKeyboardView(MidiNoteState& midiNoteStateToUse)
    : midiNoteState(midiNoteStateToUse)
{
    midiNoteState.addChangeListener(this);
}

PianoKeyboardView::~PianoKeyboardView()
{
    midiNoteState.removeChangeListener(this);
}

void PianoKeyboardView::paint(juce::Graphics& g)
{
    auto area = getLocalBounds().toFloat();
    area.reduce(0.0f, 4.0f);

    g.setColour(juce::Colour(0xff0b0d0f));
    g.fillRoundedRectangle(area, 14.0f);

    const auto whiteKeyWidth = area.getWidth() / static_cast<float>(totalWhiteKeys);
    const auto blackKeyWidth = whiteKeyWidth * 0.62f;
    const auto blackKeyHeight = area.getHeight() * 0.62f;

    for (int midiNote = firstPianoMidiNote; midiNote <= lastPianoMidiNote; ++midiNote)
    {
        if (isBlackKey(midiNote))
            continue;

        const auto keyIndex = whiteKeyIndexes[static_cast<size_t>(midiNote - firstPianoMidiNote)];
        auto keyBounds = juce::Rectangle<float>(area.getX() + whiteKeyWidth * static_cast<float>(keyIndex),
                                                area.getY(),
                                                whiteKeyWidth,
                                                area.getHeight());

        const auto isActive = midiNoteState.isNoteActive(midiNote);
        g.setColour(isActive ? juce::Colour(0xfff0c674) : juce::Colour(0xfff4f4f0));
        g.fillRect(keyBounds);

        g.setColour(juce::Colour(0xff171a1d));
        g.drawRect(keyBounds, 1.0f);

        if (keyBounds.getWidth() > 18.0f)
        {
            g.setColour(isActive ? juce::Colour(0xff6b4f13) : juce::Colour(0xff8d9399));
            g.setFont(juce::FontOptions(11.0f));
            g.drawText(getNoteName(midiNote), keyBounds.removeFromBottom(18.0f), juce::Justification::centred);
        }
    }

    for (int midiNote = firstPianoMidiNote; midiNote <= lastPianoMidiNote; ++midiNote)
    {
        if (!isBlackKey(midiNote))
            continue;

        const auto keyIndex = whiteKeyIndexes[static_cast<size_t>(midiNote - firstPianoMidiNote)];
        const auto x = area.getX() + whiteKeyWidth * static_cast<float>(keyIndex) - (blackKeyWidth * 0.5f);
        const auto keyBounds = juce::Rectangle<float>(x, area.getY(), blackKeyWidth, blackKeyHeight);

        const auto isActive = midiNoteState.isNoteActive(midiNote);
        g.setColour(isActive ? juce::Colour(0xffd88f2d) : juce::Colour(0xff1c2127));
        g.fillRoundedRectangle(keyBounds, 4.0f);

        g.setColour(isActive ? juce::Colour(0xffffd48a) : juce::Colour(0xff414954));
        g.drawRoundedRectangle(keyBounds, 4.0f, 1.0f);
    }
}

void PianoKeyboardView::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    if (source == &midiNoteState)
        repaint();
}

bool PianoKeyboardView::isBlackKey(int midiNoteNumber) const
{
    switch (midiNoteNumber % 12)
    {
        case 1:
        case 3:
        case 6:
        case 8:
        case 10:
            return true;
        default:
            return false;
    }
}

juce::String PianoKeyboardView::getNoteName(int midiNoteNumber) const
{
    return juce::MidiMessage::getMidiNoteName(midiNoteNumber, true, true, 3);
}
