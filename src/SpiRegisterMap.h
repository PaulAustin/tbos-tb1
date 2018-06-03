/*============================================================================
 Title	: Spi0.h
 Desc	: SPI on USART0
 2017-05-21	DaraiusH
/ ===========================================================================*/

#ifndef SPIREGISTERMAP_H_
#define SPIREGISTERMAP_H_

#include "Value.h"

enum RegMap_t
{
	kRM_SystemFMVers1 = 1,
	kRM_SystemFMVers2,
	kRM_SystemStatus,
	kRM_SystemIdle,
	kRM_SystemStop,		     // Stop all IO,turn off all GPIO
	kRM_System5VAutoSleep,   // Disable for encoders to work all the time.
	kRM_GroupTillTrigger,
	kRM_Trigger,

	kRM_Motor1Power 	= 10,
	kRM_Motor1Rpm 		= 11,
	kRM_Motor1Break 	= 12,
	kRM_Motor1Cpr 		= 13,
	kRM_Motor1Countdown = 14,
	kRM_Motor1Encoder   = 15,
	// P, I , D  ??

	kRM_Motor2Power 	= 20,
	kRM_Motor2Rpm 		= 21,
	kRM_Motor2Break 	= 22,
	kRM_Motor2Cpr 		= 23,
	kRM_Motor2Countdowm = 24,
	kRM_Motor2Encoder	= 25,
	// P, I , D  ??


	kRM_MotorRpmSampleRate = 33, // typically about 10Hz

	kRM_Servo1Active	= 40, // 0:off,   1:0-2ms, O:custom ???
	kRM_Servo2Active 	= 41,
	kRM_Servo3Active 	= 42,
	// Room for more ...
	kRM_Servo1Position  = 50,
	kRM_Servo2Position  = 51,
	kRM_Servo3Position  = 52,
	// Room for more ...


	kRM_NoteTempo 		= 60,	// default
	kRM_NoteLength 		= 61,
	kRM_NoteSolfege 	= 62,	// piano key 1-88
	kRM_NoteHertz 		= 63,	// 0-14k??

	kRM_Gpio 			= 70,	    // BIT FIELD
	kRM_Gpio1 			= 71,	    // individual bits
	kRM_Gpio2 			= 72,	    // individual bits
	kRM_Gpio3 			= 73,	    // individual bits
	kRM_Gpio4 			= 74,	    // individual bits
	kRM_GpioOutputMask,	// BIT FIELD

	kRM_Count 			= 80,
};

class RegisterMap {
public:
	ATrigger* _registers[kRM_Count];
public:
	void Init();  // global constructors cause problems since they are before main
	void SetValueObj(int id, ATrigger* vobj) { _registers[id] = vobj; }
	void ASet(int id, int value)  { _registers[id]->ASet(value); }
	int Get(int id)  { return _registers[id]->Get(); }
	void WriteDotted(int id, int dotted, int value);
	bool Changed(int id);
};

extern RegisterMap gRMap;

// provide a modes abstraction so a function him can be added, and a flat enum can
// be converted to a object/property hierarchy can be used

#endif // SPIREGISTERMAP_H_
