//============================================================================
//  Title	: Beep.c
//  Desc	: Beep Functions
//  2017-05-23	Daraius		Created
//============================================================================


#include "Hardware.h"
#include "Sound.h"
#include "SpiRegisterMap.h"


SoundManager gSound;

int noteBeatsRemaining = 0;

int sNotes[] =
	{16,  18,  21,  22,  25,
	 28,    31,   33,   37,   41,   44,   49,
	 55,    62,   65,   73,   82,   87,   98,
	110,   123,  131,  147,  165,  175,  196,
	220,   247,  262,  294,  330,  349,  392,
	440,   494,  523,  587,  659,  698,  784,
	880,   988, 1047, 1175, 1319, 1397, 1568,
	1760, 1976, 2093, 2349, 2637, 2794, 3136,
	3520, 3951, 4186, 4699, 5274, 5588, 6272,
	7040, 7902};

/* 16,   17,    18, 	19,   21,  	22,   23,   25,   26, */


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
	// Length of note in 16th notes
	noteLength.Set(125);

	noteLength.Updated();
	noteTempo.Updated();

	gRMap.SetValueObj(kRM_NoteTempo, &noteTempo);
	gRMap.SetValueObj(kRM_NoteLength, &noteLength);
	gRMap.SetValueObj(kRM_NoteSolfege, &noteSolfege);
	gRMap.SetValueObj(kRM_NoteHertz, &noteHertz);
}

