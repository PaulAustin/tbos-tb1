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

#include <stdlib.h>
#include "Hardware.h"
#include "SpiRegisterMap.h"
#include "BQ24195.h"
#include "Motor.h"
#include "Encoder.h"

// Motor / encoder speed control using SW DIO for h-bridge control
// and SW DIO for encoder reading. Math indicates that sampling at 4kHz
// should be OK.

uint8_t gPwmWidth;
uint8_t gPwmTic;

MotorManager gMotors;

void MotorManager::Init(void)
{
	gPwmWidth = 10;
	gPwmTic = 0;

	// Clear GPI for the HBridge so no power to the motor.
	GPIO_Write(MOT1_F, 0);
	GPIO_Write(MOT1_R, 0);
	GPIO_Write(MOT2_F, 0);
	GPIO_Write(MOT2_R, 0);
	GPIO_Write(MOT_NSLEEP, 1);

	gRMap.SetValueObj(kRM_Motor1Power, &_m1._power);
	gRMap.SetValueObj(kRM_Motor2Power, &_m2._power);

	gRMap.SetValueObj(kRM_Motor1Break, &_m1._break);
	gRMap.SetValueObj(kRM_Motor2Break, &_m2._break);
}

#define MOTOR_TUNING_INTERVAL 20
#define MOTOR_CPR 420 // counts per revolution.

/*----------------------------------------------------------------------------
Name: Check_Motor_Power
Desc: At a slower rate see if there is a reason to keep the 5V enabled for
the motors.
/ ---------------------------------------------------------------------------*/
bool MotorManager::Idle()
{
	return (_m1._power.Get() == 0 && _m2._power.Get() == 0);
}

/*----------------------------------------------------------------------------
Name: Motor::Run
Desc: Motor/Encoder State Machine
	  Called every 1msec for control operations
	  Operates the motors, read the encoders
/ ---------------------------------------------------------------------------*/
void MotorManager::Run()
{
	_m1.CheckRegs();
	_m2.CheckRegs();
}

void Motor::CheckRegs() {
	// TODO there is bit of a race condition here. If the break is applied
	// and the motor power is set in the same run cycle, the current system
	// can't tell which came last. Priority is place on the break. Applying
	// the break for 5ms or less is fairly pointless as well.

	if (_break.HasAsyncSet()) {
		_power.Set(0);
		SetBreak(_break.Get());
	}
	if (_power.HasAsyncSet()) {
		SetPower(_power.Get());
		_break.Set(0);
	}
}

/*------------------------------------------------------------------
Motor_SetPWM - Motor/Encoder State Machine
	  Called at highest rate possible to poll encoder edges and
	  to generate pulse chain for power control.
/ ----------------------------------------------------------------*/
void Motor::SetPower(int power) {

	int ticEdge = abs(power+5) / 10;

	if (ticEdge > 10)
		ticEdge = 10;

	if (power != 0) {
		BQ_5VUsagePing();
	}

	// pwmHigh is %0-%100 power level, need to map more carefully
	// Set the 0 first to avoid the ISR seeing both set.
	if (power > 0) {
		_pwmHighR = 0;
		_pwmHighF = ticEdge;
	} else if ( power < 0 ){
		_pwmHighF = 0;
		_pwmHighR = ticEdge;
	} else {
		// coast
		_pwmHighR = 0;
		_pwmHighF = 0;
	}
}

/*------------------------------------------------------------------
Motor_SetPWM - Motor/Encoder State Machine
	  Called at highest rate possible to poll encoder edges and
	  to generate pulse chain for power control.
/ ----------------------------------------------------------------*/
void Motor::SetBreak(bool state) {

	// true is break, false is coast
	int v = state ? 10 : 0;

	_pwmHighR = v;
	_pwmHighF = v;
}

/*------------------------------------------------------------------
Name: Motor_RunISR
Desc: Motor/Encoder State Machine
	  Called at highest rate possible to poll encoder edges and
	  to generate pulse chain for power control.
/ ----------------------------------------------------------------*/
void MotorManager::RunISR()
{
	// Set DIO signals for the MP607 motor driver
	//
	// 	IN1		IN2		Out1	OUt2
	//	-------------------------
	//	L	  	L		HiZ		HiZ
	//	L	  	H		Gnd		Vin		Forward
	//	H	  	L		Vin		Gnd		Reverse
	//	H	  	H		Gnd		Gnd		Break
	//
	// 	Note: logic is reversed.
	//	Writing true to register pulls line to low.

	// Motors share the same primary PWM counter, the duty cycle
	// is specific to each motor and direction. The PWM values
	// are configured so the F or R will always be low, unless
	// the motors are stopped with break mode.

	gPwmTic++;
	if (gPwmTic >= gPwmWidth) {
		gPwmTic = 0;
	}
	GPIO_WRITE(gpio_MOT1_F, _m1._pwmHighF > gPwmTic);
	GPIO_WRITE(gpio_MOT1_R, _m1._pwmHighR > gPwmTic);
	GPIO_WRITE(gpio_MOT2_F, _m2._pwmHighF > gPwmTic);
	GPIO_WRITE(gpio_MOT2_R, _m2._pwmHighR > gPwmTic);
}
