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




TODO:
 - Figure out how to set the PWM for the servo
*/

#include "Hardware.h"
#include "Servo.h"
#include "SpiRegisterMap.h"
#include "Encoder.h"

#define SERVO_MAXPERIOD_us			20000	// Maximum allowed period

ServoManager gServos;

/*----------------------------------------------------------------------------
Name: Servo_Init
Desc:
/ ---------------------------------------------------------------------------*/
void ServoManager::Init(void)
{
	SetPeriod();

	gRMap.SetValueObj(kRM_Servo1Active, &_servos[kSERVO_1]._active);
	gRMap.SetValueObj(kRM_Servo1Position, &_servos[kSERVO_1]._position);
	//gRMap.SetValueObj(kRM_Servo1MinRange, &_servos[kSERVO_1]._minPosition);
	//gRMap.SetValueObj(kRM_Servo1MaxRange, &_servos[kSERVO_1]._maxPosition);


	gRMap.SetValueObj(kRM_Servo2Active, &_servos[kSERVO_2]._active);
	gRMap.SetValueObj(kRM_Servo2Position, &_servos[kSERVO_2]._position);
	//gRMap.SetValueObj(kRM_Servo2MinRange, &_servos[kSERVO_2]._minPosition);
	//gRMap.SetValueObj(kRM_Servo2MaxRange, &_servos[kSERVO_2]._maxPosition);

	gRMap.SetValueObj(kRM_Servo3Active, &_servos[kSERVO_3]._active);
	gRMap.SetValueObj(kRM_Servo3Position, &_servos[kSERVO_3]._position);
	//gRMap.SetValueObj(kRM_Servo3MinRange, &_servos[kSERVO_3]._minPosition);
	//gRMap.SetValueObj(kRM_Servo3MaxRange, &_servos[kSERVO_3]._maxPosition);

	//gRMap.SetValueObj(kRM_Gpio, &_gpio);
#if 0
	uint16_t period_us;
	// Set Servo Period, typical is about 20ms.
	period_us = SERVO_MAXPERIOD_us;

#endif
}

/*----------------------------------------------------------------------------
Name: Servo_Run
Desc: Servo State Machine
	  called every 1msec
	  Operates the servos
/ ---------------------------------------------------------------------------*/
void ServoManager::Run(void)
{

	for (int ch=kSERVO_1; ch < kSERVO_Count; ch++)
	{

		// The HW timer channel match the Servo enum. 0, 1, 2
		if(_servos[ch]._position.HasAsyncSet()){
			HW_Timer2_SetPW_us(ch, _servos[ch]._position.Get());
		}
//		Servo_SetPW_us(ch, 10 /* _servos[ch].pw_us*/ );
	}
/*
	if (_gpio.HasAsyncSet()) {
		// If the bit field has change update all Io
		int bits = _gpio.Get();
		GPIO_Write(O4, bits & 0x01 ? 1 : 0);
		GPIO_Write(O5, bits & 0x02 ? 1 : 0);
		GPIO_Write(IO6, bits & 0x04 ? 1 : 0);
		GPIO_Write(IO7, bits & 0x08 ? 1 : 0);

		// GPIO_Write(IO8, bits & 0x04 ? 1 : 0);
		// GPIO_Write(IO9, bits & 0x08 ? 1 : 0);
	}
*/


	//kRM_Gpio1

#if 0
	uint8_t ch;

	// Read settings from the SPI register map
	// 1. PWM position +/- 100
	// 2. refresh rate pules per ms ( default 20)
	// 3. servo deactivated.  ( no pulse) a bit field so all on/off at once

	// For each Servo...
	for (ch=SERVOch1; ch <= SERVOch3; ch++)
	{
		// Check if Run flag is set
		if (m_Servo[ch].run)
		{	// Start
			m_Servo[ch].run = false;	// reset the flag once acted on..
			Servo_GetSetting(ch, &m_Servo[ch].pw_us, &m_Servo[ch].duration_ms);
			Servo_SetPeriod();	// Read latest from SpiMem:SERVO_Period
			Servo_SetPW_us(ch, m_Servo[ch].pw_us);
		}
		// Check if Stop flag is set
		if (m_Servo[ch].stop)
		{	// Stop
			m_Servo[ch].stop = false;
			Servo_Stop(ch);
		}
	}
#endif

}

/*----------------------------------------------------------------------------
Name: Servo_Stop
Desc: Stop playing (Bad Description)
Ins	: ch	SERVO1/2/3
/ ---------------------------------------------------------------------------*/
void ServoManager::Stop(int ch)
{
	HW_Timer2_SetPW_us(ch, 0);
}

/*----------------------------------------------------------------------------
Name: Servo_SetPeriod
Desc: Set the common PWM period for all 3 Servos
/ ---------------------------------------------------------------------------*/
void ServoManager::SetPeriod()
{
	// Currently hardcoded to 10msec
	HW_Timer2_SetPeriod_us(10000);

	//TODO: Replace above with a Read from a register
}




/*----------------------------------------------------------------------------
Name: SetMinRange
Desc: Sets the min range that a servo can go
Ins	: min   Minimum range that a servo will go
/ ---------------------------------------------------------------------------*/
/*
void Servo::SetMinRange(int min)
{
	if(min > 0 && min < _maxPosition.Get())
	{
	_minPosition.Set(min);
	}
}
*/

/*----------------------------------------------------------------------------
Name: SetMaxRange
Desc: Sets the max range that a servo can go
Ins	: max   Maximum range that a servo will go
/ ---------------------------------------------------------------------------*/
/*
void Servo::SetMaxRange(int max)
{
	if(max > _minPosition.Get())
	{
	_maxPosition.Set(max);
	}
}
*/
/*----------------------------------------------------------------------------
Name: SetPosition
Desc: Sets the position of the servo in PWM Units
	  and they must be between min ad max positions
Ins	: ch   SERVO1/2/3
	  pos  Position of the servo
/ ---------------------------------------------------------------------------*/
/*
void ServoManager::SetPosition(int ch, int pos)
{
	//rework so if they go over, then it'll snap to min max
	if(pos > _servos[ch]._minPosition.Get() && pos < _servos[ch]._maxPosition.Get())
	{
		HW_Timer2_SetPW_us(ch, pos);
	}

}
*/

/*----------------------------------------------------------------------------
Name: SetPosition
Desc: Sets the position of the servo in PWM Units
	  and they must be between min ad max positions
Ins	: ch   SERVO1/2/3
	  pos  Position of the servo
/ ---------------------------------------------------------------------------*/
/*
void Servo::CheckRegs(int servoNum)
{
	if(_position.HasAsyncSet()){
		gServos.SetPosition(servoNum, _position.Get());
	}
	if(_active.HasAsyncSet()){

	}
	if(_minPosition.HasAsyncSet()){
		SetMinRange(_minPosition.Get());
	}
	if(_maxPosition.HasAsyncSet()){
		SetMaxRange(_maxPosition.Get());
	}
}
*/
















