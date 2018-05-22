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

#include "Hardware.h"
#include "Sound.h"
#include "SpiRegisterMap.h"

SoundManager gSound;

// Eight full octave 96 notes + rest(0)
int sNotesChromatic[] =
	{0,
	// A0 - G#1
	 28,   29,    31,   33,   35,   37,   39,   41,   44,   46,   49,   52,
	// A1 - G#2
	 55,   58,    62,   65,   70,   73,   78,   82,   87,   92,   98,  104,
	// A2 - G#3
	110,   117,  123,  131,  139,  147,  156,  165,  175,  185,  196,  208,
	// A3 - G#4
	220,   233,  247,  262,  277,  294,  311,  330,  349,  370,  392,  415,
	// A4 - G#5
	440,   466,  494,  523,  554,  587,  622,  659,  698,  740,  784,  830,
	// A5 - G#6
	880,   932,  988, 1047, 1109, 1175, 1245, 1319, 1397, 1480, 1568, 1661,
	// A6 - G#7
	1760, 1865, 1976, 2093, 2217, 2349, 2489, 2637, 2794, 2960, 3136, 3322,
	// A7 - G#8 (C8-4186 is highest note on standard 88 key keyboard)
	3520, 3729, 3951, 4186, 4435, 4699, 4978, 5274, 5588, 5920, 6272, 6645,
	};


void SoundManager::Init(void)
{
	// Default 60 BPM
	noteTempo.Set(60);
	noteTempo.Reset();
	// Length of note in 16th notes
	noteLength.Set(125);
	noteLength.Reset();

	// Turn on op-amp for speaker
	GPIO_Write(SPK_EN, 1);

	gRMap.SetValueObj(kRM_NoteTempo, &noteTempo);
	gRMap.SetValueObj(kRM_NoteLength, &noteLength);
	gRMap.SetValueObj(kRM_NoteSolfege, &noteSolfege);
	gRMap.SetValueObj(kRM_NoteHertz, &noteHertz);
}

void SoundManager::PluckFrequency(int f)
{
	// Start a new note
	_noteTimeRemaining = noteLength.Get();

	if (f > 0 && f <= 5000) {
		HW_Timer1_SetFreq(f);
		HW_Timer1_Enable(true);  // Start Playing
	} else {
		// Quiet last for the beat duration as well
		HW_Timer1_SetFreq(0);
		HW_Timer1_Enable(false);  // Start Playing
	}
}
void SoundManager::Run(void)
{
	if (noteHertz.HasAsyncSet()) {
		PluckFrequency(noteHertz.Get());
	} else if (noteSolfege.HasAsyncSet()) {
		int n = noteSolfege.Get();
		if (n < 0 || n >= COUNT_OF(sNotesChromatic))
			n = 0;
		PluckFrequency(sNotesChromatic[n]);
	}

	if (_noteTimeRemaining > 0 ) {
		_noteTimeRemaining--;

		if (_noteTimeRemaining == 0) {
			noteHertz.ASet(0);
			noteSolfege.Set(0);
			HW_Timer1_SetFreq(0);
			HW_Timer1_Enable(false);  // Start Playing
		}
	}
}

