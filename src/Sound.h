/*
Copyright (c) 2018 Trashbots, Inc. - SDG

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "Value.h"

enum NoteEnvelope {
	kNELegato,	 	// Note played 100% of duration
	kNEStecato,  	// Note played 50% of duration
	kNEMarcato,	 	// Note played 66% of duration
	kNEGlissando, 	// note played 100% with pitch blend from previous note.
};

enum SolfegeNotes{
	kSNoteRest,
	ksNoteA3 = 37,
	ksNoteASharp3 = 38,
	ksNoteB3 = 39,
	ksNoteC4 = 40,
	ksNoteD4 = 42,
	ksNoteE4 = 44,
	ksNoteF4 = 45,
	ksNoteG4 = 47,
	ksNoteA5 = 49,
	ksNoteB5 = 51,
	ksNoteC5 = 52,
	ksNoteD5 = 54,
	ksNoteE5 = 56,
	ksNoteF5 = 57,
	ksNoteA6 = 59,
	ksNoteB6 = 61,
	ksNoteC6 = 62,
	ksNoteD6 = 64,
	ksNoteE6 = 66,
	ksNoteF6 = 67,
	ksNoteA7 = 69,
	ksNoteB7 = 71,
	ksNoteC7 = 72,
	ksNoteD7 = 74,
	ksNoteE7 = 76,
	ksNoteF7 = 77,
	ksNoteA8 = 79,
	ksNoteB8 = 81,

	// Other code for a Note stream
	ksNoteL0 = 100,		// Sets last-note but does not play it.
	ksNoteL16th,
	ksNoteL8th,
	ksNoteL4th,
	ksNoteLHalf,
	ksNoteLWhole,
	ksNoteLTriplet,  	// Note length is 1/3 twice the current
	ksNoteLDot,			// Note length is lengthened by 1/2
	ksNoteLFermata,  	// Note length is doubled

	// Envelope modifiers
	ksNoteStacatto = 110,
	ksNoteLagatto,
	ksNoteMarcato,
	ksNoteGlissando,	// Pitch Blend from last note.
	ksNoteSetKey,		// All notes will be relative to the new key.
	ksNoteSetTempo,		// In BPM in following byte
};

class SoundManager {
private:
	uint8_t _notebuffer[64];
private:
	FiFoValue _noteStream;
	Value 	_noteTempo;
	Value 	_noteLength;
	Value 	_noteSolfege;
	Value 	_noteHertz;
	Value 	_noteStatus;
	int 	_blendFrequency;
	int 	_targetFrequency;
	int 	_noteBeatsRemaining;

public:

	SoundManager() :_noteStream(_notebuffer, sizeof(_notebuffer))
	{
		_blendFrequency = 0;
		_targetFrequency = 0;
		_noteBeatsRemaining = 0;
	}

	void Init();
	void Run();
	void CheckBlend();
	void PluckFrequency(int f);
};

extern SoundManager gSound;
