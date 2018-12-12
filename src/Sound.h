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
	ksNoteRest= 0,
	ksNoteA0 	= 1,	 	// Octave 0
	ksNoteAS0 = 2,
	ksNoteB0	= 3,
	ksNoteC0 	= 4,
	ksNoteCS0 = 5,
	ksNoteD0 	= 6,
	ksNoteDS0 = 7,
	ksNoteE0 	= 8,
	ksNoteF0 	= 9,
	ksNoteFS0 = 10,
	ksNoteG0 	= 11,
	ksNoteGS0 = 12,
	ksNoteA1 	= 13,		// Octave 1
	ksNoteAS1 = 14,
	ksNoteB1	= 15,
	ksNoteC1 	= 16,
	ksNoteCS1 = 17,
	ksNoteD1 	= 18,
	ksNoteDS1 = 19,
	ksNoteE1 	= 20,
	ksNoteF1 	= 21,
	ksNoteFS1 = 22,
	ksNoteG1 	= 23,
	ksNoteGS1 = 24,
	ksNoteA2 	= 25,		// Octave 2
	ksNoteAS2 = 26,
	ksNoteB2	= 27,
	ksNoteC2 	= 28,
	ksNoteCS2 = 29,
	ksNoteD2 	= 30,
	ksNoteDS2 = 31,
	ksNoteE2 	= 32,
	ksNoteF2 	= 33,
	ksNoteFS2 = 34,
	ksNoteG2 	= 35,
	ksNoteGS2 = 36,
	ksNoteA3 	= 25,		// Octave 1
	ksNoteAS3 = 26,
	ksNoteB3 	= 27,
	ksNoteC3 	= 28,
	ksNoteCS3 = 29,
	ksNoteD3 	= 30,
	ksNoteDS3 = 31,
	ksNoteE3 	= 32,
	ksNoteF3 	= 33,
	ksNoteFS3 = 34,
	ksNoteG3 	= 35,
	ksNoteGS3 = 36,
	ksNoteA4 	= 37,		// Octave 4
	ksNoteAS4 = 38,
	ksNoteB4 	= 39,
	ksNoteC4 	= 40,
	ksNoteCS4 = 41,
	ksNoteD4 	= 42,
	ksNoteDS4 = 43,
	ksNoteE4 	= 44,
	ksNoteF4 	= 45,
	ksNoteFS4 = 46,
	ksNoteG4 	= 47,
	ksNoteGS4 = 48,
	ksNoteA5 	= 49,
	ksNoteAS5 = 50,
	ksNoteB5 	= 51,
	ksNoteC5 	= 52,		// Octave 5
	ksNoteCS5 = 53,
	ksNoteD5 	= 54,
	ksNoteDS5 = 55,
	ksNoteE5 	= 56,
	ksNoteF5 	= 57,
	ksNoteFS5 = 58,
	ksNoteG5 	= 59,
	ksNoteGS5 = 60,
	ksNoteA6  = 61,		// Octave 6
	ksNoteAS6 = 62,
	ksNoteB6 	= 63,
	ksNoteC6 	= 64,
	ksNoteCS6 = 65,
	ksNoteD6 	= 66,
	ksNoteDS6 = 67,
	ksNoteE6 	= 68,
	ksNoteF6 	= 69,
	ksNoteFS6 = 70,
	ksNoteG6 	= 71,
	ksNoteGS6 = 72,
	ksNoteA7 	= 73,		// Octave 7
	ksNoteAS7 = 74,
	ksNoteB7 	= 75,
	ksNoteC7 	= 76,
	ksNoteCS7 = 77,
	ksNoteD7 	= 78,
	ksNoteDS7 = 79,
	ksNoteE7 	= 80,
	ksNoteF7  = 81,
	ksNoteFS7 = 82,
	ksNoteG7 	= 83,
	ksNoteGS7 = 84,
	ksNoteA8 	= 85,		// Octave 8`
	ksNoteAS8 = 86,
	ksNoteB8 	= 87,
	ksNoteC8 	= 88,

	// Additional codes for a note stream
	ksNoteL0 = 100,		// Sets last note but does play it.
	ksNoteL32nd,
	ksNoteL16th,
	ksNoteL8th,
	ksNoteL4th,
	ksNoteLHalf,
	ksNoteLWhole,
	ksNoteLTriplet,  	// Note length is 1/3 twice the current
	ksNoteLDot,				// Note length is lengthened by 1/2
	ksNoteLFermata,  	// Note length is doubled

	// Envelope modifiers
	ksNoteEStacatto = 110,
	ksNoteELagatto,
	ksNoteEDiatonicTrillo,
	ksNoteEGlissando,	// Pitch Blend from last note.

	ksNoteSetKey,			// All notes will be relative to the new key.
	ksNoteSetTempo,		// In BPM
};

class SoundManager {
private:
	uint8_t _notebuffer[100];
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
	void RunSoundStream();
	void CheckBlend();
	void PluckFrequency(int f);
};

extern SoundManager gSound;