void SoundManager::PluckFrequency(int f)
{
	// Start a new note
	noteBeatsRemaining = noteLength.Read();

	if (f > 0 && f <= 10000) {
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
	if (noteHertz.Updated()) {
		PluckFrequency(noteHertz.Read());
	} else if (noteSolfege.Updated()) {
		int n = noteSolfege.Read();
		if (n < 0 || n >= COUNT_OF(sNotesChromatic))
			n = 0;
		PluckFrequency(sNotesChromatic[n]);
	}

	if (noteBeatsRemaining > 0 ) {
		noteBeatsRemaining--;

		if (noteBeatsRemaining == 0) {
			noteHertz.Set(0);
			noteSolfege.Set(0);
			HW_Timer1_SetFreq(0);
			HW_Timer1_Enable(false);  // Start Playing
		}
	}
}

#if 0
#define mask_BEEP_PlayNote_NOTE 		(0x07<<4)	//
#define shift_BEEP_PlayNote_NOTE 		4			//

#define mask_BEEP_PlayNote_OCTAVE 		(0x0F<<0)	//
#define shift_BEEP_PlayNote_OCTAVE 		0			//

#define mask_BEEP_PlayTune_TUNENUM 		(0x03<<6)	//
#define shift_BEEP_PlayTune_TUNENUM 	6			//

#define mask_BEEP_PlayTune_STARTLOC		(0x3F<<0)	//
#define mask_BEEP_PlayTune_ENDLOC		(0x3F<<0)	//
#define mask_BEEP_Status_ACTIVE 		(0x01<<0)	//
#define mask_BEEP_Tune_NOTE_LEN 		(0x01<<7)	//
#define mask_BEEP_Tune_PAUSE 			(0x01<<6)	//
#define mask_BEEP_Tune_LENGTH 			(0x0F<<0)	//

#define BEEP_MAXNOTENUM		6
#define BEEP_MAXOCTAVENUM	9
#define BEEP_MAXLENGTHNUM	6
#define BEEP_MAXTUNENUM		3
#define BEEP_MAXTUNELOC		63

// BYTE Locations in registers
#define BEEP_Settings_TEMPO		0


// Frequencies of each Note/OCtave
const uint16_t m_Note[BEEP_MAXNOTENUM+1][BEEP_MAXOCTAVENUM+1] =
{
	{28, 55, 110, 220, 440, 880, 1760, 3520, 7040, 14080},	// A0 ... A9
	{31, 62, 123, 247, 494, 988, 1976, 3951, 7902, 15804},
	{16, 33, 65, 131, 262, 523, 1047, 2093, 4186, 8372},
	{18, 37, 73, 147, 294, 587, 1175, 2349, 4699, 9397},
	{21, 41, 82, 165, 330, 659, 1319, 2637, 5274, 10548},
	{22, 44, 87, 175, 349, 698, 1397, 2794, 5588, 11175},
	{25, 49, 98, 196, 392, 784, 1568, 3136, 6272, 12544},	// G0 ... G9
};

// length of Notes, calculated based on Tempo
uint16_t m_Notelength_ms[BEEP_MAXLENGTHNUM+1];

enum { evtBEEP_NONE, evtBEEP_START, evtBEEP_CANCEL };

//uint8_t m_tune3[] = {100, 52, 36, 20, 4, 52, 36, 20, 4, 52, 36, 20, 36, 4, 52, 52, 100, 52, 36, 20, 4, 52, 36, 20, 4, 52, 36, 20, 36, 4, 52, 52, 68, 68, 68, 36, 20, 4, 100, 100, 4, 20, 4, 68, 84, 52, 52, 68, 68, 36, 20, 4, 100, 52, 4, 52, 52, 255};
uint8_t m_tune3[] = {0x24, 0x34, 0x44, 0x54, 0x64, 0x04, 0x14, 0xC0, 0x82, 0x25, 0x35, 0x45, 0x55, 0x65, 0x05, 0x15, 255};
// 0xC0 = Pause Whole Note, 0xC2 = Set 1/4 Note

void Beep_Stop(void);
void Beep_SetFreq(uint16_t freq);
uint8_t Beep_GetNoteFreq(uint8_t note, uint16_t * pFreq);
void Beep_SetActive(bool active);
void Beep_Tempo(uint8_t bpm);
void Beep_Tune3Default(void);

typedef struct
{
	uint8_t event;		// evtBEEP_xxx
	bool active;
	bool tune;			// 0=note, 1=tune
	uint8_t noteoct;	// copy of b0 _PlayNote
	uint8_t tuneaddr;
	uint8_t tuneloc;
	uint8_t tunestart;
	uint8_t tuneend;

	uint16_t notefreq;
	uint16_t noteduration_ms;
} beep_t;

beep_t m_Beep;


/*----------------------------------------------------------------------------
Name: Beep_Init
Desc:
/ ---------------------------------------------------------------------------*/
void Beep_Init(void)
{
	#define TEMPO_Default	60
	Beep_Tempo(TEMPO_Default); // Setup default tempo

	// Beep_Tune3Default();
}

/*----------------------------------------------------------------------------
Name: Beep_Tempo
Desc: Calculate Note lengths based on bpm
	  Update the SpiMem register
/ ---------------------------------------------------------------------------*/
void Beep_Tempo(uint8_t bpm)
{
	uint16_t len_ms;
	SpiMem_Write(BEEP_Settings, BEEP_Settings_TEMPO, bpm);
	len_ms = (60*1000)/bpm;  // Length of a Whole Note
	for (uint8_t i=0; i < (BEEP_MAXLENGTHNUM+1); i++)
	{
		m_Notelength_ms[i] = len_ms;
		len_ms /= 2;
	}
}


/*----------------------------------------------------------------------------
Name: Beep_Run
Desc: Beep State Machine
	  called every 1msec
	  Plays notes, tunes
/ ---------------------------------------------------------------------------*/
void Beep_Run(void)
{
	enum { sWAITEVENT, sSTOP, sSTART, sPLAYNOTE, sPLAYNOTEWAIT, sPLAYTUNE, sPLAYTUNEWAIT  };
	static uint8_t s_State = sSTOP;
	static uint8_t s_noteoct, s_tuneloc;
	uint8_t b, length;

	// Async Event
	if (m_Beep.event)
	{	// Cancel and stop
		s_State = sWAITEVENT;
	}

	switch(s_State)
	{
	case sWAITEVENT:
		if (m_Beep.event == evtBEEP_CANCEL)
		{	// Cancel and stop
			m_Beep.event = evtBEEP_NONE;
			s_State = sSTOP;
		}

		if (m_Beep.event == evtBEEP_START)
		{	// Start or Restart
			m_Beep.event = evtBEEP_NONE;
			s_State = sSTART;
		}
		break;

	case sSTOP:
		Beep_Stop();
		s_State = sWAITEVENT;
		break;

	case sSTART:
		if (m_Beep.tune)
		{	// Tune
			s_tuneloc = m_Beep.tunestart;	// 1st location
			m_Beep.noteduration_ms = m_Notelength_ms[1]; // Default to Half note
			s_State = sPLAYTUNE;
		}
		else
		{	// Single Note
			s_noteoct = m_Beep.noteoct;	// set 1st note
			s_State = sPLAYNOTE;
		}
		break;

	case sPLAYNOTE:
		/// pull length from spi map
		m_Beep.noteduration_ms = m_Notelength_ms[2]; // Quarter note
		Beep_GetNoteFreq(s_noteoct, &(m_Beep.notefreq));
		Beep_SetFreq(m_Beep.notefreq);	// Starts Playing
		Time_StartTimer(TIMER_NOTE);
		s_State = sPLAYNOTEWAIT;
		break;

	case sPLAYNOTEWAIT:
		if (Time_isTimeout(TIMER_NOTE, m_Beep.noteduration_ms))
		{	// Note is done
			s_State = sSTOP;
		}
		break;

	case sPLAYTUNE:
		// Fetch the noteoct or Pause or Length
		b = SpiMem_Read(m_Beep.tuneaddr, s_tuneloc);
		if ( b==0xFF )
		{	// End of Tune, STOP
			s_State = sSTOP;
		}
		else if ( b & mask_BEEP_Tune_NOTE_LEN )
		{	// This is a Length or Pause
			length = b & mask_BEEP_Tune_LENGTH;
			if (length > BEEP_MAXLENGTHNUM) length = BEEP_MAXLENGTHNUM;
			m_Beep.noteduration_ms = m_Notelength_ms[length];	// Set Duration
			if ( b & mask_BEEP_Tune_PAUSE )
			{	// This is a Pause
				m_Beep.notefreq = 0;
				Beep_SetFreq(0);
				Time_StartTimer(TIMER_NOTE);
				s_State = sPLAYTUNEWAIT; // play silence
			}
			else
			{	// This sets Length
				s_tuneloc++;	// continue to next...
				if (s_tuneloc > m_Beep.tuneend)
				{	// End of Tune Memory - Stop
					s_State = sSTOP;
				}
			}
		}
		else
		{	// This is a note
			Beep_GetNoteFreq(b, &(m_Beep.notefreq));
			Beep_SetFreq(m_Beep.notefreq);	// Starts Playing
			Time_StartTimer(TIMER_NOTE);
			s_State = sPLAYTUNEWAIT;
		}
		break;

	case sPLAYTUNEWAIT:
		// wait for note to complete
		if ( m_Beep.noteduration_ms == 0)
		{	// Continuous, here forever

		}
		else
		{	// play for the duration, and do the next one if applicable
			if (Time_isTimeout(TIMER_NOTE, m_Beep.noteduration_ms))
			{
				s_tuneloc++;
				if (s_tuneloc <= m_Beep.tuneend)
				{
					s_State = sPLAYTUNE;  // go play the next
				}
				else
				{	// we are done
					s_State = sSTOP;
				}
			}
		}
		break;
	}
}

/*----------------------------------------------------------------------------
Name: Beep_Event
Desc: This event is called when BEEP_PlayNote or BEEP_PlayTune register is written
	  Creates an Asynchronous Play or Cancel operation
/ ---------------------------------------------------------------------------*/
void Beep_Event(uint8_t addr)
{
	uint8_t b0;

	switch(addr)
	{
	case BEEP_PlayNote:	// BEEP PlayNote was written
		b0 = SpiMem_Read(BEEP_PlayNote, 0);

		if (b0 == 0xFF)
		{	// Stop everything
			m_Beep.event = evtBEEP_CANCEL;
			return;
		}
		// else ... START

		m_Beep.tune = 0;  // This is a Note
		m_Beep.noteoct = b0;
		m_Beep.event = evtBEEP_START;
		break;
	case BEEP_PlayTune:	// BEEP PlayTune was written
		b0 = SpiMem_Read(BEEP_PlayTune, 0);

		m_Beep.tune = 1;  // This is a Tune
		// calculate Tune addr
		m_Beep.tuneaddr = (b0 & mask_BEEP_PlayTune_TUNENUM) >> shift_BEEP_PlayTune_TUNENUM;
		m_Beep.tuneaddr = BEEP_Tune0 + m_Beep.tuneaddr;
		m_Beep.tunestart = b0 & mask_BEEP_PlayTune_STARTLOC;
		m_Beep.tuneend = b0 & mask_BEEP_PlayTune_ENDLOC;
		if (m_Beep.tuneend==0)
		{	// Play to the end of that Tune memory
			m_Beep.tuneend = BEEP_MAXTUNELOC;
		}
		m_Beep.event = evtBEEP_START;
		break;
	}
}

/*----------------------------------------------------------------------------
Name: Beep_GetNoteFreq
Desc: Retrieve the Note Freq
Ins	: noteoct			Note/Octave
	  *pFreq		Frequency returned here
Outs: return rcXXX
/ ---------------------------------------------------------------------------*/
uint8_t Beep_GetNoteFreq(uint8_t noteoct, uint16_t * pFreq)
{
	uint8_t note, octave;

	note = (noteoct & mask_BEEP_PlayNote_NOTE) >> shift_BEEP_PlayNote_NOTE;
	octave = (noteoct & mask_BEEP_PlayNote_OCTAVE) >> shift_BEEP_PlayNote_OCTAVE;
	// Validate
	if (note > BEEP_MAXNOTENUM) note = BEEP_MAXNOTENUM;
	if (octave > BEEP_MAXOCTAVENUM) note = BEEP_MAXOCTAVENUM;

	// Lookup
	*pFreq = m_Note[note][octave];
	return(rcOK);
}

/*----------------------------------------------------------------------------
Name: Beep_Stop
Desc: Stop playing
/ ---------------------------------------------------------------------------*/
void Beep_Stop(void)
{
	HW_Timer1_Enable(false);  // Stop the beeps
	Beep_SetActive(false);
}

/*----------------------------------------------------------------------------
Name: Beep_SetFreq
Desc: Start playing the specified frequency
	  If freq==0, this is a Pause, don;t play anything
/ ---------------------------------------------------------------------------*/
void Beep_SetFreq(uint16_t freq)
{
	if (freq)
	{
		HW_Timer1_SetFreq(freq);
		HW_Timer1_Enable(true);  // Start Playing
	}
	else
	{
		HW_Timer1_Enable(false);  // Stop Playing
	}
	Beep_SetActive(true);
}

/*----------------------------------------------------------------------------
Name: Beep_SetActive
Desc: Update the ACTIVE bit in the BEEP_Status register
/ ---------------------------------------------------------------------------*/
void Beep_SetActive(bool active)
{
	uint8_t b0 = SpiMem_Read(BEEP_Status, 0);
	m_Beep.active = active;

	if (active) b0 |= mask_BEEP_Status_ACTIVE;
	else        b0 &= ~mask_BEEP_Status_ACTIVE;

	SpiMem_Write_NoTrigger(BEEP_Status, 0, b0);
}

/*----------------------------------------------------------------------------
Name: Beep_Tune3Default
Desc: Write a Tune to Tune[3]
/ ---------------------------------------------------------------------------*/
void Beep_Tune3Default(void)
{
	for (uint8_t i=0; i < sizeof(m_tune3); i++)
	{
		SpiMem_Write(BEEP_Tune3, i, m_tune3[i]);
	}
}
#endif

